#version 450 core

in vec2 a_texcoord;
in vec4 a_position;

uniform mat4 u_modelMatrix_ground;
uniform mat4 u_viewMatrix_ground;
uniform mat4 u_projectionMatrix_ground;
out vec2 a_texcoord_out;
void main(void)
{
    gl_Position = u_projectionMatrix_ground * u_viewMatrix_ground * u_modelMatrix_ground * a_position;
    a_texcoord_out = a_texcoord;
}
