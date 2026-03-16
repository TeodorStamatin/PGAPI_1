#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform vec3 generator_position;
uniform float deltaTime;
uniform float totalTime;

out float vert_lifetime;
out float vert_iLifetime;
out vec3 vert_color;

struct Particle
{
    vec4 position;
    vec4 speed;
    vec4 iposition;
    vec4 ispeed;
    float delay;
    float iDelay;
    float lifetime;
    float iLifetime;
};

layout(std430, binding = 0) buffer particles {
    Particle data[];
};

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 randomDirection(float seed) {
    float u = rand(vec2(seed, 1.234));
    float v = rand(vec2(seed, 6.789));
    float theta = 2.0 * 3.14159 * u;
    float phi = acos(2.0 * v - 1.0);
    return vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

vec3 randomColor(float seed) {

    float r = rand(vec2(seed, 0.0));
    float g = rand(vec2(seed, 1.0));
    float b = rand(vec2(seed, 2.0));

    float m = max(r, max(g, b));
    return vec3(r, g, b) / m;
}

void main()
{
    int id = gl_VertexID;
    
    vec3 pos = data[id].position.xyz;
    vec3 spd = data[id].speed.xyz;
    float life = data[id].lifetime;
    float iLife = data[id].iLifetime;

    life -= deltaTime;

    if (life < 0) {
        life = iLife;
        pos = data[id].iposition.xyz;
        spd = data[id].ispeed.xyz;
    }

    float gravity = -10.0;
    spd.y += gravity * deltaTime;
    pos += spd * deltaTime;

    if (spd.y <= 0 && abs(spd.x) < 0.001 && abs(spd.z) < 0.001) {
        float mag = rand(vec2(float(id), totalTime)) * 5.0 + 2.0; 
        vec3 dir = randomDirection(float(id) + totalTime);
        spd = dir * mag;
    }

    data[id].position.xyz = pos;
    data[id].speed.xyz = spd;
    data[id].lifetime = life;

    float cycle = floor(totalTime / iLife);
    vert_color = randomColor(cycle);

    gl_Position = Model * vec4(pos + generator_position, 1);
    vert_lifetime = life;
    vert_iLifetime = iLife;
}
