#ifndef MULTIDIMRENDERER_H
#define MULTIDIMRENDERER_H

#include "kernel.h"

class MultiDimRenderer
{
public:
    typedef enum
    {
        MIP,
        AVG,
        COM
    } RAY_TRANSVERSAL;

    MultiDimRenderer();
    MultiDimRenderer(int, int);
    virtual ~MultiDimRenderer();

    virtual void init();
    virtual void destroy();
    virtual void update();
    virtual void render();
    virtual void resize(int, int);

    void loadVolume(std::string);
    void loadVertexBranchMap(std::string);
    void loadOpacityTransferFunction(std::string);
    void loadColorTransferFuntcion(std::string);

    void rotateCamera(glm::vec3, float);
    void setNumSamples(float);
    float getNumSamples();
    void setFovy(float);
    float getFovy();
    void setRayTransfersalMode(MultiDimRenderer::RAY_TRANSVERSAL);

protected:
    typedef enum
    {
        FIRST,
        SECOND
    } SHADER_PASS;

    GLuint fboId;
    GLuint fboTexId;
    GLuint depthBuffId;
    MultiDimRenderer::RAY_TRANSVERSAL mode;

    glm::vec4 eye;
    glm::mat4 projMatrix;
    glm::mat4 viewMatrix;

    float fovy;
    int width;
    int height;
    float numSamples;

    ggraf::Shader* fPass;
    ggraf::Shader* sPass;

    void loadUniforms(MultiDimRenderer::SHADER_PASS);

private:
    ggraf::MultiDimVolumeData* vd;

    virtual inline bool checkDataTypes()
    {
        return ((vd->getDataTypes()[0] == vd->getDataTypes()[2]) == vd->getDataTypes()[3]);
    }
};

#endif // MULTIDIMRENDERER_H
