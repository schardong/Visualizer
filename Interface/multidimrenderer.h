#ifndef MULTIDIMRENDERER_H
#define MULTIDIMRENDERER_H

#include "kernel.h"
#include "renderer.h"

class MultiDimRenderer : public Renderer
{
public:
    MultiDimRenderer();
    MultiDimRenderer(int, int);

    void init();

    void loadVolume(std::string);
    void loadVertexBranchMap(std::string);
    void loadOpacityTransferFunction(std::string);
    void loadColorTransferFuntcion(std::string);

protected:
    void loadUniforms(MultiDimRenderer::SHADER_PASS);
//    void checkUniforms(MultiDimRenderer::SHADER_PASS);

private:
    ggraf::MultiDimVolumeData* vd;
};

#endif // MULTIDIMRENDERER_H
