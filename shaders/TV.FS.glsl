#version 330

// Input from Geometry Shader
in vec3 world_position;
in vec3 world_normal;
in vec2 text_coord;

// Uniforms
uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;
uniform vec3 eye_position;
uniform int has_texture;
uniform int rotate_uv;

layout(location = 0) out vec4 out_color;

void main()
{
    vec2 uv = text_coord;
    if (rotate_uv == 1) {
        uv = vec2(1.0 - text_coord.y, text_coord.x);
    }

    vec4 firework_color = texture(texture_1, uv);

    vec3 N = normalize(world_normal);
    vec3 V = normalize(eye_position - world_position);
    vec3 R = reflect(-V, N);
    
    vec4 reflection_color = texture(texture_cubemap, R);

    out_color = mix(firework_color, reflection_color, 0.25);
    out_color.a = 1.0; 
}
