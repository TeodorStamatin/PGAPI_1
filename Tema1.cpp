#include "lab_m2/Tema1/Tema1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m2;

#include "components/transform.h"


/*
 *  To find out more about FrameStart, Update, FrameEnd
 *  and the order in which they are called, see world.cpp.
 */


Tema1::Tema1()
{
    fireworks_time = 0.0f;
    totalTime = 0.0;
    particleEffect = nullptr;
    fireworksCamera = nullptr;
}


Tema1::~Tema1()
{
    delete table;
	delete tv;
    delete lamp;
    delete vase;
	delete room;
    if (particleEffect) delete particleEffect;
    if (fireworksCamera) delete fireworksCamera;

    glDeleteFramebuffers(1, &fireworks_fbo);
    glDeleteTextures(1, &fireworks_texture);
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 8, 8), glm::quat(glm::vec3(-40 * TO_RADIANS, 0, 0)));
    camera->Update();

    ToggleGroundPlane();

    // SHADER OBIECTE
    {
        Shader* shader = new Shader("ShaderTema1");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    
	// SHADER UMBRE
    {
        Shader* shader = new Shader("ShaderShadow");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Shadow.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    CreateFramebuffer(2048, 2048);
    CreateFireworksFramebuffer(1024, 1024);
    CreateCubeMapFramebuffer(1024, 1024);

	// SHADER ARTIFICII
    {
        Shader* shader = new Shader("Fireworks");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Particle_fireworks.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Particle.GS.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "Particle_simple.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

	// SHADER REFLEXII TV
    {
        Shader* shader = new Shader("ShaderTV");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "GeometryShader.glsl"), GL_GEOMETRY_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "Tema1", "shaders", "TV.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "particle2.png");
    ResetParticlesFireworks();
    fireworksCamera = new gfxc::Camera();
    fireworksCamera->SetPositionAndRotation(glm::vec3(0, 5, 5), glm::quat(glm::vec3(0, 0, 0)));
    fireworksCamera->SetOrthographic(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
    fireworksCamera->Update();

    table = new Table();
    table->Init();
    glDisable(GL_CULL_FACE);

    tv = new TV();
    tv->Init();

	lamp = new Lamp();
	lamp->Init();

	vase = new Vase();
	vase->Init();

    room = new Room();
    room->Init();

    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "wood.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "grey.jpg");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "black.jpg");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "black30.jpg");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "greyish.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "ceramic.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "drywall.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "lamp1.png");
    TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, "assets", "textures"), "lamp2.png");

    //{
    //    Mesh* mesh = new Mesh("sphere");
    //    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
    //    meshes[mesh->GetMeshID()] = mesh;
    //}

}

void Tema1::CreateFireworksFramebuffer(int width, int height)
{
    glGenFramebuffers(1, &fireworks_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fireworks_fbo);

    glGenTextures(1, &fireworks_texture);
    glBindTexture(GL_TEXTURE_2D, fireworks_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fireworks_texture, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema1::ResetParticlesFireworks()
{
    unsigned int nrParticles = 5000;

    if (particleEffect) {
        delete particleEffect;
    }

    particleEffect = new ParticleEffect<Particle>();
    particleEffect->Generate(nrParticles, true);

    auto particleSSBO = particleEffect->GetParticleBuffer();
    Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

    for (unsigned int i = 0; i < nrParticles; i++)
    {
        glm::vec4 pos(0, 0, 0, 1);
        glm::vec4 speed(0);

        speed.x = 0;
        speed.z = 0;
        speed.y = 10.0f;

        data[i].SetInitial(pos, speed, 0, 2.5f);
    }

    particleSSBO->SetBufferData(data);
}

void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::CreateFramebuffer(int width, int height)
{
    glGenFramebuffers(1, &framebuffer_object);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void Tema1::DrawScene(Shader* shader, bool drawTV)
{
    {
        Texture2D* texRoom = TextureManager::GetTexture("drywall.png");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texRoom ? texRoom->GetTextureID() : 0);
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

        glDisable(GL_CULL_FACE);
        room->Draw(shader, texRoom);
    }

    Texture2D* texTable = TextureManager::GetTexture("wood.png");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texTable ? texTable->GetTextureID() : 0);
    glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

    glm::vec3 posMasa1 = glm::vec3(-5.0f, 0, -9.0f);
    table->Draw(shader, posMasa1);

    glm::vec3 posMasa2 = glm::vec3(-5.0f, 0, 4.0f);
    table->Draw(shader, posMasa2);

    {
        glm::vec3 lampPos = glm::vec3(2.6f, 0.0f, 2.6f) + glm::vec3(-5.0f, 4.3f, 4.0f);

        Texture2D* texWood = TextureManager::GetTexture("lamp1.png");
        Texture2D* texFabric = TextureManager::GetTexture("lamp2.png");

        lamp->Draw(lampPos, shader, texWood, texFabric);
    }

    if (drawTV)
    {
        glm::vec3 tvPos = glm::vec3(0.0f, 5.1f, -6.0f);
        Texture2D* texCase = TextureManager::GetTexture("greyish.png");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texCase ? texCase->GetTextureID() : 0);
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

        tv->DrawCase(tvPos, tvRotation, shader, texCase);

        if (shader == shaders["ShaderTema1"]) {
            Shader* tvShader = shaders["ShaderTV"];
            tvShader->Use();

            glm::mat4 view = GetSceneCamera()->GetViewMatrix();
            glm::mat4 proj = GetSceneCamera()->GetProjectionMatrix();
            glUniformMatrix4fv(glGetUniformLocation(tvShader->program, "View"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(tvShader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(proj));
            glUniform3fv(glGetUniformLocation(tvShader->program, "eye_position"), 1, glm::value_ptr(GetSceneCamera()->m_transform->GetWorldPosition()));
            
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
            glUniform1i(glGetUniformLocation(tvShader->program, "texture_cubemap"), 2);

            tv->DrawScreenWithFireworks(tvPos, tvRotation, tvShader, fireworks_texture);

            shader->Use();
        } else {

             tv->DrawScreenWithFireworks(tvPos, tvRotation, shader, fireworks_texture);
        }
    }

    {
        Texture2D* texVase = TextureManager::GetTexture("ceramic.png");

        glm::vec3 vasePos1 = glm::vec3(0.0f, 0.0f, 0.0f) + glm::vec3(+3.0f, 4.3f, -6.0f);
        vase->Draw(vasePos1, shader, texVase);

        glm::vec3 vasePos2 = glm::vec3(0.0f, 0.0f, 0.0f) + glm::vec3(-3.0f, 4.3f, -6.0f);
        vase->Draw(vasePos2, shader, texVase);
    }
}

void Tema1::CreateCubeMapFramebuffer(int width, int height)
{
    glGenFramebuffers(1, &cubeMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, cubeMapFBO);

    glGenTextures(1, &cubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

    for (unsigned int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (GLEW_EXT_texture_filter_anisotropic) {
        float maxAnisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }

    GLuint cubeMapDepthTexture;
    glGenTextures(1, &cubeMapDepthTexture);
    glBindTexture(GL_TEXTURE_2D, cubeMapDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, cubeMapDepthTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "CubeMap Framebuffer incomplete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tema1::Update(float deltaTimeSeconds)
{

    {
        glm::vec3 tvPos = glm::vec3(0.0f, 5.1f, -6.0f);

        glm::ivec2 resolution = window->GetResolution();

        glBindFramebuffer(GL_FRAMEBUFFER, cubeMapFBO);
        glViewport(0, 0, 1024, 1024);

        Shader* shader = shaders["ShaderTema1"];
        shader->Use();

        float noise = (float)((rand() % 100) / 100.0f) * 0.2f;
        float intensity = 1.0f + 0.5f * sin(totalTime * 3.0f) + 0.3f * cos(totalTime * 10.0f) + noise;
        glUniform1f(glGetUniformLocation(shader->program, "light_intensity"), intensity);

        glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(lightPosition));
        glUniform3fv(glGetUniformLocation(shader->program, "light_direction"), 1, glm::value_ptr(lightDirection));

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glUniform1i(glGetUniformLocation(shader->program, "shadow_map"), 1);

        glUniformMatrix4fv(glGetUniformLocation(shader->program, "light_space_view"), 1, GL_FALSE, glm::value_ptr(lightView));
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "light_space_projection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 views[6] =
        {
            glm::lookAt(tvPos, tvPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(tvPos, tvPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(tvPos, tvPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(tvPos, tvPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(tvPos, tvPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(tvPos, tvPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        };

        for (int i = 0; i < 6; i++)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMapTexture, 0);
            
            glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE, glm::value_ptr(views[i]));
            glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(tvPos));

            DrawScene(shader, false);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, resolution.x, resolution.y);
    }

    {
        glBindFramebuffer(GL_FRAMEBUFFER, fireworks_fbo);
        glViewport(0, 0, 1024, 1024);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_ONE, GL_ONE);

        auto shader = shaders["Fireworks"];
        shader->Use();

        glUniform3fv(glGetUniformLocation(shader->program, "generator_position"), 1, glm::value_ptr(glm::vec3(0, -3, 0)));
        glUniform1f(glGetUniformLocation(shader->program, "deltaTime"), deltaTimeSeconds);
        glUniform1f(glGetUniformLocation(shader->program, "totalTime"), (float)totalTime);
        glUniform1f(glGetUniformLocation(shader->program, "offset"), 0.15f);

        TextureManager::GetTexture("particle2.png")->BindToTextureUnit(GL_TEXTURE0);

        particleEffect->Render(fireworksCamera, shader);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    totalTime += deltaTimeSeconds;
    glm::vec3 lampBasePos = glm::vec3(2.6f, 0.0f, 2.6f) + glm::vec3(-5.0f, 4.3f, 4.0f);
    lightPosition = glm::vec3(2.7f, 1.8f, 2.3f) + glm::vec3(-5.0f, 4.3f, 4.0f);
    lightDirection = glm::normalize(glm::vec3(2.4f, -10.0f, -12.0f)); 

    // umbre
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);
        glViewport(0, 0, 2048, 2048);
        glClear(GL_DEPTH_BUFFER_BIT);

        lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 50.0f);
        lightView = glm::lookAt(lightPosition, lightPosition + lightDirection, glm::vec3(0, 1, 0));

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        Shader* shadowShader = shaders["ShaderShadow"];
        shadowShader->Use();

        glUniformMatrix4fv(glGetUniformLocation(shadowShader->program, "View"), 1, GL_FALSE, glm::value_ptr(lightView));
        glUniformMatrix4fv(glGetUniformLocation(shadowShader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

        DrawScene(shadowShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
    }

    // randare finala
    {

        glm::ivec2 resolution = window->GetResolution();
        glViewport(0, 0, resolution.x, resolution.y);

        ClearScreen(glm::vec3(0.121, 0.168, 0.372));

        Shader* mainShader = shaders["ShaderTema1"];
        mainShader->Use();

        glm::mat4 view = GetSceneCamera()->GetViewMatrix();
        glm::mat4 proj = GetSceneCamera()->GetProjectionMatrix();
        glUniformMatrix4fv(glGetUniformLocation(mainShader->program, "View"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(mainShader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniform3fv(glGetUniformLocation(mainShader->program, "eye_position"), 1, glm::value_ptr(GetSceneCamera()->m_transform->GetWorldPosition()));

        glUniform3fv(glGetUniformLocation(mainShader->program, "light_position"), 1, glm::value_ptr(lightPosition));
        glUniform3fv(glGetUniformLocation(mainShader->program, "light_direction"), 1, glm::value_ptr(lightDirection));

        glUniformMatrix4fv(glGetUniformLocation(mainShader->program, "light_space_view"), 1, GL_FALSE, glm::value_ptr(lightView));
        glUniformMatrix4fv(glGetUniformLocation(mainShader->program, "light_space_projection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glUniform1i(glGetUniformLocation(mainShader->program, "shadow_map"), 1);

        float noise = (float)((rand() % 100) / 100.0f) * 0.2f;
        float intensity = 1.0f + 0.5f * sin(totalTime * 3.0f) + 0.3f * cos(totalTime * 10.0f) + noise;
        glUniform1f(glGetUniformLocation(mainShader->program, "light_intensity"), intensity);

        DrawScene(mainShader);

        //{
        //    glm::mat4 modelMatrix = glm::mat4(1);
        //    modelMatrix = glm::translate(modelMatrix, lightPosition);
        //    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        //    RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
        //}
    }
}

void Tema1::FrameEnd()
{
    //DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see input_controller.h.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // You can move the control points around by using the dedicated key,
    // in combination with Ctrl, Shift, or both.
    float delta = deltaTime;
    auto keyMaps = std::vector<std::pair<glm::vec3&, uint32_t>>
    {
        // { control_p0, GLFW_KEY_1 },
        // { control_p1, GLFW_KEY_2 },
        { control_p2, GLFW_KEY_3 },
        { control_p3, GLFW_KEY_4 }
    };

    float rotSpeed = 2.0f;
    if (window->KeyHold(GLFW_KEY_1)) {
        tvRotation += rotSpeed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_2)) {
        tvRotation -= rotSpeed * deltaTime;
    }

    for (const auto& k : keyMaps)
    {
        if (window->KeyHold(k.second))
        {
            if (mods & GLFW_MOD_SHIFT && mods & GLFW_MOD_CONTROL)
            {
                k.first.y -= delta;
            }
            else if (mods & GLFW_MOD_CONTROL)
            {
                k.first.y += delta;
            }
            else if (mods & GLFW_MOD_SHIFT)
            {
                k.first.x -= delta;
            }
            else
            {
                k.first.x += delta;
            }

            std::cout << glm::vec2(control_p0) << glm::vec2(control_p1) << glm::vec2(control_p2) << glm::vec2(control_p3) << "\n";
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // TODO(student): Use keys to change the number of instances and the
    // number of generated points. Avoid the camera keys, and avoid the
    // the keys from OnInputUpdate.
    if (key == GLFW_KEY_EQUAL) {
        no_of_generated_points++;
    }

    if (key == GLFW_KEY_MINUS) {
        no_of_generated_points--;
    }

    if (key == GLFW_KEY_UP) {
        no_of_instances++;
    }

    if (key == GLFW_KEY_DOWN && no_of_instances > 1) {
        no_of_instances--;
    }

    if (key == GLFW_KEY_T) {
        surface_type = 1;
    }
    if (key == GLFW_KEY_R) {
        surface_type = 0;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Tema1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
