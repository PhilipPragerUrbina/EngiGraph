#version 330 core
out vec4 output_color;

uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 camera_position;

uniform float light_linear;
uniform float light_quadratic;
uniform float light_constant;


uniform sampler2D position_buffer;
uniform sampler2D normal_buffer;
uniform sampler2D albedo_buffer;
uniform vec2 screen_size;

void main()
{
    vec2 screen_uv = gl_FragCoord.xy/screen_size;
    vec3 world_fragment_position = texture(position_buffer, screen_uv).xyz;
    vec3 world_normal = texture(normal_buffer, screen_uv).xyz;
    vec3 albedo = texture(albedo_buffer, screen_uv).rgb;

    vec3 view_direction = normalize(camera_position - world_fragment_position);
    vec3 light_direction = normalize(light_position - world_fragment_position);

    float distance = length(light_position - world_fragment_position);
    float attenuation = 1.0 / (light_constant + light_linear * distance +
    light_quadratic * (distance * distance));
    output_color = vec4(max(dot(world_normal, light_direction), 0.0) * albedo * light_color * attenuation, 1.0);
}