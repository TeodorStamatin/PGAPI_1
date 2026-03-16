#include "Vase.h"
#include <vector>

using namespace m2;
using namespace std;

#define VASE_RES 40

Vase::Vase() {}

Vase::~Vase() {
    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &meshVBO);
    glDeleteBuffers(1, &meshIBO);
}

void Vase::Init() {

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

void Vase::Draw(glm::vec3 position, Shader* shader, Texture2D* texture) {
    glBindVertexArray(meshVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture ? texture->GetTextureID() : 0);
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "has_texture"), texture ? 1 : 0);

    glUniform1i(glGetUniformLocation(shader->program, "is_closed_frame"), 0);

    float h = 0.6f;

    glm::vec3 p0 = glm::vec3(0.05f, 0, 0);
    glm::vec3 p1 = glm::vec3(0.15f, h * 0.25f, 0);
    glm::vec3 p2 = glm::vec3(0.25f, h * 0.75f, 0);
    glm::vec3 p3 = glm::vec3(0.10f, h, 0);

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 0);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), 6.28318f);

    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), VASE_RES);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), VASE_RES);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

    float s = 3.5f; 
    model = glm::scale(model, glm::vec3(s)); 

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(model));

    glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.9f);
    glUniform3fv(glGetUniformLocation(shader->program, "material_color"), 1, glm::value_ptr(color));

    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, VASE_RES);

    glBindVertexArray(0);
}