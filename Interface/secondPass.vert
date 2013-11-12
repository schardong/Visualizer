#version 330

layout(location = 0) in vec3 in_vPosition;
layout(location = 1) in vec3 in_vColor;

out vec3 ex_vEntryPoint;
out vec4 ex_vExitPoint;

uniform mat4 u_mProjectionMatrix;
uniform mat4 u_mViewMatrix;
uniform mat4 u_mModelMatrix;

void main(void)
{
    mat4 MVP = u_mProjectionMatrix * u_mViewMatrix * u_mModelMatrix;
    gl_Position = MVP * vec4(in_vPosition, 1.f);
    ex_vEntryPoint = in_vColor;
    ex_vExitPoint = gl_Position;
}
