#include "TV.h"
#include <vector>

using namespace m2;
using namespace std;

TV::TV() {}

TV::~TV() {
    glDeleteVertexArrays(1, &screenVAO);
    glDeleteBuffers(1, &screenVBO);
    glDeleteBuffers(1, &screenIBO);
}

void TV::Init() {

    std::vector<glm::vec3> lineVertices = { glm::vec3(0, 0, 0), glm::vec3(1, 0, 0) };
    std::vector<unsigned int> lineIndices = { 0, 1 };

    glGenVertexArrays(1, &screenVAO);
    glBindVertexArray(screenVAO);

    glGenBuffers(1, &screenVBO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glGenBuffers(1, &screenIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(unsigned int), lineIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void TV::DrawRect(Shader* shader, float width, float length, glm::mat4 modelMatrix) {
    glBindVertexArray(screenVAO);

    glm::vec3 p0 = glm::vec3(0, 0, 0);
    glm::vec3 p1 = glm::vec3(0, 0, length);
    glm::vec3 p2 = glm::vec3(0, 0, length);
    glm::vec3 p3 = glm::vec3(0, 0, length);

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 1);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), width);

    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), 20);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), 20);

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, 20);
}

void TV::DrawCase(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureCase) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureCase ? textureCase->GetTextureID() : 0);
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    glUniform1i(glGetUniformLocation(shader->program, "rotate_uv"), 0); 

    glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), position);
    baseModel = glm::rotate(baseModel, rotationY, glm::vec3(0, 1, 0));
    baseModel = glm::scale(baseModel, glm::vec3(4.0f));

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(caseWidth/2, -caseHeight/2, -caseDepth/2));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1)); 
        DrawRect(shader, caseWidth, caseDepth, model);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-caseWidth / 2, caseHeight / 2, -caseDepth / 2));
        DrawRect(shader, caseWidth, caseDepth, model);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-caseWidth/2, -caseHeight/2, -caseDepth/2));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        DrawRect(shader, caseWidth, caseHeight, model);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-caseWidth/2, caseHeight/2, caseDepth/2)); 
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
        DrawRect(shader, caseWidth, caseHeight, model);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(-caseWidth/2, -caseHeight/2, -caseDepth/2));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
        DrawRect(shader, caseHeight, caseDepth, model);
    }

    {
        glm::mat4 model = glm::translate(baseModel, glm::vec3(caseWidth/2, caseHeight/2, -caseDepth/2));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
        DrawRect(shader, caseHeight, caseDepth, model);
    }
}

void TV::DrawScreen(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureScreen) {
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, textureScreen ? textureScreen->GetTextureID() : 0);
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    glUniform1i(glGetUniformLocation(shader->program, "rotate_uv"), 1); 

    float screenW = caseWidth * 0.75f;
    float screenH = caseHeight * 0.75f;

    float radius = screenW / 2.0f;

    glm::vec3 p0 = glm::vec3(radius, -screenH / 2, 0);
    glm::vec3 p1 = glm::vec3(radius * 1.1f, -screenH / 6, 0);
    glm::vec3 p2 = glm::vec3(radius * 1.1f, screenH / 6, 0);
    glm::vec3 p3 = glm::vec3(radius, screenH / 2, 0);

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 0);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), 3.14159f);

    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), 40);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), 40);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, rotationY, glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(4.0f));
    model = glm::translate(model, glm::vec3(0, 0, caseDepth /2 ));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.2f));

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, 60);
}

void TV::Draw(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureCase, Texture2D* textureScreen) {
    glBindVertexArray(screenVAO);
    DrawCase(position, rotationY, shader, textureCase);

    DrawScreen(position, rotationY, shader, textureScreen);
    glBindVertexArray(0);
}

void TV::DrawScreenWithFireworks(glm::vec3 position, float rotationY, Shader* shader, GLuint fireworksTexture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fireworksTexture);
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    glUniform1i(glGetUniformLocation(shader->program, "rotate_uv"), 1);
    
    float screenW = caseWidth * 0.75f;
    float screenH = caseHeight * 0.75f;

    float profileHalfLen = screenW / 2.0f; 

    float extrusionLen = screenH;

    float bulgeZ = 0.05f; 
    
    glm::vec3 p0 = glm::vec3(0, -profileHalfLen, 0);
    glm::vec3 p1 = glm::vec3(0, -profileHalfLen/3, bulgeZ);
    glm::vec3 p2 = glm::vec3(0, profileHalfLen/3, bulgeZ);
    glm::vec3 p3 = glm::vec3(0, profileHalfLen, 0);

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 1);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), extrusionLen);

    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), 40);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), 40);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, rotationY, glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(4.0f));
    model = glm::translate(model, glm::vec3(0, 0, caseDepth / 2 + 0.001f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(-extrusionLen/2.0f, 0, 0));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, 40);
}

void TV::DrawWithFireworks(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureCase, GLuint fireworksTexture) {
    glBindVertexArray(screenVAO);
    DrawCase(position, rotationY, shader, textureCase);
    DrawScreenWithFireworks(position, rotationY, shader, fireworksTexture);
    glBindVertexArray(0);
}