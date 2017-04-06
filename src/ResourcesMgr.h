#ifndef RESOURCESMGR_H
#define RESOURCESMGR_H

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

#include <string>
#include <map>

class ResourcesMgr
{
public:
    ResourcesMgr(const std::string& dataFolder, const std::string& shadersFolder)
        : m_dataFolder{dataFolder}, m_shadersFolder{shadersFolder}
    {}

    void addShaderProgram(const std::string& name,
                          const std::string& vertexShaderFile,
                          const std::string& fragmentShaderFile)
    {
        Shader *vs = new Shader(GL_VERTEX_SHADER, m_shadersFolder + vertexShaderFile);
        Shader *fs = new Shader(GL_FRAGMENT_SHADER, m_shadersFolder + fragmentShaderFile);

        auto sp = std::make_shared<ShaderProgram>();
        sp->addShared(vs);
        sp->addShared(fs);

        // Required in GLSL 120
        glBindAttribLocation(sp->id(), 0, "position");
        glBindAttribLocation(sp->id(), 1, "in_texCoord");

        sp->link();
     
        m_shaderPrograms[name] = sp;
    }

    ShaderProgram* getShaderProgram(const std::string& name)
    {
        return m_shaderPrograms[name].get();
    }

    void addTexture(const std::string& name,
                    const std::string& filename,
                    bool clamp = false)
    {
        std::shared_ptr<Texture> tex{Texture::create(m_dataFolder + filename)};
        m_textures[name] = tex;
    }

    Texture* getTexture(const std::string& name)
    {
        return m_textures[name].get();
    }
    
private:
    const std::string m_dataFolder, m_shadersFolder;

    std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::map<std::string, std::shared_ptr<ShaderProgram>> m_shaderPrograms;
};

#endif