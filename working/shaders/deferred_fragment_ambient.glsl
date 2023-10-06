#version 330 core
out vec4 FragColor;

in vec2 screen_uv;

uniform vec3 ambient_color;

uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D albedo_buffer;
uniform sampler2D blue_noise;

uniform mat4 view;
uniform mat4 projection;


void main()
{
    //todo ssao

    FragColor = texture(albedo_buffer,screen_uv)*vec4(ambient_color,1.0f);
}