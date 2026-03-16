#pragma once
#include "components/simple_scene.h"

namespace m2
{
    class Vase
    {
    public:
        Vase();
        ~Vase();

        void Init();
        void Draw(glm::vec3 position, Shader* shader, Texture2D* texture);

    private:
        unsigned int meshVAO, meshVBO, meshIBO;
    };
}