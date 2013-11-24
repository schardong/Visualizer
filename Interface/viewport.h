#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "kernel.h"
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
        glm::vec3 rotAxis;
        float rotSpeed;

    public:
        explicit Viewport(const QGLFormat & format, QWidget *parent);
        ~Viewport();

        void loadVolume(std::string);
        void loadTransferFuncion(std::string);
        void setNumSamples(float);
        float getNumSamples();

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
