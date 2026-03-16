#pragma once

#include "components/simple_scene.h"
#include "lab_m2/Tema1/Table.h"
#include "components/transform.h"
#include <unordered_map>
#include "lab_m2/Tema1/Tv.h"
#include "lab_m2/Tema1/Lamp.h"
#include "lab_m2/Tema1/Vase.h"
#include "lab_m2/Tema1/Room.h"
#include "core/gpu/particle_effect.h"
#include "lab_m2/lab5/lab5.h"
#include "components/camera.h"

#include <vector>

struct Particle
{
    glm::vec4 position;
    glm::vec4 speed;
    glm::vec4 initialPos;
    glm::vec4 initialSpeed;
    float delay;
    float initialDelay;
    float lifetime;
    float initialLifetime;

    Particle() {}

    Particle(const glm::vec4& pos, const glm::vec4& speed)
    {
        SetInitial(pos, speed);
    }

    void SetInitial(const glm::vec4& pos, const glm::vec4& speed,
        float delay = 0, float lifetime = 0)
    {
        position = pos;
        initialPos = pos;

        this->speed = speed;
        initialSpeed = speed;

        this->delay = delay;
        initialDelay = delay;

        this->lifetime = lifetime;
        initialLifetime = lifetime;
    }
};

namespace m2 {
    class Tema1 : public gfxc::SimpleScene {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void DrawScene(Shader* shader, bool drawTV = true);
        void CreateFramebuffer(int width, int height);

        void CreateFireworksFramebuffer(int width, int height);
        void ResetParticlesFireworks();

    protected:
        glm::vec3 control_p0, control_p1, control_p2, control_p3;
        unsigned int no_of_generated_points, no_of_instances;
        float max_translate, max_rotate;
        int surface_type;
        Table* table;
        TV* tv;
		Lamp* lamp;
		Vase* vase;
		Room* room;
        std::unordered_map<std::string, Texture2D*> mapTextures;

        glm::mat4 lightViewMatrix;
        glm::mat4 lightProjectionMatrix;

        GLuint framebuffer_object;
        GLuint depth_texture;
        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        glm::mat4 lightView;
        glm::mat4 lightProjection;

		int time_elapsed = 0;

        GLuint fireworks_fbo;
        GLuint fireworks_texture;
        ParticleEffect<Particle>* particleEffect;
        float fireworks_time;

        gfxc::Camera* fireworksCamera;
        double totalTime;

        GLuint cubeMapFBO;
        GLuint cubeMapTexture;
        void CreateCubeMapFramebuffer(int width, int height);

        float tvRotation = 0.0f;
    };
}   // namespace m2