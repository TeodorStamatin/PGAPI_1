#include "Room.h"
#include <vector>

using namespace m2;
using namespace std;

#define ROOM_RES 40

Room::Room() {}

Room::~Room() {
    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &meshVBO);
    glDeleteBuffers(1, &meshIBO);
}

void Room::Init() {

    std::vector<glm::vec3> lineVertices = { glm::vec3(0, 0, 0), glm::vec3(1, 0, 0) };
    std::vector<unsigned int> lineIndices = { 0, 1 };

    glGenVertexArrays(1, &meshVAO);
    glBindVertexArray(meshVAO);
    glGenBuffers(1, &meshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glGenBuffers(1, &meshIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(unsigned int), lineIndices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Room::DrawWall(Shader* shader, float width, float length, glm::mat4 modelMatrix, bool flipNormal) {
    glBindVertexArray(meshVAO);

    float gap = 0.1f;  
    float drawWidth = width + gap;
    float drawLength = length + gap;

    glm::vec3 p0 = glm::vec3(0, 0, -gap/2.0f);
    glm::vec3 p1 = glm::vec3(0, 0, -gap/2.0f);
    glm::vec3 p2 = glm::vec3(0, 0, drawLength - gap/2.0f);
    glm::vec3 p3 = glm::vec3(0, 0, drawLength - gap/2.0f);

    glm::vec3 c0 = p0;
    glm::vec3 c1 = p1;
    glm::vec3 c2 = p2;
    glm::vec3 c3 = p3;

    if (flipNormal) {
        c0 = p3; c1 = p2; c2 = p1; c3 = p0;
    }

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(c0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(c1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(c2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(c3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 1);

    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), drawWidth);

    modelMatrix = glm::translate(modelMatrix, glm::vec3(-gap/2.0f, 0, 0));

    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), ROOM_RES);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), ROOM_RES);

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, ROOM_RES);
}

void Room::Draw(Shader* shader, Texture2D* texture) {

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture ? texture->GetTextureID() : 0);
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);

    float size = 24.0f;
    float halfSize = size / 2.0f;

    glm::vec3 center = glm::vec3(0, size / 2.0f, 0);
    glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), center);

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-halfSize, -halfSize, -halfSize));
        DrawWall(shader, size, size, model, false);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-halfSize, halfSize, -halfSize));
        DrawWall(shader, size, size, model, true);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-halfSize, -halfSize, -halfSize));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        DrawWall(shader, size, size, model, true);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-halfSize, -halfSize, halfSize));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        DrawWall(shader, size, size, model, false);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-halfSize, -halfSize, -halfSize));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
        DrawWall(shader, size, size, model, true);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(halfSize, -halfSize, -halfSize));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
        DrawWall(shader, size, size, model, false);
    }
}