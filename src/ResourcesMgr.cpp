#include "ResourcesMgr.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

ResourcesMgr::ResourcesMgr(const std::string& dataFolder, const std::string& shadersFolder)
    : m_dataFolder{dataFolder}
    , m_shadersFolder{shadersFolder}
{
}

//------------------------------------------------------------------------------

void ResourcesMgr::load(std::string xmlFile)
{
    using boost::property_tree::ptree;
    ptree pt;

    read_xml(m_dataFolder + xmlFile, pt);

    for (ptree::value_type& v : pt.get_child("assets")) {
        const std::string& assetType = v.first;
        ptree& assetTree             = v.second;

        if (assetType == "shaderProgram") {
            const std::string& name               = assetTree.get<std::string>("name");
            const std::string& vertexShaderFile   = assetTree.get("vertexShader", "");
            const std::string& geometryShaderFile = assetTree.get("geometryShader", "");
            const std::string& fragmentShaderFile = assetTree.get("fragmentShader", "");
            addShaderProgram(name, vertexShaderFile, geometryShaderFile, fragmentShaderFile);
        } else if (assetType == "texture") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& wrap = assetTree.get<std::string>("wrap", "GL_REPEAT");
            if (assetTree.get_child("file").size() == 0) {
                const std::string& file = assetTree.get<std::string>("file");
                addTexture(name, file, wrap);
            } else {
                std::array<std::string, 6> files;
                files[0] = assetTree.get<std::string>("file.right");
                files[1] = assetTree.get<std::string>("file.left");
                files[2] = assetTree.get<std::string>("file.top");
                files[3] = assetTree.get<std::string>("file.bottom");
                files[4] = assetTree.get<std::string>("file.back");
                files[5] = assetTree.get<std::string>("file.front");
                addTexture(name, files, wrap);
            }
        } else if (assetType == "font") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& file = assetTree.get<std::string>("file");
            addFont(name, file);
        } else if (assetType == "mesh") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& file = assetTree.get<std::string>("file");
            addMesh(name, file);
        }
    }
}

//------------------------------------------------------------------------------

void ResourcesMgr::addShaderProgram(const std::string& name, const std::string& vertexShaderFile,
                                    const std::string& geometryShaderFile,
                                    const std::string& fragmentShaderFile)
{
    std::unique_ptr<Shader> vs;
    std::unique_ptr<Shader> gs;
    std::unique_ptr<Shader> fs;

    const auto extractSource = [](const std::string& filename) -> std::string {
        std::string source;
        std::ifstream f(filename.c_str());

        if (f.is_open() == true) {
            source.assign((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
            f.close();
        } else {
            throw std::runtime_error{"File not found: " + filename};
        }
        return source;
    };

    auto sp = std::make_shared<ShaderProgram>();

    if (!vertexShaderFile.empty()) {
        vs = std::make_unique<Shader>(GL_VERTEX_SHADER,
                                      extractSource(m_shadersFolder + vertexShaderFile));
        sp->addShared(vs.get());
    }

    if (!geometryShaderFile.empty()) {
        gs = std::make_unique<Shader>(GL_GEOMETRY_SHADER,
                                      extractSource(m_shadersFolder + geometryShaderFile));
        sp->addShared(gs.get());
    }

    if (!fragmentShaderFile.empty()) {
        fs = std::make_unique<Shader>(GL_FRAGMENT_SHADER,
                                      extractSource(m_shadersFolder + fragmentShaderFile));
        sp->addShared(fs.get());
    }

    sp->link();

    m_shaderPrograms[name] = sp;
}

std::shared_ptr<ShaderProgram> ResourcesMgr::getShaderProgram(const std::string& name)
{
    auto it = m_shaderPrograms.find(name);
    if (it == std::end(m_shaderPrograms))
        throw std::runtime_error("Shader '" + name + "' not loaded.");
    else
        return it->second;
}

void ResourcesMgr::addTexture(const std::string& name, const std::string& filename,
                              const std::string& wrap)
{
    bool clamp = false;
    if (wrap == "GL_CLAMP_TO_EDGE")
        clamp = true;

    std::shared_ptr<Texture> tex{Texture::create(m_dataFolder + filename, clamp)};
    m_textures[name] = tex;
}

void ResourcesMgr::addTexture(const std::string& name, std::array<std::string, 6> filenames,
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

std::shared_ptr<Texture> ResourcesMgr::getTexture(const std::string& name)
{
    auto it = m_textures.find(name);
    if (it == std::end(m_textures))
        throw std::runtime_error("Texture '" + name + "' not loaded.");
    else
        return it->second;
}

void ResourcesMgr::addMesh(const std::string& name, const std::string& filename)
{
    std::shared_ptr<Mesh> mesh{Mesh::fromWavefrontObj(m_dataFolder + filename)};
    m_meshes[name] = mesh;
}

std::shared_ptr<Mesh> ResourcesMgr::getMesh(const std::string& name)
{
    auto it = m_meshes.find(name);
    if (it == std::end(m_meshes))
        throw std::runtime_error("Mesh '" + name + "' not loaded.");
    else
        return it->second;
}

void ResourcesMgr::addFont(const std::string& name, const std::string& filename)
{
    FontLoader loader;
    loader.load(m_dataFolder + filename);

    auto font = std::shared_ptr<Font>{loader.getFont()};

    std::vector<std::shared_ptr<Texture>> textures;
    for (const auto& texFilename : font->getTexturesFilenames()) {
        textures.emplace_back(Texture::create(m_dataFolder + texFilename));
    }
    font->setTextures(textures);

    m_fonts[name] = font;
}

std::shared_ptr<Font> ResourcesMgr::getFont(const std::string& name)
{
    auto it = m_fonts.find(name);
    if (it == std::end(m_fonts))
        throw std::runtime_error("Font '" + name + "' not loaded.");
    else
        return it->second;
}

void ResourcesMgr::addScript(const std::string& name, std::shared_ptr<Script> script)
{
    m_scripts[name] = script;
}

std::shared_ptr<Script> ResourcesMgr::getScript(const std::string& name)
{
    auto it = m_scripts.find(name);
    if (it == std::end(m_scripts))
        throw std::runtime_error("Script '" + name + "' not loaded.");
    else
        return it->second;
}
