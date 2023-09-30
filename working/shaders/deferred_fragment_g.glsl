#version 330 core
layout (location = 0) out vec3 position_buffer;
layout (location = 1) out vec3 normal_buffer;
layout (location = 2) out vec4 albedo_buffer;

in vec2 uv;
in vec3 position;
in vec3 normal;

uniform sampler2D texture_albedo;

void main()
{
    position_buffer = position;
    normal_buffer = normal;
    albedo_buffer = texture(texture_albedo, uv);
}