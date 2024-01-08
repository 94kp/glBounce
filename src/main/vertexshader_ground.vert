#version 330 core

in vec4 a_position;
uniform mat4 u_modelMatrix_ground;
uniform mat4 u_viewMatrix_ground;
uniform mat4 u_projectionMatrix_ground;
void main(void)
{
    gl_Position = u_projectionMatrix_ground * u_viewMatrix_ground * u_modelMatrix_ground * a_position;
}
