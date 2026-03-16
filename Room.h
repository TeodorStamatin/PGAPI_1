#pragma once
#include "components/simple_scene.h"

namespace m2
{
    class Room
    {
    public:
        Room();
        ~Room();

        void Init();
        void Draw(Shader* shader, Texture2D* texture);

    private:
        void DrawWall(Shader* shader, float width, float length, glm::mat4 modelMatrix, bool flipNormal = false);
        unsigned int meshVAO, meshVBO, meshIBO;
    };
}