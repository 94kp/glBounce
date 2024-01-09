#version 450 core

in vec4 a_position;
uniform mat4 u_modelMatrix_sphere;
uniform mat4 u_viewMatrix_sphere;
uniform mat4 u_projectionMatrix_sphere;
void main(void)
{
    gl_Position = u_projectionMatrix_sphere * u_viewMatrix_sphere * u_modelMatrix_sphere * a_position;
}
