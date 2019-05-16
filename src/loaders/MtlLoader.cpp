#include "MtlLoader.h"

#include "../Logger.h"

#include <glm/glm.hpp>

static inline glm::vec3 to_glmvec3(const std::vector<std::string>& args)
{
    return glm::vec3(std::stof(args.at(0)), std::stof(args.at(1)), std::stof(args.at(2)));
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
        mtl.shininess     = std::stof(args.at(0));
    } else if (cmd == "map_Ka" || cmd == "map_Kd" || cmd == "map_Ks" || cmd == "map_Kn" ||
               cmd == "cube_Ka" || cmd == "cube_Kd" || cmd == "cube_Ks" || cmd == "cube_Kn") {
        MaterialData& mtl = m_materials.back();
        TextureData tex   = toTextureData(cmd, args);
        mtl.textures.push_back(tex);
    } else {
        LOG_WARNING("Unknown command: {}", cmd);
    }
}

TextureData MtlLoader::toTextureData(const std::string& /*cmd*/,
                                     const std::vector<std::string>& args) const
{
    TextureData texData;
    std::string currOpt;

    for (std::size_t i = 0; i < args.size(); ++i) {
        if (args.at(i)[0] == '-') {
            currOpt = args.at(i);
            continue;
        }

        if (currOpt == "-clamp") {
            texData.clamp = args.at(i) == "on";
        } else if (currOpt == "-cc") {
            texData.linearColor = args.at(i) != "on";
        } else {
            texData.filename = args.at(i);
        }

        currOpt = "";
    }

    texData.name = texData.filename;
    return texData;
}
