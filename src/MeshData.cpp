#include "MeshData.h"
#include "ObjLoader.h"

std::vector<glm::vec3> MeshData::calculateTangents(const MeshData& md)
{
    std::vector<glm::vec3> tangents;

    if (md.texcoords.empty()) return tangents;

    tangents.resize(md.normals.size());

    auto inc                              = 1;
    if (md.primitive == GL_TRIANGLES) inc = 3;

    for (size_t i = 2; i < md.indices.size(); i += inc) {
        auto index0 = md.indices[i - 2];
        auto index1 = md.indices[i - 1];
        auto index2 = md.indices[i - 0];
        // Shortcuts for positions
        glm::vec3 v0 = md.positions[index0];
        glm::vec3 v1 = md.positions[index1];
        glm::vec3 v2 = md.positions[index2];

        // Shortcuts for UVs
        glm::vec2 st0 = md.texcoords[index0];
        glm::vec2 st1 = md.texcoords[index1];
        glm::vec2 st2 = md.texcoords[index2];

        // Edges of the triangle : postion delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // ST delta
        glm::vec2 deltaST1 = st1 - st0;
        glm::vec2 deltaST2 = st2 - st0;

        float r           = 1.0f / (deltaST1.x * deltaST2.y - deltaST1.y * deltaST2.x);
        glm::vec3 tangent = (deltaPos1 * deltaST2.y - deltaPos2 * deltaST1.y) * r;
        // glm::vec3 bitangent = (deltaPos2 * deltaST1.x - deltaPos1 * deltaST2.x) * r;

        tangent = glm::normalize(tangent);

        tangents[index0] = tangent;
        tangents[index1] = tangent;
        tangents[index2] = tangent;
    }

    return tangents;
}

//------------------------------------------------------------------------------

MeshData MeshData::fromWavefrontObj(const std::string& objfileName)
{
    MeshData meshData;

    ObjLoader objLoader;
    objLoader.load(objfileName);

    meshData.primitive = GLenum(objLoader.primitive());
    meshData.positions = objLoader.positions();
    meshData.normals   = objLoader.normals();
    meshData.texcoords = objLoader.texCoords();
    meshData.indices   = objLoader.indices();

    return meshData;
}

//------------------------------------------------------------------------------

MeshData MeshData::fromHeightmap(const std::vector<float>& heights, int w, int h,
                                 float textureStrech, glm::vec2 turbulence)
{
    if (w % 2 != 0 || h % 2 != 0) {
        throw std::runtime_error{"Heightmap with odd size is not supported."};
    }

    const auto getHeight = [&](int x, int y) -> float { return heights[y * h + x]; };

    const auto getNormal = [&](int x, int y) -> glm::vec3 {
        auto n = glm::vec3{0.0f, 0.0f, 2.0};

        float center = getHeight(x, y);

        float left  = x > 0 ? getHeight(x - 1, y) : center;
        float right = x < w - 1 ? getHeight(x + 1, y) : center;

        n[0] = right - left;

        float bottom = y > 0 ? getHeight(x, y - 1) : center;
        float top    = y < h - 1 ? getHeight(x, y + 1) : center;

        n[1] = top - bottom;

        // W have normal in texture space where z is up but in OpenGL y is up
        n[0] = -n[0];
        n[2] = -n[1];
        n[1] = 2.0f;

        return glm::normalize(n);
    };

    // turbulence = glm::vec2(0.15, 0.15);

    const auto getTexCoord = [textureStrech, turbulence](int x, int y) -> glm::vec2 {
        auto texCoord = glm::vec2{0.0, 0.0};

        texCoord[0] = x / textureStrech;
        texCoord[1] = y / textureStrech;

        // todo: Not a real turbulence function
        const auto frand = [](float low, float hi) -> float {
            return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - low)));
        };

        if (turbulence.x > 0.0f) {
            float xTurb = frand(-turbulence.x, turbulence.x);
            texCoord += xTurb / textureStrech;
        }

        if (turbulence.y > 0.0f) {
            float yTurb = frand(-turbulence.y, turbulence.y);
            texCoord += yTurb / textureStrech;
        }

        return texCoord;
    };

    MeshData md;
    md.primitive = GL_TRIANGLE_STRIP;

    auto& positions = md.positions;
    positions.resize(w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx          = (y * h + x);
            positions[idx].x = x - w / 2.0f + 0.5f;
            positions[idx].y = getHeight(x, y);
            positions[idx].z = y - h / 2.0f + 0.5f;
        }
    }

    auto& normals = md.normals;
    normals.resize(w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx      = (y * h + x);
            normals[idx] = getNormal(x, y);
        }
    }

    auto& texCoords = md.texcoords;
    texCoords.resize(w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx        = (y * h + x);
            texCoords[idx] = getTexCoord(x, y);
        }
    }

    auto& indices = md.indices;
    indices.resize(w * h + (w - 1) * (h - 2));

    int ind = 0;
    for (int y = 0; y < h - 1; ++y) {
        if (y % 2 == 0) {
            for (int x = 0; x < w; ++x) {
                indices[ind++] = x + y * h;
                indices[ind++] = x + (y + 1) * h;
            }
        } else {
            for (int x = w - 1; x > 0; --x) {
                indices[ind++] = x + (y + 1) * h;
                indices[ind++] = (x - 1) + y * h;
            }
        }
    }

    md.name = "HEIGHTMAP";
    return md;
}
