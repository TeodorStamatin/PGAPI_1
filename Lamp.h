#pragma once
#include "components/simple_scene.h"

namespace m2
{
    class Lamp
    {
    public:
        Lamp();
        ~Lamp();

        void Init();
        void Draw(glm::vec3 position, Shader* shader, Texture2D* texBase, Texture2D* texShade);

    private:
        void DrawBase(Shader* shader, glm::mat4 modelMatrix, Texture2D* texture);
        void DrawLampshade(Shader* shader, glm::mat4 modelMatrix, Texture2D* texture);

        unsigned int meshVAO, meshVBO, meshIBO;
    };
}