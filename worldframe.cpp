#include <QResizeEvent>
#include <QCloseEvent>
#include <QDebug>
#include <QMutexLocker>

#include <limits>

#include "worldframe.h"
#include "sphere.h"
#include "worldrenderthread.h"

#define SCALE_STEP 2
#define SCALE_START 0.000000005

#define ZOOM_STEP 0.05f
#define MOVE_STEP (0.005/SCALE_START)
#define MOVE_STEP_MAX 0.05

//m pontossággal számol. Egy frissítés maximum ennyivel mozdítja el a leggyorsabban mozgó testet.
#define MAX_SIMULATED_DISTANCE 10

#define TIMESTEP_STEP 0.1
#define MAX_UPDATE_COUNT 1000000
#define UPDATE_COUNT_FACTOR 10

#define PHYSIC_G (6.674E-11)

#define EARTH_DENSITY 5515.0 //kg/m3
#define EARTH_RADIUS 6371000.0 //m
#define EARTH_ORBITAL_SPEED 0//29780.0 //m/s

#define MOON_DENSITY 3346.4 //kg/m3
#define MOON_RADIUS 1737100.0 //m
#define MOON_ORBITAL_SPEED 1022.0 //m/s

#define EART_MOON_DISTANCE 384399000.0 //m

#define ISS_FLY_HEIGHT 460000.0 //km
#define ISS_ORBITAL_SPEED 7706.6 //m


WorldFrame::WorldFrame(QWidget *parent) :
    QGLWidget(parent),
    mRenderThread(WorldRenderThread(this))
{
    setAutoBufferSwap(false);
    mFrameCounter=0;

    resetPos();

    mSimUpdateCountPerFrame = 1;
    mScale = SCALE_START;

    setFocusPolicy(Qt::StrongFocus);

    //initSingleObj();
    initEarthMoon();
    //initParticles(10,EARTH_DENSITY,0.01);
}

WorldFrame::~WorldFrame()
{
    for (int i = 0; i < mObjects.size(); ++i) {
        delete mObjects[i];
    }
}

void WorldFrame::initSingleObj()
{
    mScale = 1;

    Sphere* earth = new Sphere("Earth",Qt::green,EARTH_DENSITY,EARTH_RADIUS);
    Sphere* moon = new Sphere("UnrealMoon",Qt::gray,EARTH_DENSITY*10,MOON_RADIUS);

    moon->setPosition(Vect3(EART_MOON_DISTANCE,0,0)+earth->getPosition());
    moon->setVelocity(Vect3(0,-MOON_ORBITAL_SPEED,0)+earth->getVelocity());

    mObjects.append(earth);
    mObjects.append(moon);
}

void WorldFrame::initEarthMoon()
{
    mScale = SCALE_START;

    Sphere* earth = new Sphere("Earth",Qt::green,EARTH_DENSITY,EARTH_RADIUS);
    Sphere* moon = new Sphere("Moon",Qt::gray,MOON_DENSITY,MOON_RADIUS);
    Sphere* moon2 = new Sphere("Moon2",Qt::blue,MOON_DENSITY,MOON_RADIUS);
    Sphere* iss = new Sphere("ISS",Qt::cyan, 1.1,100);

    earth->setVelocity(Vect3(0.0,EARTH_ORBITAL_SPEED,0.0));

    moon->setPosition(Vect3(EART_MOON_DISTANCE,0,0)+earth->getPosition());
    moon->setVelocity(Vect3(0,-MOON_ORBITAL_SPEED,0)+earth->getVelocity());

    moon2->setPosition(Vect3(-EART_MOON_DISTANCE,0,0)+earth->getPosition());
    moon2->setVelocity(Vect3(0,MOON_ORBITAL_SPEED,0)+earth->getVelocity());
    moon2->setDisabled(true);

    iss->setPosition(Vect3(EARTH_RADIUS+ISS_FLY_HEIGHT,0,0)+earth->getPosition());
    iss->setVelocity(Vect3(0,-ISS_ORBITAL_SPEED,0)+earth->getVelocity());
    //iss->setDisabled(true);

    mObjects.append(earth);
    mObjects.append(moon);
    mObjects.append(moon2);
    mObjects.append(iss);
}

void WorldFrame::initParticles(unsigned int count, double rho, double r)
{
    mScale = 1;

    for (int i = 0; i < count; ++i) {
        Sphere* particle = new Sphere("Particle",Qt::gray,rho,r);

        double x = (double)qrand()/(double)RAND_MAX;
        double y = (double)qrand()/(double)RAND_MAX;
        double z = (double)qrand()/(double)RAND_MAX;

        particle->setPosition(Vect3(x,y,z));
        mObjects.append(particle);
    }
}

void WorldFrame::initRenderThread(void)
{
    doneCurrent();
    mRenderThread.start();
}

void WorldFrame::stopRenderThread(void)
{
    mRenderThread.stop();
    mRenderThread.wait();
}

void WorldFrame::keyPressEvent(QKeyEvent *evt)
{
    switch(evt->key()){
        case Qt::Key_Minus:
            zoomOut();
        break;

        case Qt::Key_Plus:
            zoomIn();
        break;

        case Qt::Key_4:
            moveLeft();
        break;

        case Qt::Key_5:
            resetPos();
        break;

        case Qt::Key_6:
            moveRight();
        break;

        case Qt::Key_8:
            moveUp();
        break;

        case Qt::Key_2:
            moveDown();
        break;

        case Qt::Key_Home:
            scaleUp();
        break;

        case Qt::Key_End:
            scaleDown();
        break;

        case Qt::Key_PageUp:
            speedUp();
        break;

        case Qt::Key_PageDown:
            speedDown();
        break;

        case Qt::Key_Q:
            mObjectsMutex.lock();
            if(mObjects.size()>1){
                mObjects[0]->setDisabled(!mObjects[0]->isDisabled());
            }
            mObjectsMutex.unlock();
        break;

        case Qt::Key_W:
            mObjectsMutex.lock();
            if(mObjects.size()>1){
                mObjects[1]->setDisabled(!mObjects[1]->isDisabled());
            }
            mObjectsMutex.unlock();
        break;

        case Qt::Key_E:
            mObjectsMutex.lock();
            if(mObjects.size()>1){
                mObjects[2]->setDisabled(!mObjects[2]->isDisabled());
            }
            mObjectsMutex.unlock();
        break;

        case Qt::Key_A:
            mObjectsMutex.lock();
            if(mObjects.size()>1){
                mObjects[1]->setRho(mObjects[1]->getRho()*1.1);
            }
            mObjectsMutex.unlock();
        break;

        case Qt::Key_S:
            mObjectsMutex.lock();
            if(mObjects.size()>1){
                mObjects[1]->setRho(mObjects[1]->getRho()*0.9);
            }
            mObjectsMutex.unlock();
        break;

        case Qt::Key_H:
            mObjectsMutex.lock();
            for (int i = 0; i < mObjects.size(); ++i) {
                mObjects[i]->setShowHelper(!mObjects[i]->isHelperVisible());
            }
            mObjectsMutex.unlock();
        break;

        case Qt::Key_T:
            mObjectsMutex.lock();
            for (int i = 0; i < mObjects.size(); ++i) {
                mObjects[i]->setShowTracer(!mObjects[i]->isTracerVisible());
            }
            mObjectsMutex.unlock();
        break;
    }
}

void WorldFrame::moveRight(){
    mX -= getMoveSize();
}

void WorldFrame::moveLeft(){
    mX += getMoveSize();
}

void WorldFrame::moveDown(){
    mY += getMoveSize();
}

void WorldFrame::moveUp(){
    mY -= getMoveSize();
}

void WorldFrame::zoomOut()
{
    mZ -= ZOOM_STEP;
}

void WorldFrame::zoomIn()
{
    mZ += ZOOM_STEP;
}

void WorldFrame::scaleUp()
{
    double x = mX / mScale;
    double y = mY / mScale;

    mScale *= SCALE_STEP;

    mX = x*mScale;
    mY = y*mScale;
}

void WorldFrame::scaleDown()
{
    double x = mX / mScale;
    double y = mY / mScale;

    mScale /= SCALE_STEP;

    mX = x*mScale;
    mY = y*mScale;
}

void WorldFrame::speedUp()
{
    mSimUpdateCountPerFrame = qMin(MAX_UPDATE_COUNT,mSimUpdateCountPerFrame*UPDATE_COUNT_FACTOR);
}

void WorldFrame::speedDown()
{

    mSimUpdateCountPerFrame = qMax(1,mSimUpdateCountPerFrame/UPDATE_COUNT_FACTOR);
}

void WorldFrame::updateWorld()
{
    QMutexLocker locker(&mObjectsMutex);


    for (int i = 0; i < mObjects.size(); ++i) {
        if(mObjects[i]->isDisabled()){
            continue;
        }
        mObjects[i]->resetForces();
    }

    double minTimeStepRequiredForMaxDistance = TIMESTEP_STEP;

    for (int i = 0; i < mObjects.size(); ++i) {
        if(mObjects[i]->isDisabled()){
            continue;
        }
        for (int ii = i+1; ii < mObjects.size(); ++ii) {
            if(mObjects[ii]->isDisabled()){
                continue;
            }
            setForces(mObjects[i],mObjects[ii]);
        }

        double timeStepRequiredForMaxDistance = mObjects[i]->getTimestepForDistance(MAX_SIMULATED_DISTANCE);

        if(minTimeStepRequiredForMaxDistance == 0.0){
            minTimeStepRequiredForMaxDistance = timeStepRequiredForMaxDistance;
        }else{
            minTimeStepRequiredForMaxDistance = qMin(minTimeStepRequiredForMaxDistance,timeStepRequiredForMaxDistance);
        }
    }

    if(minTimeStepRequiredForMaxDistance>0.0){
        for (int i = 0; i < mObjects.size(); ++i) {
            if(mObjects[i]->isDisabled()){
                continue;
            }
            mObjects[i]->update(minTimeStepRequiredForMaxDistance);
        }
    }
}

void WorldFrame::printWorldInfo()
{
    qDebug()<<"TimeStep:"<<TIMESTEP_STEP;
    for (int i = 0; i < mObjects.size(); ++i) {
        mObjects[i]->printInfo();
    }
}

void WorldFrame::resetPos()
{
    mX = mY = 0.0f;
    mZ = -5.0f;
}

void WorldFrame::setForces(Object* obj1,Object* obj2){

    Vect3 dist = obj1->getPosition()-obj2->getPosition();

    double m1 = obj1->getMass();
    double m2 = obj2->getMass();

    double distance = sqrt((dist.x*dist.x)+(dist.y*dist.y)+(dist.z*dist.z));

    Vect3 forces;

    if(distance>0.0){
        double force = PHYSIC_G*( m1*m2)/(distance*distance);
        forces = (dist/distance)*force;
    }

    obj1->addForce(-1.0*forces);
    obj2->addForce(forces);
}

void WorldFrame::resizeEvent(QResizeEvent *evt)
{
    mFrameSize = evt->size();
    mRenderThread.resizeViewport();
}

void WorldFrame::glInit()
{
    glClearColor(0.05f, 0.05f, 0.1f, 0.0f);
}

void WorldFrame::glResize(){
    glViewport(0, 0, mFrameSize.width(), mFrameSize.height());
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(45.,((GLfloat)mFrameSize.width())/((GLfloat)mFrameSize.height()),0.1f,1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void WorldFrame::gluPerspective(double fovy,double aspect, double zNear, double zFar)
{
     double xmin, xmax, ymin, ymax;

     ymax = zNear * tan(fovy * PI / 360.0);
     ymin = -ymax;
     xmin = ymin * aspect;
     xmax = ymax * aspect;

     glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void WorldFrame::glPaint()
{
    QMutexLocker locker(&mObjectsMutex);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(mX, mY, mZ);

    for (int i = 0; i < mObjects.size(); ++i) {
        if(mObjects[i]->isDisabled()){
            continue;
        }
        mObjects[i]->draw(mScale);
    }
}

int WorldFrame::getSimUpdateCountPerFrame() const
{
    return mSimUpdateCountPerFrame;
}

void WorldFrame::closeEvent(QCloseEvent *evt)
{
    stopRenderThread();
    QGLWidget::closeEvent(evt);
}

float WorldFrame::getMoveSize()
{
    return qMin(MOVE_STEP_MAX,MOVE_STEP*mScale);
}

void WorldFrame::paintEvent(QPaintEvent *){
    //DO NOTHING, LET THE RENDER THREAD TO THE WORK
}
