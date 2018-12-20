#include "Scene.h"

#include "MeshData.h"
#include "Util.h"

#include <fx/gltf.h>

Accessor calculateTangents(const std::array<Accessor, Accessor::Attribute::Size>& attributes, const Accessor& indicesAcc)
{
    auto indices   = indicesAcc.getData<uint16_t>();
    auto positions = attributes[Accessor::Attribute::Normal].getData<glm::vec3>();
    auto normals   = attributes[Accessor::Attribute::Normal].getData<glm::vec3>();
    auto texcoords = attributes[Accessor::Attribute::TexCoord_0].getData<glm::vec2>();

    std::vector<glm::vec3> tangents;

    if (texcoords.empty()) return {};

    tangents.resize(normals.size());

    auto inc = 3; // Only GL_TRIANGLES supported

    for (size_t i = 2; i < indices.size(); i += inc) {
        auto index0 = indices[i - 2];
        auto index1 = indices[i - 1];
        auto index2 = indices[i - 0];
        // Shortcuts for positions
        glm::vec3 v0 = positions[index0];
        glm::vec3 v1 = positions[index1];
        glm::vec3 v2 = positions[index2];

        // Shortcuts for UVs
        glm::vec2 st0 = texcoords[index0];
        glm::vec2 st1 = texcoords[index1];
        glm::vec2 st2 = texcoords[index2];

        // Edges of the triangle : postion delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // ST delta
        glm::vec2 deltaST1 = st1 - st0;
        glm::vec2 deltaST2 = st2 - st0;

        float r           = 1.0f / (deltaST1.x * deltaST2.y - deltaST1.y * deltaST2.x);
        glm::vec3 tangent = (deltaPos1 * deltaST2.y - deltaPos2 * deltaST1.y) * r;
        // glm::vec3 bitangent = (deltaPos2 * deltaST1.x - deltaPos1 * deltaST2.x) * r;

        tangent = glm::normalize(tangent);

        tangents[index0] = tangent;
        tangents[index1] = tangent;
        tangents[index2] = tangent;
    }

	auto tangentsBuffer = std::make_shared<Buffer>();
    tangentsBuffer->loadData(tangents.data(), tangents.size() * sizeof(tangents[0]));
    
	Accessor tangentsAccesor;
    tangentsAccesor.buffer = tangentsBuffer;
    tangentsAccesor.count  = tangents.size();
    tangentsAccesor.type   = GL_FLOAT;
    tangentsAccesor.size   = sizeof(tangents[0]) / sizeof(tangents[0][0]);

    return tangentsAccesor;
}

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

void Scene::load(const std::filesystem::path& file)
{
    fx::gltf::Document doc = fx::gltf::LoadFromText(file.string());

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
        if (smpl.magFilter != fx::gltf::Sampler::MagFilter::None)
            sampler->setParameter(GL_TEXTURE_MAG_FILTER, static_cast<GLint>(smpl.magFilter));
        if (smpl.minFilter != fx::gltf::Sampler::MinFilter::None)
            sampler->setParameter(GL_TEXTURE_MIN_FILTER, static_cast<GLint>(smpl.minFilter));

        m_samplers.push_back(sampler);
    }

    for (auto& txr : doc.textures) {
        const auto filename =
            file.parent_path() /
            std::filesystem::path(doc.images[txr.source].uri).replace_extension(".ktx");

        auto texture = std::make_shared<Texture>(filename, txr.name);

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
            if (attr != std::end(subMesh.attributes)) {
                attributes[Accessor::Attribute::Tangent] = m_accessors[attr->second];
            } 

            attr = subMesh.attributes.find("TEXCOORD_0");
            if (attr != std::end(subMesh.attributes))
                attributes[Accessor::Attribute::TexCoord_0] = m_accessors[attr->second];

			// Missing tangent vectors!
			if (!attributes[Accessor::Attribute::Tangent].buffer)
                attributes[Accessor::Attribute::Tangent] = calculateTangents(attributes, indices);

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
                 const TexturePack& environment)
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
