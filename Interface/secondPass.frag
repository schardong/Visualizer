#version 330

uniform sampler3D u_sDensityMap;
uniform sampler1D u_sTransferFunction;
uniform sampler2D u_sBackFaces;

uniform vec2 u_vScreenSize;
uniform float u_fNumSamples;

in vec3 ex_vEntryPoint;
in vec4 ex_vExitPoint;

layout(location = 0) out vec4 out_vColor;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

void main(void)
{
    vec3 exitPoint = texture(u_sBackFaces, gl_FragCoord.st / u_vScreenSize).xyz;

    if(ex_vEntryPoint == exitPoint) {
        discard;
    }

    Ray ray = Ray(ex_vEntryPoint, normalize(exitPoint - ex_vEntryPoint));
    float len = length(exitPoint - ex_vEntryPoint);
    vec3 step = ray.direction * sqrt(3.f) / u_fNumSamples;
    vec3 currPos = ray.origin;
    vec4 colorAcc = vec4(0.f);
    float lenAcc = 0.f;

    for(int i = 0; i < u_fNumSamples; i++, currPos += step, lenAcc += length(step)) {
        vec4 colorSample = texture(u_sTransferFunction, texture(u_sDensityMap, currPos).r);

        colorSample = abs(colorSample);
        colorSample.a = clamp(colorSample.a, 0.f, 1.f);

        colorSample.rgb *= colorSample.a;
        out_vColor = (1.f - out_vColor.a) * colorSample + out_vColor;

//        if(length(colorSample.rgb) > length(colorAcc.rgb))
//            colorAcc = colorSample;

        if(out_vColor.a > 0.95f || lenAcc >= len) {
            break;
        }
    }

//    out_vColor = vec4(1.f, 0, 1.f, 0);
//    out_vColor = colorAcc;
}
