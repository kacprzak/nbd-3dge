#ifndef MESHDATA_H
#define MESHDATA_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

struct MeshData final
{
    GLenum primitive;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<unsigned short> indices;

    static MeshData fromWavefrontObj(const std::string& objfileName);
    static MeshData fromHeightmap(const std::vector<float>& heights, int w, int h,
                                  float textureStrech);

    static std::array<float, 6> calculateAABB(const std::vector<glm::vec3>& positions);

    static std::vector<glm::vec3> calculateTangents(const MeshData& meshData);
};

#endif // MESHDATA_H
