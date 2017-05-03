#ifndef MTLLOADER_H
#define MTLLOADER_H

#include "Loader.h"

#include <vector>
#include <string>

class MtlLoader : public Loader
{
    struct Material {
        std::string name;
        std::string texture;
    };

 protected:
    void command(const std::string& cmd, const std::vector<std::string>& args);

 private:
    std::vector<Material> m_materials;
};

#endif // MTLLOADER_H
