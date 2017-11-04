#include "MtlLoader.h"

#include "Util.h"
#include "Logger.h"

#include <glm/glm.hpp>

static inline glm::vec3 to_glmvec3(const std::vector<std::string>& args)
{
    return glm::vec3(to_float(args.at(0)), to_float(args.at(1)), to_float(args.at(2)));
}

void MtlLoader::command(const std::string& cmd, const std::vector<std::string>& args)
{
    if (cmd == "newmtl") {
        MaterialData mtl;
        mtl.name = args.at(0);
        m_materials.push_back(mtl);
    } else if (cmd == "Ka") {
        MaterialData& mtl = m_materials.back();
        mtl.ambient       = to_glmvec3(args);
    } else if (cmd == "Kd") {
        MaterialData& mtl = m_materials.back();
        mtl.diffuse       = to_glmvec3(args);
    } else if (cmd == "Ks") {
        MaterialData& mtl = m_materials.back();
        mtl.specular      = to_glmvec3(args);
    } else if (cmd == "Ns") {
        MaterialData& mtl = m_materials.back();
        mtl.shininess     = to_float(args.at(0));
    } else if (cmd == "map_Ka" || cmd == "map_Kd") {
        MaterialData& mtl = m_materials.back();
        TextureData tex;
        tex.filename    = args.back();
        tex.linearColor = false;
        mtl.textures.push_back(tex);
    } else if (cmd == "map_Ks" || cmd == "map_Kn") {
        MaterialData& mtl = m_materials.back();
        TextureData tex;
        tex.filename    = args.back();
        tex.linearColor = true;
        mtl.textures.push_back(tex);
    } else {
        LOG_WARNING << "Unknown command: " << cmd;
    }
}
