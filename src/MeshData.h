#ifndef MESHDATA_H
#define MESHDATA_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

struct Aabb final
{
    Aabb(const std::vector<glm::vec3>& positions)
    {
        setToMaximum();

        for (const auto pos : positions) {
            leftBottomNear.x = std::max(leftBottomNear.x, pos.x);
            leftBottomNear.y = std::max(leftBottomNear.y, pos.y);
            leftBottomNear.z = std::max(leftBottomNear.z, pos.z);
            rightTopFar.x    = std::min(rightTopFar.x, pos.x);
            rightTopFar.y    = std::min(rightTopFar.y, pos.y);
            rightTopFar.z    = std::min(rightTopFar.z, pos.z);
        }

        sort();
    }

    glm::vec3 dimensions() const { return rightTopFar - leftBottomNear; }

    void sort()
    {
        if (leftBottomNear.x > rightTopFar.x) std::swap(leftBottomNear.x, rightTopFar.x);
        if (leftBottomNear.y > rightTopFar.y) std::swap(leftBottomNear.y, rightTopFar.y);
        if (leftBottomNear.z > rightTopFar.z) std::swap(leftBottomNear.z, rightTopFar.z);
    }

    void setToMaximum()
    {
        leftBottomNear.x = std::numeric_limits<float>::lowest();
        leftBottomNear.y = std::numeric_limits<float>::lowest();
        leftBottomNear.z = std::numeric_limits<float>::lowest();
        rightTopFar.x    = std::numeric_limits<float>::max();
        rightTopFar.y    = std::numeric_limits<float>::max();
        rightTopFar.z    = std::numeric_limits<float>::max();
    }

    glm::vec3 leftBottomNear;
    glm::vec3 rightTopFar;
};

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

    static Aabb calculateAABB(const std::vector<glm::vec3>& positions) { return Aabb{positions}; }

    static std::vector<glm::vec3> calculateTangents(const MeshData& meshData);
};

#endif // MESHDATA_H
