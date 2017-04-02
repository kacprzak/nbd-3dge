#ifndef RESOURCESMGR_H
#define RESOURCESMGR_H

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

#include <string>

class ResourcesMgr
{
 public:
    ResourcesMgr(const std::string& dataFolder, const std::string& shadersFolder);

    void addShaderProgram(const std::string& name, ShaderProgram* sp)
    {
        m_shaderPrograms[name] = sp;
    }
    
 private:
    const std::string m_dataFolder, m_shadersFolder;

    std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::map<std::string, std::shared_ptr<ShaderProgram> m_shaderPrograms;
};

#endif
