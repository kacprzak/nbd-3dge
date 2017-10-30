#ifndef MTLLOADER_H
#define MTLLOADER_H

#include "Loader.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

struct MaterialData
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    std::string ambientTex;
    std::string diffuseTex;
    std::string specularTex;
    std::string normalsTex;

    std::string name;
};

class MtlLoader : public Loader
{
  public:
    std::vector<MaterialData> materials() const { return m_materials; }

  protected:
    void command(const std::string& cmd, const std::vector<std::string>& args);

  private:
    std::vector<MaterialData> m_materials;
};

#endif // MTLLOADER_H
