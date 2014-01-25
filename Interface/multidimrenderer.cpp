#include "multidimrenderer.h"

MultiDimRenderer::MultiDimRenderer()
{
    vd = new ggraf::MultiDimVolumeData("/home/guilherme/Pictures/datasets/nucleon.41x41x41.uint8",
                                       "/home/guilherme/Pictures/datasets/vertex-branch-maps/nucleon.41x41x41.uint8",
                                       "/home/guilherme/Pictures/datasets/transfer-functions/nucleon.6.uint8",
                                       "/home/guilherme/Pictures/datasets/transfer-functions/color_tf1.uint8");
}

MultiDimRenderer::MultiDimRenderer(int w, int h) : Renderer(w, h)
{
    vd = new ggraf::MultiDimVolumeData("/home/guilherme/Pictures/datasets/nucleon.41x41x41.uint8",
                                       "/home/guilherme/Pictures/datasets/vertex-branch-maps/nucleon.41x41x41.uint8",
                                       "/home/guilherme/Pictures/datasets/transfer-functions/nucleon.6.uint8",
                                       "/home/guilherme/Pictures/datasets/transfer-functions/color_tf1.uint8");
}

void MultiDimRenderer::init()
{
    Logger::getInstance()->log("void MultiDimRenderer::init()");
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
    sPass = new ggraf::Shader("secondPass.vert", "secondPassMultiTf.frag");
    sPass->bind();
    sPass->bindFragDataLoc("out_vColor", 0);
    ggraf::Shader::unbind();
}

void MultiDimRenderer::loadVertexBranchMap(std::string path)
{
    Logger::getInstance()->log("MultiDimRenderer::loadVertexBranchMap(" + path + ")");

    if(path.empty()) {
        Logger::getInstance()->error("Invalid path provided. Vertex to branch map not loaded.");
        return;
    }

    vd->loadVertexToBranchMap(path);
}

void MultiDimRenderer::loadOpacityTransferFunction(std::string path)
{
    Logger::getInstance()->log("MultiDimRenderer::loadOpacityTransferFunction(" + path + ")");

    if(path.empty()) {
        Logger::getInstance()->error("Invalid path provided. Opacity transfer function map not loaded.");
        return;
    }

    vd->loadOpacityTransferFunction(path);
}

void MultiDimRenderer::loadColorTransferFuntcion(std::string path)
{
    Logger::getInstance()->log("MultiDimRenderer::loadColorTransferFuntcion(" + path + ")");

    if(path.empty()) {
        Logger::getInstance()->error("Invalid path provided. Color transfer function map not loaded.");
        return;
    }

    vd->loadColorTransferFunction(path);
}

void MultiDimRenderer::loadUniforms(MultiDimRenderer::SHADER_PASS p)
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
            glBindTexture(GL_TEXTURE_3D, vd->getVertexBranchTexId());
            s->setUniform1i("u_sVertexToBranchMap", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, vd->getOpacityTransferFuncTexId());
            s->setUniform1i("u_sOpacityTransferFunction", 2);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_1D, vd->getColorTransferFuncTexId());
            s->setUniform1i("u_sColorTransferFunction", 3);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, fboTexId);
            s->setUniform1i("u_sBackFaces", 4);

            s->setUniformfv("u_vScreenSize", screenSize, 2);
            s->setUniform1f("u_fNumSamples", numSamples);
//            s->setUniform1i("rayTMode", mode);
        }

        s->setUniformMatrix("u_mProjectionMatrix", projMatrix);
        s->setUniformMatrix("u_mViewMatrix", viewMatrix);
        s->setUniformMatrix("u_mModelMatrix", vd->getModelMatrix());
    }
}

//void MultiDimRenderer::checkUniforms(MultiDimRenderer::SHADER_PASS pass)
//{

//}
