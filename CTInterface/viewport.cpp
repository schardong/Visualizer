#include "GL3/gl3w.h"
#include "viewport.h"
#include "scene.h"
#include "multidimrenderer.h"

#include <QTimer>
#include <QKeyEvent>
#include <QVector2D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

namespace Ui
{

    Viewport::Viewport(const QGLFormat & format, QWidget *parent) : QGLWidget(format, parent)
    {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));

        scene = NULL;
        rotSpeed = 0;
        mousePressLoc = glm::vec2(0, 0);
        rotAxis = glm::vec3(0, 0, 0);
    }

    Viewport::~Viewport()
    {
        timer->stop();
        delete timer;
        delete scene;
    }

    void Viewport::loadVolume(std::string path)
    {
        timer->stop();
        scene->loadVolume(path);
        timer->start(16);
    }

    void Viewport::loadVertexBranchMap(std::string)
    {
        timer->stop();
        Logger::getInstance()->warn("void Viewport::loadVertexBranchMap is not fully implemented yet!");
        timer->start(16);
    }

    void Viewport::loadOpacityTransferFunction(std::string)
    {
        timer->stop();
        Logger::getInstance()->warn("void Viewport::loadOpacityTransferFunction is not fully implemented yet!");
        timer->start(16);
    }

    void Viewport::loadColorTransferFunction(std::string)
    {
        timer->stop();
        Logger::getInstance()->warn("void Viewport::loadColorTransferFunction is not fully implemented yet!");
        timer->start(16);
    }

    void Viewport::buildContourTree()
    {
        timer->stop();
        Logger::getInstance()->warn("void Viewport::buildContourTree is not fully implemented yet!");
        timer->start(16);
    }

    void Viewport::setNumSamples(float n)
    {
        scene->setNumSamples(n);
    }

    float Viewport::getNumSamples()
    {
        return scene->getNumSamples();
    }

    void Viewport::setRayTransversalMode(MultiDimRenderer::RAY_TRANSVERSAL rtm)
    {
        scene->setRayTransfersalMode(rtm);
    }

    void Viewport::initializeGL()
    {
        makeCurrent();
        bool init_success = ggraf::Kernel::getInstance()->init(format().majorVersion(), format().minorVersion());
        if(!init_success) {
            Logger::getInstance()->error("OpenGL Context creation failed. Exiting now");
            exit(1);
        }

        glEnable(GL_TEXTURE_1D);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_3D);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.f, 0.f, 0.f, 1.f);

        scene = new MultiDimRenderer(width(), height());
        scene->init();
        timer->start(16);
    }

    void Viewport::paintGL()
    {
        scene->update();
        scene->render();
    }

    void Viewport::resizeGL(int w, int h)
    {
        scene->resize(w, h);
    }

    void Viewport::mousePressEvent(QMouseEvent* e)
    {
        if(e->buttons() & Qt::LeftButton) {
            QVector2D tmpMouseLoc = QVector2D(e->pos());
            mousePressLoc = glm::vec2(tmpMouseLoc.x(), tmpMouseLoc.y());
        }
    }

    void Viewport::mouseReleaseEvent(QMouseEvent* e)
    {
        if(e->button() & Qt::LeftButton) {
            QVector2D tmpMouseLoc = QVector2D(e->pos());
            glm::vec2 mouseReleaseLoc = glm::vec2(tmpMouseLoc.x(), tmpMouseLoc.y());

            glm::vec2 mouseDiff = mouseReleaseLoc - mousePressLoc;

            if(mouseDiff[0] == 0 && mouseDiff[1] == 0) {
                rotSpeed = 0;
                rotAxis = glm::vec3(0, 0, 0);
                return;
            }

            rotAxis = glm::normalize(glm::vec3(mouseDiff[1], mouseDiff[0], 0.f));

            rotSpeed = glm::length(mouseDiff) / 10;
            scene->rotateCamera(rotAxis, rotSpeed);
            mousePressLoc = glm::vec2(0, 0);
        }
    }

    void Viewport::wheelEvent(QWheelEvent* e)
    {
        if(e->buttons() & Qt::RightButton) {
            if(e->delta() > 0)
                scene->setFovy(scene->getFovy() - 1);
            else
                scene->setFovy(scene->getFovy() + 1);
        } else {
            if(e->delta() > 0)
                scene->setNumSamples(scene->getNumSamples() + 8);
            else
                scene->setNumSamples(scene->getNumSamples() - 8);
        }
    }

    void Viewport::timerUpdate()
    {
        if(rotSpeed <= 0.5)
            rotSpeed = 0;
        else {
            rotSpeed *= 0.95;
            scene->rotateCamera(rotAxis, rotSpeed);
        }

        updateGL();
    }

}
