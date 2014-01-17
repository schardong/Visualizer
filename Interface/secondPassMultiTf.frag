#version 330

uniform sampler3D u_sDensityMap;
uniform sampler3D u_sVertexToBranchMap;
uniform sampler2D u_sTransferFunction;
uniform sampler2D u_sBackFaces;

uniform vec2 u_vScreenSize;
uniform float u_fNumSamples;
uniform int rayTMode;

in vec3 ex_vEntryPoint;
in vec4 ex_vExitPoint;

layout(location = 0) out vec4 out_vColor;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec4 composite(Ray ray, vec3 step)
{
    float len = length(texture(u_sBackFaces, gl_FragCoord.st / u_vScreenSize).xyz - ex_vEntryPoint);
    vec3 currPos = ray.origin;
    vec4 colorAcc = vec4(0.f);
    float lenAcc = 0.f;

    for(int i = 0; i < u_fNumSamples; i++, currPos += step, lenAcc += length(step)) {
        vec4 colorSample = texture(u_sTransferFunction, texture(u_sDensityMap, currPos).r);

        colorSample = abs(colorSample);
        colorSample.a = clamp(colorSample.a, 0.f, 1.f);

        colorSample.rgb *= colorSample.a;
        colorAcc = (1.f - colorAcc.a) * colorSample + colorAcc;

        if(colorAcc.a > 0.95f || lenAcc >= len) {
            break;
        }
    }

    return colorAcc;
}

void main(void)
{
    vec3 exitPoint = texture(u_sBackFaces, gl_FragCoord.st / u_vScreenSize).xyz;

    if(ex_vEntryPoint == exitPoint) {
        discard;
    }

    Ray ray = Ray(ex_vEntryPoint, normalize(exitPoint - ex_vEntryPoint));
    vec3 step = ray.direction * sqrt(3.f) / u_fNumSamples;
    out_vColor = composite(ray, step);
}
