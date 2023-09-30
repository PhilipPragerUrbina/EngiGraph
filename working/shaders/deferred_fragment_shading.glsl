#version 330 core
out vec4 FragColor;

in vec2 screen_uv;

uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D albedo_buffer;

void main()
{
    FragColor = texture(albedo_buffer,screen_uv);
}