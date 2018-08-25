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

    static MeshData fromWavefrontObj(const std::string& objfileName);
    static MeshData fromHeightmap(const std::vector<float>& heights, int w, int h,
                                  float textureStrech,
                                  glm::vec2 turbulence = glm::vec2(0.0f, 0.0f));

    static std::vector<glm::vec3> calculateTangents(const MeshData& meshData);
};

#endif // MESHDATA_H
