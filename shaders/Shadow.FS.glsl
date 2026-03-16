#version 330

in vec3 world_position;
in vec3 world_normal;
in vec2 text_coord;

uniform sampler2D texture_1;
uniform int has_texture;

void main()
{
    if (has_texture == 1) {
        float alpha = texture(texture_1, text_coord).a;
        if (alpha < 0.5)
            discard;
    }
}