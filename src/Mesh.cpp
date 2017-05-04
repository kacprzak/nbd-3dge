#include "Mesh.h"

#include "ObjLoader.h"
#include "Util.h"
#include "Logger.h"
#include <cstring>

Mesh::Mesh(GLenum primitive,
           const std::vector<GLfloat> &vertices,
           const std::vector<GLfloat> &normals,
           const std::vector<GLfloat> &texcoords,
           const std::vector<GLushort> &indices)
    : m_primitive{primitive}
{
    m_numberOfVertices = vertices.size();
    m_numberOfElements = indices.size();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(NUM_BUFFERS, m_buffers);

    LOG_TRACE << "Loaded Mesh: " << "\t vaoId: " << m_vao << '\n'
              << "  Vertices: " << vertices.size()/3 << "\t id: " << m_buffers[POSITIONS] << '\n'
              << "  Normals: " << normals.size()/3 << "\t id: " << m_buffers[NORMALS] << '\n'
              << "  TexCoords: " << texcoords.size()/2 << "\t id: " << m_buffers[TEXCOORDS] << '\n'
              << "  Indices: " << indices.size() << "\t id: " << m_buffers[INDICES] << '\n'
              << "  Primitive: " << primitive;

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POSITIONS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (normals.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[NORMALS]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    if (texcoords.size() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffers[TEXCOORDS]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (indices.size() > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDICES]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(NUM_BUFFERS, m_buffers);

    LOG_INFO << "Released: " << m_vao;
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

    // Draw
    if (m_numberOfElements == 0) {
        glDrawArrays(m_primitive, start, count);
    } else {
        glDrawElements(m_primitive, count, GL_UNSIGNED_SHORT, 0);
    }
}

Mesh *Mesh::fromWavefrontObj(const std::string& objfileName)
{
    std::string name = extractFilename(objfileName);
    LOG_INFO << "Loading... " << name;

    ObjLoader objLoader;
    objLoader.load(objfileName);

    return new Mesh{GLenum(objLoader.primitive()),
            objLoader.vertices(),
            objLoader.normals(),
            objLoader.texCoords(),
            objLoader.indices()};
}

Mesh* Mesh::fromHeightmap(const std::vector<unsigned char>& heights, int w, int h,
                          float amplitude, float textureStrech)
{
    if (w % 2 != 0 || h % 2 != 0) {
        throw std::runtime_error{"Heightmap with odd size is not supported."};
    }
    
    const auto getHeight = [&](int x, int y)->float {
        return ((heights[y * h + x]/255.0f) - 0.5) * amplitude;
    };

    const auto getNormal = [&](int x, int y)->glm::vec3 {
        auto n =  glm::vec3{0.0f, 0.0f, 2.0};

        float center = getHeight(x, y);

        float left = x > 0 ? getHeight(x - 1, y) : center;
        float right = x < w - 1 ? getHeight(x + 1, y) : center;

        n[0] = right - left;

        float bottom = y > 0 ? getHeight(x, y - 1) : center;
        float top = y < h - 1 ? getHeight(x, y + 1) : center;

        n[1] = top - bottom;

        // W have normal in texture space where z is up but in OpenGL y is up
        n[0] = -n[0];
        n[2] = -n[1];
        n[1] = 2.0f;

        return glm::normalize(n);
    };

    const auto getTexCoord = [textureStrech](int x, int y)->glm::vec2 {
        auto texCoord = glm::vec2{0.0, 0.0};

        texCoord[0] = x / textureStrech;
        texCoord[1] = y / textureStrech;

        return texCoord;
    };

    
    std::vector<GLfloat> vertices;
    vertices.resize(w * h * 3);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = (y * h + x) * 3;
            vertices[idx] = x - w/2.0f;
            vertices[idx + 1] = getHeight(x, y);
            vertices[idx + 2] = y - h/2.0f;
        }
    }

    std::vector<GLfloat> normals;
    normals.resize(w * h * 3);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = (y * h + x) * 3;
            auto n = getNormal(x, y);
            normals[idx] = n[0];
            normals[idx + 1] = n[1];
            normals[idx + 2] = n[2];
        }
    }

    std::vector<GLfloat> texCoords;
    texCoords.resize(w * h * 2);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = (y * h + x) * 2;
            auto t = getTexCoord(x, y);
            texCoords[idx] = t[0];
            texCoords[idx + 1] = t[1];
        }
    }
    
    std::vector<GLushort> indices;
    indices.resize(w * h + (w-1) * (h-2));

    int ind = 0;
    for (int y = 0; y < h - 1; ++y) {
        if (y % 2 == 0) {
            for (int x = 0; x < w; ++x) {
                indices[ind++] = x + y * h;
                indices[ind++] = x + (y+1) * h;
            }
        } else {
            for (int x = w - 1; x > 0; --x) {
                indices[ind++] = x + (y+1) * h;
                indices[ind++] = (x-1) + y * h;
            }
        }
    }

    return new Mesh{GL_TRIANGLE_STRIP,
            vertices, normals,
            texCoords, indices};
}
