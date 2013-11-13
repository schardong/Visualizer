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

Renderer::Renderer(int w, int h)
    : width(w),
      height(h)
{
    eye = glm::vec4(0, 0, 1.5, 1);
    projMatrix = glm::perspective(fovy, static_cast<float>(w) / static_cast<float>(h), 0.1f, 10.f);
    viewMatrix = glm::lookAt(glm::vec3(eye), glm::vec3(0, 0, -0.1), glm::vec3(0, 1, 0));
    fboId = fboTexId = depthBuffId = 0;
    fovy = 45;
    numSamples = 256.f;
    vd = new ggraf::VolumeData();
}

Renderer::~Renderer()
{
    delete vd;

    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &fboTexId);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glDeleteRenderbuffers(1, &depthBuffId);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fboId);

    eye = glm::vec4(0, 0, 0, 0);
    projMatrix = glm::mat4();
    viewMatrix = glm::mat4();

    Shader::unbind();
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
        cerr << "FBO undefined" << endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :
        cerr << "FBO incomplete attachment" << endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
        cerr << "FBO missing attachment" << endl;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER :
        cerr << "FBO incomplete draw buffer" << endl;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED :
        cerr << "FBO unsupported" << endl;
        break;
    case GL_FRAMEBUFFER_COMPLETE:
        cout << "FBO created successfully" << endl;
        break;
    default:
        cerr << "FBO generic problem" << endl;
    }

    GLenum drawBuffer[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    fPass = new Shader("firstPass.vert", "firstPass.frag");
    fPass->bind();
    loadUniforms(FIRST);

    sPass = new Shader("secondPass.vert", "secondPass.frag");
    sPass->bind();
    loadUniforms(SECOND);

    Shader::unbind();
}

void Renderer::destroy() {}

void Renderer::update(float)
{
}

void Renderer::render()
{
    fPass->bind();
    loadUniforms(FIRST);

    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    vd->render();
    glDisable(GL_CULL_FACE);
    Shader::unbind();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    sPass->bind();
    loadUniforms(SECOND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    vd->render();
    glDisable(GL_CULL_FACE);
    Shader::unbind();
}

void Renderer::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    projMatrix = glm::perspective(fovy, static_cast<float>(w) / static_cast<float>(h), 0.1f, 10.f);
}

void Renderer::rotateCamera(glm::vec2 axis, float speed)
{
    viewMatrix = glm::rotate(viewMatrix, speed, glm::vec3(axis, 0.f));
}

void Renderer::moveCamera(float distance)
{
    cout << "Renderer::moveCamera -> NOT IMPLEMENTED" << endl;
}

void Renderer::loadUniforms(Renderer::SHADER_PASS p)
{
    Shader* s;
    if(vd->isVolumeLoaded() && vd->isTfLoaded()) {
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
        }

        s->setUniformMatrix("u_mProjectionMatrix", projMatrix);
        s->setUniformMatrix("u_mViewMatrix", viewMatrix);
        s->setUniformMatrix("u_mModelMatrix", vd->getModelMatrix());
    }
}

void Renderer::checkUniforms(Renderer::SHADER_PASS p)
{
    Shader* s;
    if(vd->isVolumeLoaded() && vd->isTfLoaded()) {
        float* pMat;
        float* vMat;
        float* mMat;

        if(p == FIRST) {
            s = fPass;
        } else if(p == SECOND) {
            s = sPass;

            float* screenSize;
            int* tex0;
            int* tex1;
            int* tex2;
            float* samples;

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_3D, vd->getVolumeTexId());
            tex0 = s->getUniformiv("u_sDensityMap", 1);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_1D, vd->getTransferFuncTexId());
            tex1 = s->getUniformiv("u_sTransferFunction", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, fboTexId);
            tex2 = s->getUniformiv("u_sBackFaces", 1);

            screenSize = s->getUniformfv("u_vScreenSize", 2);
            samples = s->getUniformfv("u_fNumSamples", 1);

            cout << "densityMap = " << *tex0 << endl;
            cout << "tf = " << *tex1 << endl;
            cout << "backfaces = " << *tex2 << endl;
            cout << "screenSize = " << screenSize[0] << " x " << screenSize[1] << endl;
            cout << "numSamples = " << *samples << endl;
        }

        pMat = s->getUniformfv("u_mProjectionMatrix", 16);
        vMat = s->getUniformfv("u_mViewMatrix", 16);
        mMat = s->getUniformfv("u_mModelMatrix", 16);

        for(int i = 0; i < 16; i++) {
            if(i % 4 == 0)
                cout << endl;
            cout << pMat[i] << " ";
        }
        cout << endl;

        for(int i = 0; i < 16; i++) {
            if(i % 4 == 0)
                cout << endl;
            cout << vMat[i] << " ";
        }
        cout << endl;

        for(int i = 0; i < 16; i++) {
            if(i % 4 == 0)
                cout << endl;
            cout << mMat[i] << " ";
        }
        cout << endl;
    }
}

void Renderer::loadVolume(std::string path)
{
    int w, h, s;
    size_t bytes;

    if(path.empty()) {
        cerr << "Renderer::loadVolume -> ERROR: Invalid path provided. Volume not loaded." << endl;
        return;
    }

    std::vector<std::string> strs;
    boost::algorithm::split(strs, path, boost::is_any_of("."));

    if(strs[2] == "uint8")
        bytes = 1;
    else
        bytes = 2;

    std::vector<std::string> dims_str;
    boost::algorithm::split(dims_str, strs[1], boost::is_any_of("x"));

    std::istringstream(dims_str[0]) >> w;
    std::istringstream(dims_str[1]) >> h;
    std::istringstream(dims_str[2]) >> s;

    vd->loadVolume(path, w, h, s, bytes);

    fPass = new Shader("firstPass.vert", "firstPass.frag");
    fPass->bind();
    loadUniforms(FIRST);
    checkUniforms(FIRST);

    sPass = new Shader("secondPass.vert", "secondPass.frag");
    sPass->bind();
    loadUniforms(SECOND);
    checkUniforms(SECOND);

    Shader::unbind();
}

void Renderer::loadTransferFunction(std::string path)
{
    int bytes;

    if(path.empty()) {
        cerr << "Renderer::loadTransferFunction -> ERROR: Invalid path provided. Transfer function not loaded." << endl;
        return;
    }

    std::vector<std::string> strs;
    boost::algorithm::split(strs, path, boost::is_any_of("."));

    if(strs.back() == "uint8")
        bytes = 1;
    else
        bytes = 2;

    vd->loadTransferFunction(path, bytes);

    fPass = new Shader("firstPass.vert", "firstPass.frag");
    fPass->bind();
    loadUniforms(FIRST);

    sPass = new Shader("secondPass.vert", "secondPass.frag");
    sPass->bind();
    loadUniforms(SECOND);

    Shader::unbind();
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
