#version 330

in vec3 world_position;
in vec3 world_normal;
in vec2 text_coord;

uniform sampler2D texture_1;
uniform sampler2D shadow_map; 
uniform int has_texture;
uniform vec3 material_color;

uniform vec3 light_position;
uniform vec3 light_direction;
uniform vec3 eye_position;

uniform float light_intensity; 

uniform mat4 light_space_view;
uniform mat4 light_space_projection;

layout(location = 0) out vec4 out_color;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 N, vec3 L)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) return 0.0;
    float closestDepth = texture(shadow_map, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    float bias = max(0.00005 * (1.0 - dot(N, L)), 0.000005);
    
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec4 color;
    if (has_texture == 1) {
        color = texture(texture_1, text_coord);
        if(color.a < 0.5) discard;
    } else {
        color = vec4(material_color, 1.0);
    }

    vec3 N = normalize(world_normal);
    vec3 L = normalize(light_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    float ambient_strength = 0.15;
    float specular_strength = 0.5;
    float shininess = 30.0;

    vec3 ambient = ambient_strength * vec3(1);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * vec3(1);
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specular = specular_strength * spec * vec3(1);

    float cutOff = cos(radians(30.0));
    float outerCutOff = cos(radians(45.0));
    float theta = dot(L, normalize(-light_direction));
    float epsilon = cutOff - outerCutOff;
    float spotFactor = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    vec4 fragPosLightSpace = light_space_projection * light_space_view * vec4(world_position, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace, N, L);

    vec3 lighting = (ambient + (1.0 - shadow) * spotFactor * light_intensity * (diffuse + specular)) * color.rgb;

    out_color = vec4(lighting, 1.0);
}