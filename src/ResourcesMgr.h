#ifndef RESOURCESMGR_H
#define RESOURCESMGR_H

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Script.h"
#include "Font.h"
#include "FontLoader.h"

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
                          const std::string& geometryShaderFile,
                          const std::string& fragmentShaderFile)
    {
        std::unique_ptr<Shader> vs;
        std::unique_ptr<Shader> gs;
        std::unique_ptr<Shader> fs;

        auto sp = std::make_shared<ShaderProgram>();

        if (!vertexShaderFile.empty()) {
            vs = std::make_unique<Shader>(GL_VERTEX_SHADER, m_shadersFolder + vertexShaderFile);
            sp->addShared(vs.get());
        }

        if (!geometryShaderFile.empty()) {
            gs = std::make_unique<Shader>(GL_GEOMETRY_SHADER, m_shadersFolder + geometryShaderFile);
            sp->addShared(gs.get());
        }
        
        if (!fragmentShaderFile.empty()) {
            fs = std::make_unique<Shader>(GL_FRAGMENT_SHADER, m_shadersFolder + fragmentShaderFile);
            sp->addShared(fs.get());
        }

        sp->link();
     
        m_shaderPrograms[name] = sp;
    }

    std::shared_ptr<ShaderProgram> getShaderProgram(const std::string& name)
    {
        auto it = m_shaderPrograms.find(name);
        if (it == std::end(m_shaderPrograms))
            throw std::runtime_error("Shader '" + name + "' not loaded.");
        else
            return it->second;
    }

    void addTexture(const std::string& name,
                    const std::string& filename,
                    const std::string& wrap)
    {
        bool clamp = false;
        if (wrap == "GL_CLAMP_TO_EDGE")
            clamp = true;
        
        std::shared_ptr<Texture> tex{Texture::create(m_dataFolder + filename, clamp)};
        m_textures[name] = tex;
    }

    void addTexture(const std::string& name,
                    std::array<std::string, 6> filenames,
                    const std::string& wrap)
    {
        bool clamp = false;
        if (wrap == "GL_CLAMP_TO_EDGE")
            clamp = true;

        for (auto& filename : filenames) {
            filename = m_dataFolder + filename;
        }
        
        std::shared_ptr<Texture> tex{Texture::create(filenames, clamp)};
        m_textures[name] = tex;
    }
    
    std::shared_ptr<Texture> getTexture(const std::string& name)
    {
        auto it = m_textures.find(name);
        if (it == std::end(m_textures))
            throw std::runtime_error("Texture '" + name + "' not loaded.");
        else
            return it->second;
    }

    void addMesh(const std::string& name,
                 const std::string& filename)
    {
        std::shared_ptr<Mesh> mesh{Mesh::create(m_dataFolder + filename)};
        m_meshes[name] = mesh;
    }

    std::shared_ptr<Mesh> getMesh(const std::string& name)
    {
        auto it = m_meshes.find(name);
        if (it == std::end(m_meshes))
            throw std::runtime_error("Mesh '" + name + "' not loaded.");
        else
            return it->second;
    }

    void addFont(const std::string& name,
                 const std::string& filename)
    {
        FontLoader loader;
        loader.load(m_dataFolder + filename);

        auto font = std::shared_ptr<Font>{loader.getFont()};

        std::vector<std::shared_ptr<Texture>> textures;
        for (const auto& texFilename : font->getTexturesFilenames())
        {
            textures.emplace_back(Texture::create(m_dataFolder + texFilename));
        }
        font->setTextures(textures);
        
        m_fonts[name] = font;
    }

    std::shared_ptr<Font> getFont(const std::string& name)
    {
        auto it = m_fonts.find(name);
        if (it == std::end(m_fonts))
            throw std::runtime_error("Font '" + name + "' not loaded.");
        else
            return it->second;
    }
    
    void addScript(const std::string& name,
                   std::shared_ptr<Script> script)
    {
        m_scripts[name] = script;
    }

    std::shared_ptr<Script> getScript(const std::string& name)
    {
        auto it = m_scripts.find(name);
        if (it == std::end(m_scripts))
            throw std::runtime_error("Script '" + name + "' not loaded.");
        else
            return it->second;
    }

    
private:
    const std::string m_dataFolder, m_shadersFolder;

    std::map<std::string, std::shared_ptr<Mesh>> m_meshes;
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::map<std::string, std::shared_ptr<Font>> m_fonts;
    std::map<std::string, std::shared_ptr<ShaderProgram>> m_shaderPrograms;
    std::map<std::string, std::shared_ptr<Script>> m_scripts;
};

#endif
