#version 330

in vec3 ex_vColor;

layout(location = 0) out vec4 out_vColor;

void main()
{
    out_vColor = vec4(ex_vColor, 1.f);
}
