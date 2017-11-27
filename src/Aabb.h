#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

#include <array>
#include <vector>

struct Aabb final
{
    Aabb() = default;

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

    Aabb(const std::array<glm::vec4, 8>& positions)
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

    Aabb(glm::vec3 p1, glm::vec3 p2)
        : leftBottomNear{p1}
        , rightTopFar{p2}
    {
        sort();
    }

    bool isEmpty() const { return leftBottomNear == rightTopFar; }

    static Aabb unit()
    {
        glm::vec3 lbn = {-1.f, -1.f, -1.f};
        glm::vec3 rtf = {1.f, 1.f, 1.f};
        return Aabb{lbn, rtf};
    }

    float left() const { return leftBottomNear.x; };
    float right() const { return rightTopFar.x; };
    float bottom() const { return leftBottomNear.y; };
    float top() const { return rightTopFar.y; };
    float near() const { return leftBottomNear.z; };
    float far() const { return rightTopFar.z; };

    std::array<glm::vec4, 8> toPositions() const
    {
        std::array<glm::vec4, 8> ans;

        ans[0] = glm::vec4(leftBottomNear, 1.f);
        ans[1] = {rightTopFar.x, leftBottomNear.y, leftBottomNear.z, 1.f};
        ans[2] = {rightTopFar.x, rightTopFar.y, leftBottomNear.z, 1.f};
        ans[3] = {leftBottomNear.x, rightTopFar.y, leftBottomNear.z, 1.f};

        ans[4] = glm::vec4(rightTopFar, 1.f);
        ans[5] = {rightTopFar.x, leftBottomNear.y, rightTopFar.z, 1.f};
        ans[6] = {leftBottomNear.x, leftBottomNear.y, rightTopFar.z, 1.f};
        ans[7] = {leftBottomNear.x, rightTopFar.y, rightTopFar.z, 1.f};

        return ans;
    }

    glm::vec3 dimensions() const { return rightTopFar - leftBottomNear; }

    void sort()
    {
        if (leftBottomNear.x > rightTopFar.x) std::swap(leftBottomNear.x, rightTopFar.x);
        if (leftBottomNear.y > rightTopFar.y) std::swap(leftBottomNear.y, rightTopFar.y);
        if (leftBottomNear.z > rightTopFar.z) std::swap(leftBottomNear.z, rightTopFar.z);
    }

    Aabb mbr(const Aabb& other) const
    {
        if (isEmpty()) return other;
        if (other.isEmpty()) return *this;

        Aabb copy = *this;

        copy.leftBottomNear.x = std::min(copy.leftBottomNear.x, other.leftBottomNear.x);
        copy.leftBottomNear.y = std::min(copy.leftBottomNear.y, other.leftBottomNear.y);
        copy.leftBottomNear.z = std::min(copy.leftBottomNear.z, other.leftBottomNear.z);

        copy.rightTopFar.x = std::max(copy.rightTopFar.x, other.rightTopFar.x);
        copy.rightTopFar.y = std::max(copy.rightTopFar.y, other.rightTopFar.y);
        copy.rightTopFar.z = std::max(copy.rightTopFar.z, other.rightTopFar.z);

        return copy;
    }

    bool intersects(const Aabb& other) const
    {
        return !(leftBottomNear.x > other.rightTopFar.x || other.leftBottomNear.x > rightTopFar.x ||
                 leftBottomNear.y > other.rightTopFar.y || other.leftBottomNear.y > rightTopFar.y ||
                 leftBottomNear.z > other.rightTopFar.z || other.leftBottomNear.z > rightTopFar.z);
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

inline Aabb operator*(const glm::mat4& matrix, const Aabb& aabb)
{
    auto positions = aabb.toPositions();
    for (auto& pos : positions) {
        pos = matrix * pos;
        pos /= pos.w;
    }
    return Aabb(positions);
}

#endif /* AABB_H */
