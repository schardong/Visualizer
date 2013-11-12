#include "GL3/gl3w.h"
#include "viewport.h"
#include "scene.h"
#include "renderer.h"

#include <QTimer>
#include <QKeyEvent>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

namespace Ui
{

    Viewport::Viewport(const QGLFormat & format, QWidget *parent) : QGLWidget(format, parent)
    {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));

        scene = NULL;
        rotSpeed = 0;
        mousePressLoc = glm::vec2(0, 0);
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

    void Viewport::loadTransferFuncion(std::string path)
    {
        timer->stop();
        scene->loadTransferFunction(path);
        timer->start(16);
    }

    void Viewport::initializeGL()
    {
        makeCurrent();
        if(gl3wInit()) {
            cerr << "Failed to initialize OpenGL." << endl;
            return;
        }

        if(!gl3wIsSupported(format().majorVersion(), format().minorVersion())) {
            cerr << "OpenGL " << format().majorVersion() << "." << format().minorVersion() << " is not supported." << endl;
            return;
        }
        cout << "\nOpenGL " << glGetString(GL_VERSION) << "\nGLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

        glEnable(GL_TEXTURE_1D);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_3D);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.1f, 0.1f, 0.f, 1.f);

        scene = new Renderer(width(), height());
        scene->init();
        timer->start(16);
    }

    void Viewport::paintGL()
    {
        scene->render();
    }

    void Viewport::resizeGL(int w, int h)
    {
        scene->resize(w, h);
    }

    void Viewport::mousePressEvent(QMouseEvent* e)
    {
        QVector2D tmpMouseLoc = QVector2D(e->localPos());
        mousePressLoc = glm::vec2(tmpMouseLoc.x(), tmpMouseLoc.y());
    }

    void Viewport::mouseReleaseEvent(QMouseEvent* e)
    {
        QVector2D tmpMouseLoc = QVector2D(e->localPos());
        glm::vec2 mouseReleaseLoc = glm::vec2(tmpMouseLoc.x(), tmpMouseLoc.y());

        glm::vec2 mouseDiff = mouseReleaseLoc - mousePressLoc;

        glm::vec2 axis = glm::normalize(glm::vec2(mouseDiff[1], mouseDiff[0]));

        rotSpeed = glm::length(mouseDiff) / 10;
        cout << rotSpeed << endl;
        scene->rotateCamera(axis, rotSpeed);
    }

    void Viewport::timerUpdate()
    {
        if(rotSpeed > 0.5)
            rotSpeed = 0;
        else
            rotSpeed *= 0.99;
        updateGL();
    }

}
