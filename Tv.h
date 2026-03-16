#pragma once
#include "components/simple_scene.h"

namespace m2
{
    class TV
    {
    public:
        TV();
        ~TV();
        void Init();
        void Draw(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureCase, Texture2D* textureScreen);
        void DrawScreenWithFireworks(glm::vec3 position, float rotationY, Shader* shader, GLuint fireworksTexture);
        void DrawWithFireworks(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureCase, GLuint fireworksTexture);
        void DrawCase(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureCase);
        void DrawScreen(glm::vec3 position, float rotationY, Shader* shader, Texture2D* textureScreen);

    private:
        void DrawRect(Shader* shader, float width, float height, glm::mat4 modelMatrix);
        unsigned int screenVAO, screenVBO, screenIBO;
        float caseWidth = 0.5f;
        float caseHeight = 0.4f;
        float caseDepth = 0.35f;
    };
}