#ifndef MESHDATA_H
#define MESHDATA_H

#include "Buffer.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

struct MeshData final
{
    GLenum primitive;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<unsigned short> indices;
    std::string name;

    enum Attributes { Position, Normal, Tangent, TexCoord_0, TexCoord_1, TexCoord_2, TexCoord_3, Size };

    Accessor* iindices = nullptr;
    std::array<const Accessor*, Attributes::Size> attributes{};

    static MeshData fromWavefrontObj(const std::string& objfileName);
    static MeshData fromHeightmap(const std::vector<float>& heights, int w, int h,
                                  float textureStrech,
                                  glm::vec2 turbulence = glm::vec2(0.0f, 0.0f));

    static std::vector<glm::vec3> calculateTangents(const MeshData& meshData);
};

#endif // MESHDATA_H
