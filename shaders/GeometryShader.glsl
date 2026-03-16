#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 85) out;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_instances;
uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;
uniform int surface_type;

in int instance[2];

out vec3 world_position;
out vec3 world_normal;
out vec2 text_coord;

vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z * sin(u);
    float z = point.x * sin(u) + point.z * cos(u);
    return vec3(x, point.y, z);
}

vec3 translateX(vec3 point, float t)
{
    return vec3(point.x + t, point.y, point.z);
}

vec3 bezier(float t)
{
    return control_p0 * pow((1 - t), 3) +
           control_p1 * 3 * t * pow((1 - t), 2) +
           control_p2 * 3 * pow(t, 2) * (1 - t) +
           control_p3 * pow(t, 3);
}

void main() {
    const int SURFACE_TYPE_ROTATION = 0;
    const int SURFACE_TYPE_TRANSLATION = 1;

    int i0 = instance[0];
    int i1 = instance[0] + 1;

    float delta_translate = max_translate / float(no_of_instances);
    float delta_rotate = max_rotate / float(no_of_instances);

    float t_val_0 = (surface_type == SURFACE_TYPE_TRANSLATION) ? float(i0) * delta_translate : float(i0) * delta_rotate;
    float t_val_1 = (surface_type == SURFACE_TYPE_TRANSLATION) ? float(i1) * delta_translate : float(i1) * delta_rotate;

    float u_tex_0 = float(i0) / float(no_of_instances);
    float u_tex_1 = float(i1) / float(no_of_instances);

    if (surface_type == SURFACE_TYPE_ROTATION) {
        vec3 p_start = bezier(0.0);
        
        if (abs(p_start.x) > 0.001) {
            
            vec3 center = vec3(0, p_start.y, 0);
            vec3 p_ring0 = rotateY(p_start, t_val_0);
            vec3 p_ring1 = rotateY(p_start, t_val_1);

            mat3 normalMatrix = mat3(transpose(inverse(Model)));
            vec3 downNormal = normalize(normalMatrix * vec3(0, -1, 0));

            world_position = (Model * vec4(center, 1)).xyz;
            world_normal = downNormal;
            text_coord = vec2(0.5, 0.5);
            gl_Position = Projection * View * vec4(world_position, 1.0);
            EmitVertex();

            world_position = (Model * vec4(p_ring1, 1)).xyz;
            world_normal = downNormal;
            text_coord = vec2(u_tex_1, 0.0);
            gl_Position = Projection * View * vec4(world_position, 1.0);
            EmitVertex();

            world_position = (Model * vec4(p_ring0, 1)).xyz;
            world_normal = downNormal;
            text_coord = vec2(u_tex_0, 0.0);
            gl_Position = Projection * View * vec4(world_position, 1.0);
            EmitVertex();

            EndPrimitive();
        }
    }

    mat3 normalMatrix = mat3(transpose(inverse(Model)));

    for (int i = 0; i < no_of_generated_points - 1; i++) {

        float t_curr = float(i) / float(no_of_generated_points - 1);
        float t_next = float(i + 1) / float(no_of_generated_points - 1);

        vec3 p_curr_local = bezier(t_curr);
        vec3 p_next_local = bezier(t_next);

        vec3 p0, p1, p2; 

        if (surface_type == SURFACE_TYPE_TRANSLATION) {
            p0 = translateX(p_curr_local, t_val_0);
            p1 = translateX(p_curr_local, t_val_1);
            p2 = translateX(p_next_local, t_val_0);
        } else {
            p0 = rotateY(p_curr_local, t_val_0);
            p1 = rotateY(p_curr_local, t_val_1);
            p2 = rotateY(p_next_local, t_val_0);
        }

        vec3 tangentV = normalize(p2 - p0);
        vec3 tangentU = normalize(p1 - p0);
        vec3 localNormal = normalize(cross(tangentV, tangentU));
        vec3 finalNormal = normalize(normalMatrix * localNormal);

        world_position = (Model * vec4(p0, 1)).xyz;
        world_normal = finalNormal;
        text_coord = vec2(u_tex_0, t_curr);
        gl_Position = Projection * View * vec4(world_position, 1.0);
        EmitVertex();

        vec3 p1_pos;
        if (surface_type == SURFACE_TYPE_TRANSLATION) 
            p1_pos = translateX(p_curr_local, t_val_1);
        else 
            p1_pos = rotateY(p_curr_local, t_val_1);
        
        world_position = (Model * vec4(p1_pos, 1)).xyz;
        world_normal = finalNormal; 
        text_coord = vec2(u_tex_1, t_curr);
        gl_Position = Projection * View * vec4(world_position, 1.0);
        EmitVertex();
    }

    EndPrimitive();
}