#ifndef RENDERER_H
#define RENDERER_H

#include "viskernel.h"

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

    Shader* fPass;
    Shader* sPass;
    VolumeData* vd;

    void loadUniforms(Renderer::SHADER_PASS);
    void checkUniforms(Renderer::SHADER_PASS);

public:
    Renderer(int w, int h);
    virtual ~Renderer();

    void init();
    void destroy();
    void update(float);
    void render();
    void resize(int, int);
    void rotateCamera(glm::vec2, float);
    void moveCamera(float);
    void loadVolume(std::string);
    void loadTransferFunction(std::string);
    void setNumSamples(float);
    float getNumSamples();
};

#endif // RENDERER_H
