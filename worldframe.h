#ifndef WORLDFRAME_H
#define WORLDFRAME_H

#include <QGLWidget>
#include <QMutex>

#include "worldrenderthread.h"
#include "object.h"

class WorldFrame : public QGLWidget
{
    Q_OBJECT
public:
    explicit WorldFrame(QWidget *parent = 0);
    ~WorldFrame();

    void initSingleObj();
    void initEarthMoon();
    void initParticles(unsigned int count, double rho, double r);
    
    void initRenderThread(void);
    void stopRenderThread(void);

    void updateWorld();
    void printWorldInfo();

    void resetPos();
    void moveRight();
    void moveLeft();
    void moveDown();
    void moveUp();
    void zoomOut();
    void zoomIn();
    void scaleUp();
    void scaleDown();
    void speedUp();
    void speedDown();

    void glInit();
    void glResize();
    void glPaint();

    int getSimUpdateCountPerFrame()const;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void closeEvent(QCloseEvent *evt);

    float getMoveSize();
private:

    WorldRenderThread mRenderThread;

    QSize mFrameSize;
    long mFrameCounter;

    float mX,mY,mZ;
    int mSimUpdateCountPerFrame;
    double mScale;

    void gluPerspective(double fovy,double aspect, double zNear, double zFar);

    QMutex mObjectsMutex;
    QVector<Object*> mObjects;

    void setForces(Object* obj1,Object* obj2);
};

#endif // WORLDFRAME_H
