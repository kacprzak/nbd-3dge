#include "GltfLoader.h"

#include <fx/gltf.h>

namespace loaders {

int typeToSize(fx::gltf::Accessor::Type type)
{
    switch (type) {
    case fx::gltf::Accessor::Type::None: return -1;
    case fx::gltf::Accessor::Type::Scalar: return 1;
    case fx::gltf::Accessor::Type::Vec2: return 2;
    case fx::gltf::Accessor::Type::Vec3: return 3;
    case fx::gltf::Accessor::Type::Vec4: return 4;
    case fx::gltf::Accessor::Type::Mat2: return 4;
    case fx::gltf::Accessor::Type::Mat3: return 9;
    case fx::gltf::Accessor::Type::Mat4: return 16;
    default: throw std::invalid_argument("Unknown type");
    }
}

//------------------------------------------------------------------------------

void GltfLoader::load(const std::filesystem::path& file)
{
    using namespace gfx;

    fx::gltf::Document doc = fx::gltf::LoadFromText(file.string());

    loadBuffers(doc);
    loadAccessors(doc);
    loadSamplers(doc);
    loadTextures(doc, file);
    loadMaterials(doc);
    loadMeshes(doc);
    loadAnimations(doc);
    loadCameras(doc);
    loadNodes(doc);
    loadSkins(doc);

    for (auto& scene : doc.scenes) {
        m_scenes.push_back(scene.nodes);
    }

    m_name = file.filename().string();
}

//------------------------------------------------------------------------------

std::shared_ptr<gfx::Model> GltfLoader::model() const
{
    auto model = std::make_shared<gfx::Model>();

    model->m_buffers    = m_buffers;
    model->m_samplers   = m_samplers;
    model->m_textures   = m_textures;
    model->m_meshes     = m_meshes;
    model->m_animations = m_animations;
    model->m_skins      = m_skins;

    model->m_nodes = m_nodes;
    for (auto& n : model->m_nodes)
        n.setModel(model.get());

    model->m_scenes = m_scenes;
    model->name     = m_name;

    return model;
}

//------------------------------------------------------------------------------

void GltfLoader::loadBuffers(const fx::gltf::Document& doc)
{
    for (auto& bv : doc.bufferViews) {
        auto gpuBuffer = std::make_shared<gfx::Buffer>();

        gpuBuffer->loadData(reinterpret_cast<const uint8_t*>(doc.buffers[bv.buffer].data.data()) +
                                bv.byteOffset,
                            bv.byteLength);
        gpuBuffer->m_byteStride = bv.byteStride;

        m_buffers.push_back(gpuBuffer);
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadAccessors(const fx::gltf::Document& doc)
{
    for (auto& acc : doc.accessors) {
        gfx::Accessor accessor;

        accessor.buffer     = m_buffers[acc.bufferView];
        accessor.byteOffset = acc.byteOffset;
        accessor.count      = acc.count;
        accessor.size       = typeToSize(acc.type);
        accessor.type       = static_cast<GLenum>(acc.componentType);
        accessor.normalized = acc.normalized;
        std::copy(std::begin(acc.min), std::end(acc.min), std::begin(accessor.min));
        std::copy(std::begin(acc.max), std::end(acc.max), std::begin(accessor.max));

        m_accessors.push_back(accessor);
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadSamplers(const fx::gltf::Document& doc)
{
    for (auto& smpl : doc.samplers) {
        auto sampler = std::make_shared<gfx::Sampler>();

        sampler->setParameter(GL_TEXTURE_WRAP_S, static_cast<GLint>(smpl.wrapS));
        sampler->setParameter(GL_TEXTURE_WRAP_T, static_cast<GLint>(smpl.wrapT));
        if (smpl.magFilter != fx::gltf::Sampler::MagFilter::None)
            sampler->setParameter(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(smpl.magFilter));
        if (smpl.minFilter != fx::gltf::Sampler::MinFilter::None)
            sampler->setParameter(GL_TEXTURE_MIN_FILTER, static_cast<GLint>(smpl.minFilter));

        m_samplers.push_back(sampler);
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadTextures(const fx::gltf::Document& doc, const std::filesystem::path& file)
{
    for (auto& txr : doc.textures) {
        const auto filename =
            file.parent_path() /
            std::filesystem::path(doc.images[txr.source].uri).replace_extension(".ktx");

        auto texture = std::make_shared<gfx::Texture>(filename, txr.name);

        if (txr.sampler != -1) {
            texture->setSampler(m_samplers[txr.sampler]);
        }

        m_textures.push_back(texture);
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadMaterials(const fx::gltf::Document& doc)
{
    using namespace gfx;

    auto white = std::make_shared<Texture>(glm::vec3{1.0f, 1.0f, 1.0f});
    auto blue  = std::make_shared<Texture>(glm::vec3{0.5f, 0.5f, 1.0f});

    for (auto& mtl : doc.materials) {
        Material material;
        material.textures[TextureUnit::BaseColor]         = white;
        material.textures[TextureUnit::Normal]            = blue;
        material.textures[TextureUnit::MetallicRoughness] = white;
        material.textures[TextureUnit::Occlusion]         = white;
        material.textures[TextureUnit::Emissive]          = white;

        // BaseColor
        for (int i = 0; i < 4; ++i)
            material.baseColorFactor[i] = mtl.pbrMetallicRoughness.baseColorFactor[i];

        if (!mtl.pbrMetallicRoughness.baseColorTexture.empty())
            material.textures[TextureUnit::BaseColor] =
                m_textures.at(mtl.pbrMetallicRoughness.baseColorTexture.index);

        // Normal
        if (!mtl.normalTexture.empty()) {
            material.normalScale                   = mtl.normalTexture.scale;
            material.textures[TextureUnit::Normal] = m_textures.at(mtl.normalTexture.index);
        }

        // MetallicRoughness
        material.metallicFactor  = mtl.pbrMetallicRoughness.metallicFactor;
        material.roughnessFactor = mtl.pbrMetallicRoughness.roughnessFactor;
        if (!mtl.pbrMetallicRoughness.metallicRoughnessTexture.empty()) {
            material.textures[TextureUnit::MetallicRoughness] =
                m_textures.at(mtl.pbrMetallicRoughness.metallicRoughnessTexture.index);
        }

        // Ambient Occlusion
        if (!mtl.occlusionTexture.empty()) {
            material.occlusionStrength                = mtl.occlusionTexture.strength;
            material.textures[TextureUnit::Occlusion] = m_textures.at(mtl.occlusionTexture.index);
        }

        // Emissive
        for (int i = 0; i < 3; ++i)
            material.emissiveFactor[i] = mtl.emissiveFactor[i];
        if (!mtl.emissiveTexture.empty()) {
            material.textures[TextureUnit::Emissive] = m_textures.at(mtl.emissiveTexture.index);
        }

        material.name = mtl.name;

        m_materials.push_back(material);
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadMeshes(const fx::gltf::Document& doc)
{
    using namespace gfx;

    for (auto& mesh : doc.meshes) {

        std::vector<Primitive> primitives;

        for (auto& prim : mesh.primitives) {

            std::array<Accessor, Accessor::Attribute::Size> attributes{};
            auto primitive = static_cast<GLenum>(prim.mode);

            Accessor& indices = m_accessors[prim.indices];

            auto attr = prim.attributes.find("POSITION");
            if (attr != std::end(prim.attributes))
                attributes[Accessor::Attribute::Position] = m_accessors[attr->second];

            attr = prim.attributes.find("NORMAL");
            if (attr != std::end(prim.attributes))
                attributes[Accessor::Attribute::Normal] = m_accessors[attr->second];

            attr = prim.attributes.find("TANGENT");
            if (attr != std::end(prim.attributes)) {
                attributes[Accessor::Attribute::Tangent] = m_accessors[attr->second];
            }

            attr = prim.attributes.find("TEXCOORD_0");
            if (attr != std::end(prim.attributes))
                attributes[Accessor::Attribute::TexCoord_0] = m_accessors[attr->second];

            attr = prim.attributes.find("COLOR_0");
            if (attr != std::end(prim.attributes))
                attributes[Accessor::Attribute::Color_0] = m_accessors[attr->second];

            attr = prim.attributes.find("JOINTS_0");
            if (attr != std::end(prim.attributes))
                attributes[Accessor::Attribute::Joints_0] = m_accessors[attr->second];

            attr = prim.attributes.find("WEIGHTS_0");
            if (attr != std::end(prim.attributes))
                attributes[Accessor::Attribute::Weights_0] = m_accessors[attr->second];

            // Missing tangent vectors!
            if (!attributes[Accessor::Attribute::Tangent].buffer)
                attributes[Accessor::Attribute::Tangent] = calculateTangents(attributes, indices);

            std::vector<std::array<Accessor, 3>> targets;

            for (auto& target : prim.targets) {
                std::array<Accessor, 3> attributes{};

                auto attr = target.find("POSITION");
                if (attr != std::end(target))
                    attributes[Accessor::Attribute::Position] = m_accessors[attr->second];

                attr = target.find("NORMAL");
                if (attr != std::end(target))
                    attributes[Accessor::Attribute::Normal] = m_accessors[attr->second];

                attr = target.find("TANGENT");
                if (attr != std::end(target)) {
                    attributes[Accessor::Attribute::Tangent] = m_accessors[attr->second];
                }

                targets.push_back(attributes);
            }

            primitives.emplace_back(attributes, indices, primitive, targets);
            if (prim.material != -1) {
                primitives.back().setMaterial(m_materials.at(prim.material));
            }
        }

        auto m = std::make_shared<Mesh>(std::move(primitives));
        m_meshes.push_back(m);
        m_meshes.back()->name = mesh.name;
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadAnimations(const fx::gltf::Document& doc)
{
    const auto toInterpolation = [](fx::gltf::Animation::Sampler::Type interpolationType) {
        switch (interpolationType) {
        case fx::gltf::Animation::Sampler::Type::Step:
            return gfx::Animation::Sampler::Interpolation::Step;
        case fx::gltf::Animation::Sampler::Type::CubicSpline:
            return gfx::Animation::Sampler::Interpolation::CubicSpline;
        default: return gfx::Animation::Sampler::Interpolation::Linear;
        }
    };

    const auto toPath = [](const std::string& path) {
        if (path == "rotation")
            return gfx::Animation::Channel::Path::Rotation;
        else if (path == "scale")
            return gfx::Animation::Channel::Path::Scale;
        else if (path == "weights")
            return gfx::Animation::Channel::Path::Weights;
        return gfx::Animation::Channel::Path::Translation;
    };

    for (auto& animation : doc.animations) {
        std::vector<gfx::Animation::Sampler> samplers;
        for (const auto& samp : animation.samplers) {
            gfx::Animation::Sampler sampler;
            sampler.interpolation = toInterpolation(samp.interpolation);
            sampler.input         = m_accessors[samp.input];
            sampler.output        = m_accessors[samp.output];
            samplers.push_back(sampler);
        }

        std::vector<gfx::Animation::Channel> channels;
        for (const auto& chan : animation.channels) {
            gfx::Animation::Channel channel;
            channel.node    = chan.target.node;
            channel.path    = toPath(chan.target.path);
            channel.sampler = samplers.at(chan.sampler);
            channels.push_back(channel);
        }

        m_animations.emplace_back(channels);
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadCameras(const fx::gltf::Document& doc)
{
    for (auto& cam : doc.cameras) {
        gfx::Camera camera;
        camera.setAspectRatio(cam.perspective.aspectRatio);
        camera.setPerspective(cam.perspective.yfov, cam.perspective.znear, cam.perspective.zfar);

        m_cameras.push_back(std::move(camera));
    }
}

//------------------------------------------------------------------------------

void GltfLoader::loadNodes(const fx::gltf::Document& doc)
{
    for (auto& n : doc.nodes) {
        gfx::Node node;

        node.setTranslation({n.translation[0], n.translation[1], n.translation[2]});
        node.setScale({n.scale[0], n.scale[1], n.scale[2]});
        node.setRotation({n.rotation[3], n.rotation[0], n.rotation[1], n.rotation[2]});

        node.setMesh(n.mesh);
        node.setCamera(n.camera);
        node.setSkin(n.skin);

        node.name = n.name;

        m_nodes.push_back(std::move(node));
    }

    for (auto i = 0u; i < doc.nodes.size(); ++i) {
        const auto& gltfNode = doc.nodes[i];

        for (auto nodeIdx : gltfNode.children) {
            m_nodes[i].addChild(nodeIdx);
        }
    }
}

void GltfLoader::loadSkins(const fx::gltf::Document& doc)
{
    for (auto& s : doc.skins) {
        gfx::Skin skin;

        skin.setIBMatrix(m_accessors[s.inverseBindMatrices]);
        skin.setJoints(s.joints, s.skeleton);
        skin.name = s.name;

        m_skins.push_back(skin);
    }
}

} // namespace loaders
