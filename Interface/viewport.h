#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "GL3/gl3w.h"
#include "scene.h"
#include "renderer.h"
#include <QGLWidget>

namespace Ui
{

    class Viewport : public QGLWidget
    {
        Q_OBJECT
    private:
        QTimer* timer;
        Renderer* scene;

        glm::vec2 mousePressLoc;
        glm::vec2 rotAxis;
        float rotSpeed;

    public:
        explicit Viewport(const QGLFormat & format, QWidget *parent);
        ~Viewport();

        void loadVolume(std::string);
        void loadTransferFuncion(std::string);

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int, int);

        void mousePressEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void wheelEvent(QWheelEvent *);

    public slots:
        void timerUpdate();

    };

}

#endif // VIEWPORT_H
