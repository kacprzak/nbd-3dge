#ifndef MTLLOADER_H
#define MTLLOADER_H

#include "Loader.h"
#include "MaterialData.h"

#include <vector>

class MtlLoader : public Loader
{
  public:
    std::vector<MaterialData> materials() const { return m_materials; }

  protected:
    void command(const std::string& cmd, const std::vector<std::string>& args);

  private:
    TextureData toTextureData(const std::string& cmd, const std::vector<std::string>& args) const;

    std::vector<MaterialData> m_materials;
};

#endif // MTLLOADER_H
