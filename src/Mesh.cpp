#include "Mesh.h"

#include "Logger.h"
#include "ObjLoader.h"
#include "Util.h"

#include <array>
#include <cstring>
#include <limits>

Mesh::Mesh(GLenum primitive, const std::vector<glm::vec3>& vertices,
           const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texcoords,
           const std::vector<GLushort>& indices)
    : m_primitive{primitive}
{
    std::fill(std::begin(m_bufferSizes), std::end(m_bufferSizes), 0);

    m_numberOfVertices = vertices.size();
    m_numberOfElements = indices.size();

    const auto& tangents = calculateTangents(primitive, vertices, normals, texcoords, indices);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(NUM_BUFFERS, m_buffers);

    auto aabb = calculateAABB(vertices);
    LOG_INFO << "Loaded Mesh: " << m_vao;
    LOG_TRACE << "  Vertices: " << vertices.size() << "\t id: " << m_buffers[POSITIONS] << '\n'
              << "  Normals: " << normals.size() << "\t id: " << m_buffers[NORMALS] << '\n'
              << "  Tangents: " << tangents.size() << "\t id: " << m_buffers[TANGENTS] << '\n'
              << "  TexCoords: " << texcoords.size() << "\t id: " << m_buffers[TEXCOORDS] << '\n'
              << "  Indices: " << indices.size() << "\t id: " << m_buffers[INDICES] << '\n'
              << "  Primitive: " << primitive << '\n'
              << "  Dimensions: (" << aabb[1] - aabb[0] << " x " << aabb[3] - aabb[2] << " x "
              << aabb[5] - aabb[4] << ")";

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    m_bufferSizes[POSITIONS] = sizeof(vertices[0]) * vertices.size();
    glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[POSITIONS], &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMALS]);
        m_bufferSizes[NORMALS] = sizeof(normals[0]) * normals.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[NORMALS], &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (tangents.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TANGENTS]);
        m_bufferSizes[TANGENTS] = sizeof(tangents[0]) * tangents.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[TANGENTS], &tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (texcoords.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORDS]);
        m_bufferSizes[TEXCOORDS] = sizeof(texcoords[0]) * texcoords.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[TEXCOORDS], &texcoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);
        m_bufferSizes[INDICES] = sizeof(GLushort) * indices.size();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_bufferSizes[INDICES], &indices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(NUM_BUFFERS, m_buffers);

    LOG_INFO << "Released Mesh: " << m_vao;
}

void Mesh::draw() const
{
    if (m_numberOfElements == 0) {
        draw(0, m_numberOfVertices);
    } else {
        draw(0, m_numberOfElements);
    }
}

void Mesh::draw(int start, int count) const
{
    glBindVertexArray(m_vao);

    if (m_numberOfElements == 0) {
        glDrawArrays(m_primitive, start, count);
    } else {
        glDrawElements(m_primitive, count, GL_UNSIGNED_SHORT, 0);
    }

    glBindVertexArray(0);
}

std::vector<float> Mesh::positions() const
{
    std::vector<float> retVal;
    retVal.resize(m_bufferSizes[POSITIONS] / sizeof(float));

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    void* buff = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

    std::memcpy(retVal.data(), buff, m_bufferSizes[POSITIONS]);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return retVal;
}

std::array<float, 6> Mesh::calculateAABB(const std::vector<glm::vec3>& positions)
{
    std::array<float, 6> retVal;
    // x axis
    retVal[0] = std::numeric_limits<float>::max();
    retVal[1] = std::numeric_limits<float>::lowest();
    // y axis
    retVal[2] = std::numeric_limits<float>::max();
    retVal[3] = std::numeric_limits<float>::lowest();
    // z axis
    retVal[4] = std::numeric_limits<float>::max();
    retVal[5] = std::numeric_limits<float>::lowest();

    for (const auto pos : positions) {
        retVal[0] = std::min(retVal[0], pos.x);
        retVal[1] = std::max(retVal[1], pos.x);
        retVal[2] = std::min(retVal[2], pos.y);
        retVal[3] = std::max(retVal[3], pos.y);
        retVal[4] = std::min(retVal[4], pos.z);
        retVal[5] = std::max(retVal[5], pos.z);
    }

    return retVal;
}

//------------------------------------------------------------------------------

std::vector<glm::vec3> Mesh::calculateTangents(GLenum primitive,
                                               const std::vector<glm::vec3>& positions,
                                               const std::vector<glm::vec3>& normals,
                                               const std::vector<glm::vec2>& texcoords,
                                               const std::vector<GLushort>& indices)
{
    std::vector<glm::vec3> tangents;

    if (texcoords.empty()) return tangents;

    tangents.resize(normals.size());

    auto inc                           = 1;
    if (primitive == GL_TRIANGLES) inc = 3;

    for (size_t i = 2; i < indices.size(); i += inc) {
        auto index0 = indices[i - 2];
        auto index1 = indices[i - 1];
        auto index2 = indices[i - 0];
        // Shortcuts for positions
        glm::vec3 v0 = positions[index0];
        glm::vec3 v1 = positions[index1];
        glm::vec3 v2 = positions[index2];

        // Shortcuts for UVs
        glm::vec2 st0 = texcoords[index0];
        glm::vec2 st1 = texcoords[index1];
        glm::vec2 st2 = texcoords[index2];

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

std::unique_ptr<Mesh> Mesh::fromWavefrontObj(const std::string& objfileName)
{
    // std::string name = extractFilename(objfileName);

    ObjLoader objLoader;
    objLoader.load(objfileName);

    return std::make_unique<Mesh>(GLenum(objLoader.primitive()), objLoader.positions(),
                                  objLoader.normals(), objLoader.texCoords(), objLoader.indices());
}

//------------------------------------------------------------------------------

std::unique_ptr<Mesh> Mesh::fromHeightmap(const std::vector<float>& heights, int w, int h,
                                          float textureStrech)
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

    const auto getTexCoord = [textureStrech](int x, int y) -> glm::vec2 {
        auto texCoord = glm::vec2{0.0, 0.0};

        texCoord[0] = x / textureStrech;
        texCoord[1] = y / textureStrech;

        return texCoord;
    };

    std::vector<glm::vec3> positions;
    positions.resize(w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx          = (y * h + x);
            positions[idx].x = x - w / 2.0f + 0.5f;
            positions[idx].y = getHeight(x, y);
            positions[idx].z = y - h / 2.0f + 0.5f;
        }
    }

    std::vector<glm::vec3> normals;
    normals.resize(w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx      = (y * h + x);
            normals[idx] = getNormal(x, y);
        }
    }

    std::vector<glm::vec2> texCoords;
    texCoords.resize(w * h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx        = (y * h + x);
            texCoords[idx] = getTexCoord(x, y);
        }
    }

    std::vector<GLushort> indices;
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

    return std::make_unique<Mesh>(GL_TRIANGLE_STRIP, positions, normals, texCoords, indices);
}
