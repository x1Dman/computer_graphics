#version 330 core

in vec4 position;
out vec4 v_color;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transform * position;
    v_color = position;
}
