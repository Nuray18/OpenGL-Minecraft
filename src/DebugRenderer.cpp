#include "headers/debuging/debugRenderer.h"

unsigned int DebugRenderer::VAO = 0;
unsigned int DebugRenderer::VBO = 0;

void DebugRenderer::drawAABB(const AABB &box)
{
    // GLint mode;
    // glGetIntegerv(GL_POLYGON_MODE, &mode);

    // std::cout << mode << std::endl;

    // glLineWidth(5.0f);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    vec3 min = box.min;
    vec3 max = box.max;

    /*
          7---------6
         /|        /|
        / |       / |
       3---------2  |
       |  4------|--5
       | /       | /
       |/        |/
       0---------1
    */
    vec3 vertices[8] = {
        vec3(min.x, min.y, min.z), // 0
        vec3(max.x, min.y, min.z), // 1
        vec3(max.x, max.y, min.z), // 2
        vec3(min.x, max.y, min.z), // 3
        vec3(min.x, min.y, max.z), // 4
        vec3(max.x, min.y, max.z), // 5
        vec3(max.x, max.y, max.z), // 6
        vec3(min.x, max.y, max.z)  // 7
    };

    unsigned int indices[]{
        // alt
        0, 1,
        1, 2,
        2, 3,
        3, 0,

        // ust
        4, 5,
        5, 6,
        6, 7,
        7, 4,

        // dikey
        0, 4,
        1, 5,
        2, 6,
        3, 7};

    float lineVertices[72];

    for (int i = 0; i < 24; i++)
    {
        vec3 p = vertices[indices[i]];

        lineVertices[i * 3 + 0] = p.x;
        lineVertices[i * 3 + 1] = p.y;
        lineVertices[i * 3 + 2] = p.z;
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(lineVertices),
        lineVertices);

    glDrawArrays(GL_LINES, 0, 24);

    glBindVertexArray(0);
};

void DebugRenderer::init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * 72,
        nullptr,
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 3,
        (void *)0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
