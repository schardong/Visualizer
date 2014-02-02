#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>
#include <cstdio>
#include <sstream>
#include <QFileDialog>

using std::cout;
using std::cerr;
using std::endl;

Renderer::Renderer()
    : width(600),
      height(600)
{
    eye = glm::vec4(0, 0, 1.5, 1);
    projMatrix = glm::perspective(fovy, static_cast<float>(width) / static_cast<float>(height), 0.1f, 10.f);
    viewMatrix = glm::lookAt(glm::vec3(eye), glm::vec3(0, 0, -0.1), glm::vec3(0, 1, 0));
    fboId = fboTexId = depthBuffId = 0;
    fovy = 45;
    numSamples = 256.f;
    mode = COM;
}

Renderer::Renderer(int w, int h)
    : width(w),
      height(h)
{
    eye = glm::vec4(0, 0, 1.5, 1);
    projMatrix = glm::perspective(fovy, static_cast<float>(width) / static_cast<float>(height), 0.1f, 10.f);
    viewMatrix = glm::lookAt(glm::vec3(eye), glm::vec3(0, 0, -0.1), glm::vec3(0, 1, 0));
    fboId = fboTexId = depthBuffId = 0;
    fovy = 45;
    numSamples = 256.f;
    mode = COM;
}

Renderer::~Renderer()
{
    if(vd != NULL) {
        delete vd;
        vd = NULL;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &fboTexId);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glDeleteRenderbuffers(1, &depthBuffId);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fboId);

    eye = glm::vec4(0, 0, 0, 0);
    projMatrix = glm::mat4();
    viewMatrix = glm::mat4();

    ggraf::Shader::unbind();
    delete fPass;
    delete sPass;
}

void Renderer::init()
{
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    glGenTextures(1, &fboTexId);

    glBindTexture(GL_TEXTURE_2D, fboTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTexId, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &depthBuffId);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffId);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(fboStatus) {
    case GL_FRAMEBUFFER_UNDEFINED:
        Logger::getInstance()->error("FBO undefined");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        Logger::getInstance()->error("FBO incomplete attachment");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        Logger::getInstance()->error("FBO missing attachment");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        Logger::getInstance()->error("FBO incomplete draw buffe");
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        Logger::getInstance()->error("FBO unsupported");
        break;
    case GL_FRAMEBUFFER_COMPLETE:
        Logger::getInstance()->log("FBO created successfully");
        break;
    default:
        Logger::getInstance()->error("FBO undefined problem");
    }

    GLenum drawBuffer[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    fPass = new ggraf::Shader("firstPass.vert", "firstPass.frag");
    sPass = new ggraf::Shader("secondPass.vert", "secondPass.frag");
    sPass->bind();
    sPass->bindFragDataLoc("out_vColor", 0);
    ggraf::Shader::unbind();

    vd = new ggraf::VolumeData();
}

void Renderer::destroy() {}

void Renderer::update()
{
    fPass->bind();
    loadUniforms(FIRST);

    sPass->bind();
    loadUniforms(SECOND);

    ggraf::Shader::unbind();
}

void Renderer::render()
{
    if(mode == RAY_TRANSVERSAL::COM && checkDataTypes()
            || mode == RAY_TRANSVERSAL::AVG
            || mode == RAY_TRANSVERSAL::MIP) {
        fPass->bind();
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        vd->render();
        glDisable(GL_CULL_FACE);
        ggraf::Shader::unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);

        sPass->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        vd->render();
        glDisable(GL_CULL_FACE);
        ggraf::Shader::unbind();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void Renderer::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    projMatrix = glm::perspective(fovy, static_cast<float>(w) / static_cast<float>(h), 0.1f, 10.f);
}

void Renderer::rotateCamera(glm::vec3 axis, float speed)
{
    viewMatrix = glm::rotate(viewMatrix, speed, axis);
}

void Renderer::loadUniforms(Renderer::SHADER_PASS p)
{
    if(vd->isVolumeLoaded() && vd->isTfLoaded()) {
        ggraf::Shader* s;
        if(p == FIRST) {
            s = fPass;
        } else if(p == SECOND) {
            s = sPass;
            float screenSize[] = {static_cast<float>(width), static_cast<float>(height)};

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_3D, vd->getVolumeTexId());
            s->setUniform1i("u_sDensityMap", 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_1D, vd->getTransferFuncTexId());
            s->setUniform1i("u_sTransferFunction", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, fboTexId);
            s->setUniform1i("u_sBackFaces", 2);

            s->setUniformfv("u_vScreenSize", screenSize, 2);
            s->setUniform1f("u_fNumSamples", numSamples);
            s->setUniform1i("rayTMode", mode);
        }

        s->setUniformMatrix("u_mProjectionMatrix", projMatrix);
        s->setUniformMatrix("u_mViewMatrix", viewMatrix);
        s->setUniformMatrix("u_mModelMatrix", vd->getModelMatrix());
    }
}

void Renderer::loadVolume(std::string path)
{
    Logger::getInstance()->log("Renderer::loadVolume(" + path + ")");

    if(path.empty()) {
        Logger::getInstance()->error("Invalid path provided. Volume not loaded.");
        return;
    }

    vd->loadVolume(path);
}

void Renderer::loadTransferFunction(std::string path)
{
    Logger::getInstance()->log("Renderer::loadTransferFunction(" + path + ")");

    if(path.empty()) {
        Logger::getInstance()->error("Invalid path provided. Transfer function not loaded.");
        return;
    }

    vd->loadTransferFunction(path);
}

void Renderer::setNumSamples(float n)
{
    if(n > 0.f)
        numSamples = n;
}

float Renderer::getNumSamples()
{
    return numSamples;
}

void Renderer::setFovy(float n)
{
    if(n > 0.f && fovy < 85.f) {
        fovy = n;
        projMatrix = glm::perspective(fovy, static_cast<float>(width) / static_cast<float>(height), 0.1f, 10.f);
    }
}

float Renderer::getFovy()
{
    return fovy;
}

void Renderer::setRayTransfersalMode(Renderer::RAY_TRANSVERSAL rtm)
{
    mode = rtm;
}
