#pragma once
#include "components/simple_scene.h"
#include <vector>

// Structura particulei (compatibilă cu shaderele din lab5)
struct FireworkParticle {
    glm::vec4 position;      // xyz = poziție, w = state (0=ascensiune, 1=explodat)
    glm::vec4 speed;
    glm::vec4 iposition;     // poziție inițială
    glm::vec4 ispeed;        // viteză inițială
    float delay;
    float iDelay;
    float lifetime;
    float iLifetime;
};

class FireworksEffect {
public:
    FireworksEffect(int numParticles = 1000) : numParticles(numParticles) {
        totalLifetime = 5.0f;
        currentTime = 0.0f;

        RegenerateColor();
        InitializeParticles();
    }

    ~FireworksEffect() {
        if (particleBuffer) glDeleteBuffers(1, &particleBuffer);
    }

    void InitializeParticles() {
        std::vector<FireworkParticle> particles(numParticles);

        // Poziție START (în coordonatele tale, ajustează dacă e nevoie)
        glm::vec3 startPos = glm::vec3(0, -2, 0);

        for (int i = 0; i < numParticles; i++) {
            // TOATE pornesc din același punct
            particles[i].position = glm::vec4(startPos, 0.0f); // .w = 0 = nu a explodat
            particles[i].iposition = glm::vec4(startPos, 0.0f);

            // TOATE au aceeași viteză inițială (în sus)
            glm::vec3 upSpeed = glm::vec3(0, 8.0f, 0);
            particles[i].speed = glm::vec4(upSpeed, 0.0f);
            particles[i].ispeed = glm::vec4(upSpeed, 0.0f);

            particles[i].delay = 0.0f;
            particles[i].iDelay = 0.0f;
            particles[i].lifetime = totalLifetime;
            particles[i].iLifetime = totalLifetime;
        }

        // Creăm SSBO
        glGenBuffers(1, &particleBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
            particles.size() * sizeof(FireworkParticle),
            particles.data(),
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void Update(float deltaTime) {
        currentTime += deltaTime;

        // Reset când expiră lifetime-ul
        if (currentTime >= totalLifetime) {
            currentTime = 0.0f;
            RegenerateColor();
            InitializeParticles();
        }
    }

    void Render(Shader* shader) {
        if (!shader || !shader->GetProgramID()) return;

        shader->Use();

        // Bind SSBO
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);

        // Draw
        glDrawArrays(GL_POINTS, 0, numParticles);
    }

    glm::vec3 GetCurrentColor() const { return currentColor; }

private:
    void RegenerateColor() {
        int colorChoice = rand() % 6;
        switch (colorChoice) {
        case 0: currentColor = glm::vec3(1.0f, 0.3f, 0.3f); break; // Roșu
        case 1: currentColor = glm::vec3(0.3f, 1.0f, 0.3f); break; // Verde
        case 2: currentColor = glm::vec3(0.3f, 0.3f, 1.0f); break; // Albastru
        case 3: currentColor = glm::vec3(1.0f, 1.0f, 0.3f); break; // Galben
        case 4: currentColor = glm::vec3(1.0f, 0.3f, 1.0f); break; // Magenta
        case 5: currentColor = glm::vec3(0.3f, 1.0f, 1.0f); break; // Cyan
        }
    }

    int numParticles;
    float totalLifetime;
    float currentTime;
    glm::vec3 currentColor;

    GLuint particleBuffer = 0;
};