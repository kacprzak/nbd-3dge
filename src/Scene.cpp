#include "Scene.h"

#include "MeshData.h"
#include "Util.h"

#include <fx/gltf.h>

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
    }
}

void Scene::load(const std::string& file)
{
    fx::gltf::Document doc = fx::gltf::LoadFromText(file);

    for (auto& bv : doc.bufferViews) {
        auto gpuBuffer = std::make_shared<Buffer>();

        gpuBuffer->loadData(reinterpret_cast<uint8_t*>(doc.buffers[bv.buffer].data.data()) +
                                bv.byteOffset,
                            bv.byteLength);
        gpuBuffer->m_byteStride = bv.byteStride;

        m_buffers.push_back(gpuBuffer);
    }

    for (auto& acc : doc.accessors) {
        Accessor accessor;

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

    for (auto& smpl : doc.samplers) {
        auto sampler = std::make_shared<Sampler>();

        sampler->setParameter(GL_TEXTURE_WRAP_S, static_cast<GLint>(smpl.wrapS));
        sampler->setParameter(GL_TEXTURE_WRAP_T, static_cast<GLint>(smpl.wrapT));
        sampler->setParameter(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(smpl.magFilter));
        sampler->setParameter(GL_TEXTURE_MIN_FILTER, static_cast<GLint>(smpl.minFilter));

        m_samplers.push_back(sampler);
    }

    for (auto& txr : doc.textures) {
        const auto filename =
            extractDirectory(file) + changeExtension(doc.images[txr.source].uri, ".ktx");

        auto texture = std::make_shared<Texture>(filename.c_str(), txr.name);

        if (txr.sampler != -1) {
            texture->setSampler(m_samplers[txr.sampler]);
        }

        m_textures.push_back(texture);
    }

    for (auto& mtl : doc.materials) {
        Material material;

        // BaseColor
        for (int i = 0; i < 4; ++i)
            material.baseColorFactor[i] = mtl.pbrMetallicRoughness.baseColorFactor[i];

        if (!mtl.pbrMetallicRoughness.baseColorTexture.empty())
            material.textures[Material::Unit::BaseColor] =
                m_textures.at(mtl.pbrMetallicRoughness.baseColorTexture.index);

        // Normal
        if (!mtl.normalTexture.empty()) {
            material.normalScale                      = mtl.normalTexture.scale;
            material.textures[Material::Unit::Normal] = m_textures.at(mtl.normalTexture.index);
        }

        // MetallicRoughness
        material.metallicFactor  = mtl.pbrMetallicRoughness.metallicFactor;
        material.roughnessFactor = mtl.pbrMetallicRoughness.roughnessFactor;
        if (!mtl.pbrMetallicRoughness.metallicRoughnessTexture.empty()) {
            material.textures[Material::Unit::MetallicRoughness] =
                m_textures.at(mtl.pbrMetallicRoughness.metallicRoughnessTexture.index);
        }

        // Ambient Occlusion
        if (!mtl.occlusionTexture.empty()) {
            material.occlusionStrength = mtl.occlusionTexture.strength;
            material.textures[Material::Unit::Occlusion] =
                m_textures.at(mtl.occlusionTexture.index);
        }

        // Emissive
        for (int i = 0; i < 3; ++i)
            material.emissiveFactor[i] = mtl.emissiveFactor[i];
        if (!mtl.emissiveTexture.empty()) {
            material.textures[Material::Unit::Emissive] = m_textures.at(mtl.emissiveTexture.index);
        }

        material.name = mtl.name;

        m_materials.push_back(material);
    }

    for (auto& mesh : doc.meshes) {
        for (auto& subMesh : mesh.primitives) {

            std::array<Accessor, Accessor::Attribute::Size> attributes{};
            auto primitive = static_cast<GLenum>(subMesh.mode);

            Accessor& indices = m_accessors[subMesh.indices];

            auto attr = subMesh.attributes.find("POSITION");
            if (attr != std::end(subMesh.attributes))
                attributes[Accessor::Attribute::Position] = m_accessors[attr->second];

            attr = subMesh.attributes.find("NORMAL");
            if (attr != std::end(subMesh.attributes))
                attributes[Accessor::Attribute::Normal] = m_accessors[attr->second];

            attr = subMesh.attributes.find("TANGENT");
            if (attr != std::end(subMesh.attributes))
                attributes[Accessor::Attribute::Tangent] = m_accessors[attr->second];

            attr = subMesh.attributes.find("TEXCOORD_0");
            if (attr != std::end(subMesh.attributes))
                attributes[Accessor::Attribute::TexCoord_0] = m_accessors[attr->second];

            auto m = std::make_shared<Mesh>(attributes, indices, primitive);

            if (subMesh.material != -1) {
                m->setMaterial(m_materials.at(subMesh.material));
            }

            m_meshes.push_back(m);
        }
    }

    for (auto& cam : doc.cameras) {
        Camera camera;
        camera.setAspectRatio(cam.perspective.aspectRatio);
        camera.setPerspective(cam.perspective.yfov, cam.perspective.znear, cam.perspective.zfar);

        m_cameras.push_back(std::move(camera));
    }

    for (auto& n : doc.nodes) {
        static int actorId = 0;
        RenderNode node{actorId++};

        node.setTranslation({n.translation[0], n.translation[1], n.translation[2]});
        node.setScale({n.scale[0], n.scale[1], n.scale[2]});
        node.setRotation({n.rotation[3], n.rotation[0], n.rotation[1], n.rotation[2]});

        if (n.mesh != -1) node.setMesh(m_meshes.at(n.mesh));
        if (n.camera != -1) node.setCamera(&m_cameras.at(n.camera));

        node.name = n.name;

        m_nodes.push_back(std::move(node));
    }

    for (auto i = 0u; i < doc.nodes.size(); ++i) {
        const auto& gltfNode = doc.nodes[i];
        auto& oglNode        = m_nodes[i];

        for (auto nodeIdx : gltfNode.children) {
            oglNode.addChild(&m_nodes[nodeIdx]);
        }
    }

    for (auto nodeIdx : doc.scenes[doc.scene].nodes) {
        m_scene.push_back(&m_nodes[nodeIdx]);
    }
}

void Scene::draw(ShaderProgram* shaderProgram, const Camera* camera, std::array<Light*, 8>& lights,
                 Texture* environment)
{
    glm::mat4 identity{1.0f};

    for (auto n : m_scene)
        n->draw(identity, shaderProgram, camera, lights, environment);
}

void Scene::drawAabb(ShaderProgram* shaderProgram, const Camera* camera)
{
    glm::mat4 identity{1.0f};

    for (auto n : m_scene)
        n->drawAabb(identity, shaderProgram, camera);
}

void Scene::update(float delta)
{
    glm::mat4 identity{1.0f};

    for (auto n : m_scene)
        n->update(identity, delta);
}

RenderNode* Scene::findNode(const std::string& node)
{
    for (auto& n : m_nodes) {
        if (n.name == node) return &n;
    }
    return nullptr;
}

Aabb Scene::aabb() const
{
    Aabb aabb;
    for (const auto& n : m_nodes) {
        aabb = aabb.mbr(n.aabb());
    }
    return aabb;
}
