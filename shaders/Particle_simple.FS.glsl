#version 430

// Input
layout(location = 0) in vec2 texture_coord;
layout(location = 1) in float geom_lifetime;
layout(location = 2) in float geom_iLifetime;
layout(location = 3) in vec3 geom_color;

// Uniform properties
uniform sampler2D texture_1;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec4 texColor = texture(texture_1, texture_coord);
    if (texColor.a < 0.5) discard;

    out_color = vec4(geom_color * texColor.rgb, 1.0);
}
