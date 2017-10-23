#include "Mesh.h"

#include "Logger.h"
#include "ObjLoader.h"
#include "Util.h"

#include <array>
#include <cstring>
#include <limits>

Mesh::Mesh(GLenum primitive, const std::vector<GLfloat>& vertices,
           const std::vector<GLfloat>& normals, const std::vector<GLfloat>& texcoords,
           const std::vector<GLushort>& indices)
    : m_primitive{primitive}
{
    std::fill(std::begin(m_bufferSizes), std::end(m_bufferSizes), 0);

    m_numberOfVertices = vertices.size();
    m_numberOfElements = indices.size();

    const std::vector<GLfloat>& tangents = calculateTangents(vertices, normals, texcoords, indices);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(NUM_BUFFERS, m_buffers);

    auto aabb = calculateAABB(vertices);
    LOG_INFO << "Loaded Mesh: " << m_vao;
    LOG_TRACE << "  Vertices: " << vertices.size() / 3 << "\t id: " << m_buffers[POSITIONS] << '\n'
              << "  Normals: " << normals.size() / 3 << "\t id: " << m_buffers[NORMALS] << '\n'
              << "  Tangents: " << tangents.size() / 3 << "\t id: " << m_buffers[TANGENTS] << '\n'
              << "  TexCoords: " << texcoords.size() / 2 << "\t id: " << m_buffers[TEXCOORDS]
              << '\n'
              << "  Indices: " << indices.size() << "\t id: " << m_buffers[INDICES] << '\n'
              << "  Primitive: " << primitive << '\n'
              << "  Dimensions: (" << aabb[1] - aabb[0] << " x " << aabb[3] - aabb[2] << " x "
              << aabb[5] - aabb[4] << ")";

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    m_bufferSizes[POSITIONS] = sizeof(float) * vertices.size();
    glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[POSITIONS], &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMALS]);
        m_bufferSizes[NORMALS] = sizeof(float) * normals.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[NORMALS], &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (tangents.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TANGENTS]);
        m_bufferSizes[TANGENTS] = sizeof(float) * tangents.size();
        glBufferData(GL_ARRAY_BUFFER, m_bufferSizes[TANGENTS], &tangents[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (texcoords.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORDS]);
        m_bufferSizes[TEXCOORDS] = sizeof(float) * texcoords.size();
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

std::array<float, 6> Mesh::calculateAABB(const std::vector<float>& positions)
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

    for (size_t i = 0; i < positions.size(); i = i + 3) {
        retVal[0] = std::min(retVal[0], positions[i]);
        retVal[1] = std::max(retVal[1], positions[i]);
        retVal[2] = std::min(retVal[2], positions[i + 1]);
        retVal[3] = std::max(retVal[3], positions[i + 1]);
        retVal[4] = std::min(retVal[4], positions[i + 2]);
        retVal[5] = std::max(retVal[5], positions[i + 2]);
    }

    return retVal;
}

//------------------------------------------------------------------------------

std::vector<GLfloat> Mesh::calculateTangents(const std::vector<GLfloat>& vertices,
                                             const std::vector<GLfloat>& normals,
                                             const std::vector<GLfloat>& texcoords,
                                             const std::vector<GLushort>& indices)
{
    std::vector<GLfloat> tangents;

    if (texcoords.empty()) return tangents;

    tangents.resize(normals.size());

    for (size_t i = 0; i < indices.size(); i += 3) {
        auto index0 = 3 * indices[i];
        auto index1 = 3 * indices[i + 1];
        auto index2 = 3 * indices[i + 2];
        // Shortcuts for vertices
        glm::vec3 v0{vertices[index0 + 0], vertices[index0 + 1], vertices[index0 + 2]};
        glm::vec3 v1{vertices[index1 + 0], vertices[index1 + 1], vertices[index1 + 2]};
        glm::vec3 v2{vertices[index2 + 0], vertices[index2 + 1], vertices[index2 + 2]};

        // Shortcuts for UVs
        glm::vec2 st0{texcoords[index0 + 0], texcoords[index0 + 1]};
        glm::vec2 st1{texcoords[index1 + 0], texcoords[index1 + 1]};
        glm::vec2 st2{texcoords[index2 + 0], texcoords[index2 + 1]};

        // Edges of the triangle : postion delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // ST delta
        glm::vec2 deltaST1 = st1 - st0;
        glm::vec2 deltaST2 = st2 - st0;

        float r           = 1.0f / (deltaST1.x * deltaST2.y - deltaST1.y * deltaST2.x);
        glm::vec3 tangent = (deltaPos1 * deltaST2.y - deltaPos2 * deltaST1.y) * r;
        // glm::vec3 bitangent = (deltaPos2 * deltaST1.x - deltaPos1 * deltaST2.x) * r;

        tangents[index0 + 0] = tangent.x;
        tangents[index0 + 1] = tangent.y;
        tangents[index0 + 2] = tangent.z;
        tangents[index1 + 0] = tangent.x;
        tangents[index1 + 1] = tangent.y;
        tangents[index1 + 2] = tangent.z;
        tangents[index2 + 0] = tangent.x;
        tangents[index2 + 1] = tangent.y;
        tangents[index2 + 2] = tangent.z;
    }

    return tangents;
}

//------------------------------------------------------------------------------

std::unique_ptr<Mesh> Mesh::fromWavefrontObj(const std::string& objfileName)
{
    // std::string name = extractFilename(objfileName);

    ObjLoader objLoader;
    objLoader.load(objfileName);

    return std::make_unique<Mesh>(GLenum(objLoader.primitive()), objLoader.vertices(),
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

    std::vector<GLfloat> vertices;
    vertices.resize(w * h * 3);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx           = (y * h + x) * 3;
            vertices[idx]     = x - w / 2.0f + 0.5f;
            vertices[idx + 1] = getHeight(x, y);
            vertices[idx + 2] = y - h / 2.0f + 0.5f;
        }
    }

    std::vector<GLfloat> normals;
    normals.resize(w * h * 3);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx          = (y * h + x) * 3;
            auto n           = getNormal(x, y);
            normals[idx]     = n[0];
            normals[idx + 1] = n[1];
            normals[idx + 2] = n[2];
        }
    }

    std::vector<GLfloat> texCoords;
    texCoords.resize(w * h * 2);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx            = (y * h + x) * 2;
            auto t             = getTexCoord(x, y);
            texCoords[idx]     = t[0];
            texCoords[idx + 1] = t[1];
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

    return std::make_unique<Mesh>(GL_TRIANGLE_STRIP, vertices, normals, texCoords, indices);
}
