#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform vec3 light_position;
uniform float light_scale_factor;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4((vertex_position*light_scale_factor)+light_position, 1.0f);
}