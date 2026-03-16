#pragma once

#include "components/simple_scene.h"
#include "core/gpu/shader.h"

namespace m2 {
    class Table {
    public:
        Table();
        ~Table();

        void Init();
		void Draw(Shader* shader, glm::vec3 position);

    private:
        void DrawTableLeg(const glm::vec3& position, Shader* shader);
        void DrawTableTop(const glm::vec3& position, float width, float depth, float thickness, Shader* shader);
        void DrawTableTopSides(const glm::vec3& position, float width, float depth, float thickness, Shader* shader);
        void DrawTableSide(const glm::vec3& start_pos, float length, float thickness, float angleY, Shader* shader);
        void DrawTableTopCap(const glm::vec3& position, float width, float depth, bool isTop, Shader* shader);

        float table_height;
        float table_width;
        float table_depth;
        float top_thickness;
        float leg_offset;

        GLuint lineVAO, lineVBO, lineIBO;
        GLuint quadVAO, quadVBO, quadIBO;
    };
}