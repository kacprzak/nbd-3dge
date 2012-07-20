#include "mtlloader.h"

#include "util.h"

void MtlLoader::command(const std::string& cmd, const std::vector<std::string>& args)
{
    if (cmd == "newmtl") {
        Material mtl;
        mtl.name = args[0];
        m_materials.push_back(mtl);
    } else if (cmd == "map_Kd") {
        Material *mtl = &m_materials.back();
        mtl->texture = args[0];
    }
}
