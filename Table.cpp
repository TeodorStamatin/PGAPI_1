#include "Table.h"
#include <iostream>

using namespace m2;

float S = 5.0f;

Table::Table() {

    table_height = 0.8f * S;
    table_width = 2.0f * S;
    table_depth = 1.2f * S;
    top_thickness = 0.08f * S;
    leg_offset = 0.15f * S;
}

Table::~Table() {
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteBuffers(1, &lineIBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadIBO);
}

void Table::Init() {

    std::vector<glm::vec3> lineVertices = { glm::vec3(0, 0, 0), glm::vec3(1, 0, 0) };
    std::vector<unsigned int> lineIndices = { 0, 1 };

    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(glm::vec3), lineVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glGenBuffers(1, &lineIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndices.size() * sizeof(unsigned int), lineIndices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Table::DrawTableLeg(const glm::vec3& position, Shader* shader) {

    glBindVertexArray(lineVAO);

    glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);
    glUniform1i(glGetUniformLocation(shader->program, "is_closed_frame"), 0);

    glm::vec3 p0 = glm::vec3(0.02f * S, 0.0f, 0.0f);
    glm::vec3 p1 = glm::vec3(0.03f * S, 0.3f * S, 0.0f);
    glm::vec3 p2 = glm::vec3(0.04f * S, 0.5f * S, 0.0f);
    glm::vec3 p3 = glm::vec3(0.05f * S, table_height, 0.0f);

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 0);
    glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), 6.28318f);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), 40);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), 40);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, 40);
    glBindVertexArray(0);
}

void Table::DrawTableSide(const glm::vec3& start_pos, float length, float thickness, float angleY, Shader* shader) {
    glBindVertexArray(lineVAO);
    glUniform1i(glGetUniformLocation(shader->program, "is_closed_frame"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);

    float convex = 0.05f * S;

    glm::vec3 p0 = glm::vec3(0, 0, 0);
    glm::vec3 p1 = glm::vec3(0, thickness * 0.2f, convex);
    glm::vec3 p2 = glm::vec3(0, thickness * 0.8f, convex);
    glm::vec3 p3 = glm::vec3(0, thickness, 0);

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 1);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), length);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), 60);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), 20);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), start_pos);
    model = glm::rotate(model, angleY, glm::vec3(0, 1, 0));

    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, 60);
    glBindVertexArray(0);
}

void Table::DrawTableTopSides(const glm::vec3& position, float width, float depth, float thickness, Shader* shader) {

    DrawTableSide(position + glm::vec3(width, 0, depth), width, thickness, glm::radians(180.0f), shader);

    DrawTableSide(position + glm::vec3(0, 0, 0), width, thickness, 0.0f, shader);

    DrawTableSide(position + glm::vec3(width, 0, 0), depth, thickness, glm::radians(-90.0f), shader);

    DrawTableSide(position + glm::vec3(0, 0, depth), depth, thickness, glm::radians(90.0f), shader);
}


void Table::DrawTableTopCap(const glm::vec3& position, float width, float depth, bool isTop, Shader* shader) {
    glBindVertexArray(lineVAO);
    glUniform1i(glGetUniformLocation(shader->program, "is_closed_frame"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);

    float gap = 0.005f * S;
    float drawWidth = width + gap;
    float drawDepth = depth + gap;
    glm::vec3 startPos = position - glm::vec3(gap / 2.0f, 0, gap / 2.0f);

    glm::vec3 p0, p1, p2, p3;
    if (isTop) {

        p0 = glm::vec3(0, 0, 0); p1 = glm::vec3(0, 0, 0);
        p2 = glm::vec3(0, 0, drawDepth); p3 = glm::vec3(0, 0, drawDepth);
    }
    else {

        p0 = glm::vec3(0, 0, drawDepth); p1 = glm::vec3(0, 0, drawDepth);
        p2 = glm::vec3(0, 0, 0); p3 = glm::vec3(0, 0, 0);
    }

    glUniform3fv(glGetUniformLocation(shader->program, "control_p0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(shader->program, "control_p3"), 1, glm::value_ptr(p3));

    glUniform1i(glGetUniformLocation(shader->program, "surface_type"), 1);
    glUniform1f(glGetUniformLocation(shader->program, "max_translate"), drawWidth);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_instances"), 40);
    glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), 40);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), startPos);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawElementsInstanced(GL_LINES, 2, GL_UNSIGNED_INT, 0, 40);
    glBindVertexArray(0);
}

void Table::DrawTableTop(const glm::vec3& position, float width, float depth, float thickness, Shader* shader) {

    DrawTableTopSides(position, width, depth, thickness, shader);

    DrawTableTopCap(position, width, depth, false, shader);

    float gap = 0.005f * S;
    DrawTableTopCap(position + glm::vec3(0, thickness - gap / 2, 0), width, depth, true, shader);
}

void Table::Draw(Shader* shader, glm::vec3 position) {
    glm::vec3 base_position = position;

    DrawTableLeg(base_position + glm::vec3(leg_offset, 0, leg_offset), shader);
    DrawTableLeg(base_position + glm::vec3(table_width - leg_offset, 0, leg_offset), shader);
    DrawTableLeg(base_position + glm::vec3(leg_offset, 0, table_depth - leg_offset), shader);
    DrawTableLeg(base_position + glm::vec3(table_width - leg_offset, 0, table_depth - leg_offset), shader);

    float sink_factor = 0.02f * S;
    glm::vec3 top_position = base_position + glm::vec3(0, table_height - sink_factor, 0);

    DrawTableTop(top_position, table_width, table_depth, top_thickness, shader);
}