#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

#include <array>
#include <vector>

struct Aabb final
{
    Aabb() = default;

    explicit Aabb(const std::vector<glm::vec3>& positions)
    {
        setToMaximum();

        for (const auto pos : positions) {
            minimum.x = std::max(minimum.x, pos.x);
            minimum.y = std::max(minimum.y, pos.y);
            minimum.z = std::max(minimum.z, pos.z);
            maximum.x = std::min(maximum.x, pos.x);
            maximum.y = std::min(maximum.y, pos.y);
            maximum.z = std::min(maximum.z, pos.z);
        }

        sort();
    }

    explicit Aabb(const std::array<glm::vec4, 8>& positions)
    {
        setToMaximum();

        for (const auto pos : positions) {
            minimum.x = std::max(minimum.x, pos.x);
            minimum.y = std::max(minimum.y, pos.y);
            minimum.z = std::max(minimum.z, pos.z);
            maximum.x = std::min(maximum.x, pos.x);
            maximum.y = std::min(maximum.y, pos.y);
            maximum.z = std::min(maximum.z, pos.z);
        }

        sort();
    }

    Aabb(glm::vec3 p1, glm::vec3 p2)
        : minimum{p1}
        , maximum{p2}
    {
        sort();
    }

    bool isEmpty() const { return minimum == maximum; }

    static Aabb unit()
    {
        const glm::vec3 min = {-1.f, -1.f, -1.f};
        const glm::vec3 max = {1.f, 1.f, 1.f};
        return Aabb{min, max};
    }

    /**
     *    3----2     y
     *   /|   /|     |
     *  7----4 |     o--x
     *  | 0--|-1    /
     *  |/   |/    z
     *  6----5
     */
    std::array<glm::vec4, 8> toPositions() const
    {
        std::array<glm::vec4, 8> ans;

        ans[0] = glm::vec4(minimum, 1.f);                // lbf
        ans[1] = {maximum.x, minimum.y, minimum.z, 1.f}; // rbf
        ans[2] = {maximum.x, maximum.y, minimum.z, 1.f}; // rtf
        ans[3] = {minimum.x, maximum.y, minimum.z, 1.f}; // ltf

        ans[4] = glm::vec4(maximum, 1.f);                // rtn
        ans[5] = {maximum.x, minimum.y, maximum.z, 1.f}; // rbn
        ans[6] = {minimum.x, minimum.y, maximum.z, 1.f}; // lbn
        ans[7] = {minimum.x, maximum.y, maximum.z, 1.f}; // ltn

        return ans;
    }

    glm::vec3 dimensions() const { return maximum - minimum; }

    void sort()
    {
        if (minimum.x > maximum.x) std::swap(minimum.x, maximum.x);
        if (minimum.y > maximum.y) std::swap(minimum.y, maximum.y);
        if (minimum.z > maximum.z) std::swap(minimum.z, maximum.z);
    }

    Aabb mbr(const Aabb& other) const
    {
        if (isEmpty()) return other;
        if (other.isEmpty()) return *this;

        Aabb copy = *this;

        copy.minimum.x = std::min(copy.minimum.x, other.minimum.x);
        copy.minimum.y = std::min(copy.minimum.y, other.minimum.y);
        copy.minimum.z = std::min(copy.minimum.z, other.minimum.z);

        copy.maximum.x = std::max(copy.maximum.x, other.maximum.x);
        copy.maximum.y = std::max(copy.maximum.y, other.maximum.y);
        copy.maximum.z = std::max(copy.maximum.z, other.maximum.z);

        return copy;
    }

    bool intersects(const Aabb& other) const
    {
        return !(minimum.x > other.maximum.x || other.minimum.x > maximum.x ||
                 minimum.y > other.maximum.y || other.minimum.y > maximum.y ||
                 minimum.z > other.maximum.z || other.minimum.z > maximum.z);
    }

    void setToMaximum()
    {
        minimum.x = std::numeric_limits<float>::lowest();
        minimum.y = std::numeric_limits<float>::lowest();
        minimum.z = std::numeric_limits<float>::lowest();
        maximum.x = std::numeric_limits<float>::max();
        maximum.y = std::numeric_limits<float>::max();
        maximum.z = std::numeric_limits<float>::max();
    }

    glm::vec3 minimum{};
    glm::vec3 maximum{};
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
