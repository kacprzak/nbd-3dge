#include "ResourcesMgr.h"

#include "Logger.h"
#include "MtlLoader.h"

#include <SDL.h>
#include <SDL_image.h>

#include <array>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

ResourcesMgr::ResourcesMgr(const std::string& dataFolder, const std::string& shadersFolder)
    : m_dataFolder{dataFolder}
    , m_shadersFolder{shadersFolder}
{
}

//------------------------------------------------------------------------------

void ResourcesMgr::load(const std::string& xmlFile)
{
    loadShaders(xmlFile);

    using boost::property_tree::ptree;
    ptree pt;

    read_xml(m_dataFolder + xmlFile, pt);

    for (ptree::value_type& v : pt.get_child("assets")) {
        const std::string& assetType = v.first;
        ptree& assetTree             = v.second;

        if (assetType == "font") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& file = assetTree.get<std::string>("file");
            addFont(name, file);
        } else if (assetType == "mesh") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& file = assetTree.get<std::string>("file");
            MeshData meshData       = MeshData::fromWavefrontObj(m_dataFolder + file);
            meshData.name           = name;
            addMesh(meshData);
        } else if (assetType == "heightfield") {
            const std::string& name = assetTree.get<std::string>("name");
            const std::string& file = assetTree.get<std::string>("file");
            float amplitude         = assetTree.get("amplitude", 1.0f);
            addHeightfield(name, file, amplitude);
        }
    }

    loadMaterials(xmlFile);
}

//------------------------------------------------------------------------------

void ResourcesMgr::loadShaders(const std::string& xmlFile)
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
        }
    }
}

//------------------------------------------------------------------------------

void ResourcesMgr::loadMaterials(const std::string& xmlFile)
{
    using boost::property_tree::ptree;
    ptree pt;

    read_xml(m_dataFolder + xmlFile, pt);

    for (ptree::value_type& v : pt.get_child("assets")) {
        const std::string& assetType = v.first;
        ptree& assetTree             = v.second;

        if (assetType == "materials") {
            const std::string& file = assetTree.get<std::string>("file");
            MtlLoader mtlLoader;
            mtlLoader.load(m_dataFolder + file);

            for (const auto& mtl : mtlLoader.materials()) {
                addMaterial(mtl);
            }
        }
    }
}

//------------------------------------------------------------------------------

void ResourcesMgr::addMaterial(const MaterialData& materialData)
{
    std::vector<std::shared_ptr<Texture>> textures;

    for (const auto& texData : materialData.textures) {
        const auto& file = texData.filenames.at(0);
        addTexture(texData);
        textures.push_back(getTexture(file));
    }

    auto it = m_materials.find(materialData.name);
    if (it == m_materials.end()) {
        LOG_TRACE << "Adding Material: " << materialData.name;

        m_materials[materialData.name] = std::make_shared<Material>(materialData, textures);
    } else {
        LOG_TRACE << "Reloading Material: " << materialData.name;
        *it->second = Material{materialData, textures};
    }
}

std::shared_ptr<Material> ResourcesMgr::getMaterial(const std::string& name) const
{
    auto it = m_materials.find(name);
    if (it == std::end(m_materials))
        throw std::runtime_error("Material '" + name + "' not loaded.");
    else
        return it->second;
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

    std::vector<Shader*> shaders;

    if (!vertexShaderFile.empty()) {
        vs = std::make_unique<Shader>(GL_VERTEX_SHADER,
                                      extractSource(m_shadersFolder + vertexShaderFile));
        shaders.push_back(vs.get());
    }

    if (!geometryShaderFile.empty()) {
        gs = std::make_unique<Shader>(GL_GEOMETRY_SHADER,
                                      extractSource(m_shadersFolder + geometryShaderFile));
        shaders.push_back(gs.get());
    }

    if (!fragmentShaderFile.empty()) {
        fs = std::make_unique<Shader>(GL_FRAGMENT_SHADER,
                                      extractSource(m_shadersFolder + fragmentShaderFile));
        shaders.push_back(fs.get());
    }

    auto it = m_shaderPrograms.find(name);
    if (it == std::end(m_shaderPrograms)) {
        LOG_TRACE << "Adding ShaderProgram: " << name;

        auto sp = std::make_shared<ShaderProgram>();
        sp->link(shaders);
        m_shaderPrograms[name] = sp;
    } else {
        LOG_TRACE << "Reloading ShaderProgram: " << name;

        it->second->link(shaders);
    }
}

std::shared_ptr<ShaderProgram> ResourcesMgr::getShaderProgram(const std::string& name) const
{
    auto it = m_shaderPrograms.find(name);
    if (it == std::end(m_shaderPrograms))
        throw std::runtime_error("Shader '" + name + "' not loaded.");
    else
        return it->second;
}

//------------------------------------------------------------------------------

void ResourcesMgr::addTexture(const TextureData& texData)
{
    TextureData tmp = texData;

    LOG_TRACE << "Adding Texture: " << tmp.name;

    for (auto& filename : tmp.filenames)
        filename = m_dataFolder + filename;

    m_textures[tmp.name] = std::make_shared<Texture>(tmp);
}

std::shared_ptr<Texture> ResourcesMgr::getTexture(const std::string& name) const
{
    auto it = m_textures.find(name);
    if (it == std::end(m_textures))
        throw std::runtime_error("Texture '" + name + "' not loaded.");
    else
        return it->second;
}

//------------------------------------------------------------------------------

void ResourcesMgr::addMesh(const MeshData& meshData)
{
    LOG_TRACE << "Adding Mesh: " << meshData.name;

    m_meshes[meshData.name] = std::make_shared<Mesh>(meshData);
}

std::shared_ptr<Mesh> ResourcesMgr::getMesh(const std::string& name) const
{
    auto it = m_meshes.find(name);
    if (it == std::end(m_meshes))
        throw std::runtime_error("Mesh '" + name + "' not loaded.");
    else
        return it->second;
}

//------------------------------------------------------------------------------

void ResourcesMgr::addFont(const std::string& name, const std::string& filename)
{
    LOG_TRACE << "Adding Font: " << name;

    FontLoader loader;
    loader.load(m_dataFolder + filename);

    auto font = std::make_shared<Font>(loader.getFont());

    std::vector<std::shared_ptr<Texture>> textures;
    for (const auto& texFilename : font->getTexturesFilenames()) {
        TextureData texData;
        texData.name = texFilename;
        texData.filenames.push_back(m_dataFolder + texFilename);
        textures.emplace_back(std::make_shared<Texture>(texData));
    }
    font->setTextures(textures);

    m_fonts[name] = font;
}

std::shared_ptr<Font> ResourcesMgr::getFont(const std::string& name) const
{
    auto it = m_fonts.find(name);
    if (it == std::end(m_fonts))
        throw std::runtime_error("Font '" + name + "' not loaded.");
    else
        return it->second;
}

//------------------------------------------------------------------------------

void ResourcesMgr::addScript(const std::string& name, std::shared_ptr<Script> script)
{
    m_scripts[name] = script;
}

std::shared_ptr<Script> ResourcesMgr::getScript(const std::string& name) const
{
    auto it = m_scripts.find(name);
    if (it == std::end(m_scripts))
        throw std::runtime_error("Script '" + name + "' not loaded.");
    else
        return it->second;
}

//------------------------------------------------------------------------------

void ResourcesMgr::addHeightfield(const std::string& name, const std::string& filename,
                                  float amplitude)
{
    LOG_TRACE << "Adding Heightfield: " << name;

    const std::string filepath = m_dataFolder + filename;
    SDL_Surface* surface       = IMG_Load(filepath.c_str());

    if (!surface) {
        throw std::runtime_error{"SDL_Image load error: " + std::string{IMG_GetError()}};
    }

    auto heightfield = std::make_shared<Heightfield>();

    heightfield->w         = surface->w;
    heightfield->h         = surface->h;
    heightfield->amplitude = amplitude;

    heightfield->heights.resize(heightfield->w * heightfield->h);

    SDL_LockSurface(surface);
    Uint8* pixels = (Uint8*)surface->pixels;
    auto bytespp  = surface->format->BytesPerPixel;

    for (int y = 0; y < heightfield->h; ++y) {
        for (int x = 0; x < heightfield->w; ++x) {
            auto& val = heightfield->heights[y * heightfield->h + x];
            val       = pixels[(y * surface->w + x) * bytespp] / 128.0 - 1.0;
            val *= amplitude;
        }
    }
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    m_heightfields[name] = heightfield;
}

std::shared_ptr<const Heightfield> ResourcesMgr::getHeightfield(const std::string& name) const
{
    auto it = m_heightfields.find(name);

    auto sep = name.find_first_of(':');
    if (sep != std::string::npos) {
        auto sub = name.substr(sep + 1);
        it       = m_heightfields.find(sub);
    }

    if (it == std::end(m_heightfields))
        throw std::runtime_error("Heightfield '" + name + "' not loaded.");
    else
        return it->second;
}
