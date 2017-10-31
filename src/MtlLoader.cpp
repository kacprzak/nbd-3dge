#include "MtlLoader.h"

#include "Util.h"

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
        mtl.shininess     = to_int(args.at(0));
    } else if (cmd == "map_Ka") {
        MaterialData& mtl = m_materials.back();
        mtl.ambientTex    = args.back();
    } else if (cmd == "map_Kd") {
        MaterialData& mtl = m_materials.back();
        mtl.diffuseTex    = args.back();
    } else if (cmd == "map_Ks") {
        MaterialData& mtl = m_materials.back();
        mtl.specularTex   = args.back();
    } else if (cmd == "map_Kn") {
        MaterialData& mtl = m_materials.back();
        mtl.normalsTex    = args.back();
    }
}
