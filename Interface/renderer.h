#ifndef RENDERER_H
#define RENDERER_H

#include "kernel.h"

class Renderer : public Scene
{
public:    
    typedef enum
    {
        MIP,
        AVG,
        COM
    } RAY_TRANSVERSAL;

    Renderer();
    Renderer(int w, int h);
    virtual ~Renderer();

    virtual void init();
    virtual void destroy();
    virtual void update();
    virtual void render();
    virtual void resize(int, int);

    void rotateCamera(glm::vec3, float);
    void loadVolume(std::string);
    void loadTransferFunction(std::string);
    void setNumSamples(float);
    float getNumSamples();
    void setFovy(float);
    float getFovy();
    void setRayTransfersalMode(Renderer::RAY_TRANSVERSAL);

protected:
    typedef enum
    {
        FIRST,
        SECOND
    } SHADER_PASS;

    GLuint fboId;
    GLuint fboTexId;
    GLuint depthBuffId;
    Renderer::RAY_TRANSVERSAL mode;

    glm::vec4 eye;
    glm::mat4 projMatrix;
    glm::mat4 viewMatrix;

    float fovy;
    int width;
    int height;
    float numSamples;

    ggraf::Shader* fPass;
    ggraf::Shader* sPass;


    virtual void loadUniforms(Renderer::SHADER_PASS);
//    virtual void checkUniforms(Renderer::SHADER_PASS);

private:
    ggraf::VolumeData* vd;

};

#endif // RENDERER_H
