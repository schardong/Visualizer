#ifndef RENDERER_H
#define RENDERER_H

#include "kernel.h"

class Renderer : public Scene
{
private:

    typedef enum
    {
        FIRST,
        SECOND
    } SHADER_PASS;

    GLuint fboId;
    GLuint fboTexId;
    GLuint depthBuffId;

    glm::vec4 eye;
    glm::mat4 projMatrix;
    glm::mat4 viewMatrix;

    float fovy;
    int width;
    int height;
    float numSamples;

    ggraf::Shader* fPass;
    ggraf::Shader* sPass;
    ggraf::VolumeData* vd;

    void loadUniforms(Renderer::SHADER_PASS);
    void checkUniforms(Renderer::SHADER_PASS);

public:
    Renderer(int w, int h);
    virtual ~Renderer();

    void init();
    void destroy();
    void update();
    void render();
    void resize(int, int);
    void rotateCamera(glm::vec3, float);
    void loadVolume(std::string);
    void loadTransferFunction(std::string);
    void setNumSamples(float);
    float getNumSamples();
    void setFovy(float);
    float getFovy();
};

#endif // RENDERER_H
