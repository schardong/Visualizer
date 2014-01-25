#version 330

uniform sampler3D u_sDensityMap;
uniform sampler3D u_sVertexToBranchMap;

uniform sampler2D u_sOpacityTransferFunction;
uniform sampler1D u_sColorTransferFunction;

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

vec4 composite(Ray ray, vec3 step)
{
    float len = length(texture(u_sBackFaces, gl_FragCoord.st / u_vScreenSize).xyz - ex_vEntryPoint);
    vec3 currPos = ray.origin;
    vec4 colorAcc = vec4(0.f);
    float lenAcc = 0.f;

    for(int i = 0; i < u_fNumSamples; i++, currPos += step, lenAcc += length(step)) {

        float scalar_val = texture(u_sDensityMap, currPos).r;
        float branch = texture(u_sVertexToBranchMap, currPos).r;

        vec4 color_sample;
        color_sample.a = texture(u_sOpacityTransferFunction, vec2(branch, scalar_val)).r;
        color_sample.rgb = texture(u_sColorTransferFunction, scalar_val).rgb;

        color_sample = abs(color_sample);
        color_sample.a = clamp(color_sample.a, 0.f, 1.f);

        color_sample.rgb *= color_sample.a;
        colorAcc = (1.f - colorAcc.a) * color_sample + colorAcc;

        if(colorAcc.a >= 0.9)
            break;

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
