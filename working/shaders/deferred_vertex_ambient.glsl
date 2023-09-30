#version 330 core
//see https://stackoverflow.com/questions/2588875/whats-the-best-way-to-draw-a-fullscreen-quad-in-opengl-3-2/51625078#51625078
out vec2 screen_uv;
void main() {
    vec2 vertices[3]=vec2[3](vec2(-1,-1), vec2(3,-1), vec2(-1, 3));
    gl_Position = vec4(vertices[gl_VertexID],0,1);
    screen_uv = 0.5 * gl_Position.xy + vec2(0.5);
}