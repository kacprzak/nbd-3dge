#include "Scene.h"

#include "MeshData.h"

#include <fx/gltf.h>

void Scene::load(const std::string& file)
{
    fx::gltf::Document doc = fx::gltf::LoadFromText(file);

    for (auto& bv : doc.bufferViews) {
        Buffer gpuBuffer{static_cast<GLenum>(bv.target)};

        gpuBuffer.loadData(reinterpret_cast<uint8_t*>(doc.buffers[bv.buffer].data.data()) +
                               bv.byteOffset,
                           bv.byteLength);
        gpuBuffer.m_byteStride = bv.byteStride;

        m_buffers.emplace_back(std::move(gpuBuffer));
    }

    for (auto& acc : doc.accessors) {
        Accessor accessor;

        accessor.buffer     = &m_buffers[acc.bufferView];
        accessor.byteOffset = acc.byteOffset;
        accessor.count      = acc.count;
        accessor.size       = acc.min.size();
        accessor.type       = static_cast<GLenum>(acc.componentType);
        accessor.normalized = acc.normalized;
        std::copy(std::begin(acc.min), std::end(acc.min), std::begin(accessor.min));
        std::copy(std::begin(acc.max), std::end(acc.max), std::begin(accessor.max));

        m_accessors.push_back(accessor);
    }

    for (auto& mesh : doc.meshes) {
        for (auto& subMesh : mesh.primitives) {
            MeshData md;
            md.primitive = static_cast<GLenum>(subMesh.mode);

            md.iindices = &m_accessors[subMesh.indices];

            auto attr = subMesh.attributes.find("POSITION");
            if (attr != std::end(subMesh.attributes))
                md.attributes[MeshData::Attributes::Position] = &m_accessors[attr->second];

            attr = subMesh.attributes.find("NORMAL");
            if (attr != std::end(subMesh.attributes))
                md.attributes[MeshData::Attributes::Normal] = &m_accessors[attr->second];

            attr = subMesh.attributes.find("TANGENT");
            if (attr != std::end(subMesh.attributes))
                md.attributes[MeshData::Attributes::Tangent] = &m_accessors[attr->second];

            attr = subMesh.attributes.find("TEXCOORD_0");
            if (attr != std::end(subMesh.attributes))
                md.attributes[MeshData::Attributes::TexCoord_0] = &m_accessors[attr->second];

            m_meshes.push_back(std::make_shared<Mesh>(md));
        }
    }

    for (auto& cam : doc.cameras) {
        Camera camera{{200, 200}};
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

void Scene::draw(ShaderProgram* shaderProgram, const Camera* camera, std::array<Light*, 8>& lights)
{
    glm::mat4 identity{1.0f};

    for (auto n : m_scene)
        n->draw(identity, shaderProgram, &m_cameras[0], lights, nullptr);
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
