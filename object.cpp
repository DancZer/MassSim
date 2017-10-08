#include <QGLWidget>
#include <QDebug>
#include <qmath.h>

#include "object.h"

#define HELPER_RADIUS 0.1

#define ENABLE_TRACE
#define TRACE_MAX_LENGTH 100

#define SPEED_OF_LIGHT 299792458.0

Object::Object(const QString &name, const QColor& color, double rho) :
    QObject(),
    mRho(rho),
    mName(name),
    mColor(color),
    mDisabled(false),
    mShowHelper(false),
    mShowTracer(false)
{
    mTotalElapsedTime = 0.0;
}

Object::Object(const Object &object)
{
    mForce = object.mForce;
    mPosition = object.mPosition;

    mRho = object.mRho;

    mName = object.mName;
}

void Object::setRho(double rho)
{
    mRho = rho;
}

double Object::getRho() const
{
    return mRho;
}

bool Object::isDisabled() const
{
    return mDisabled;
}

void Object::setDisabled(bool disabled)
{
    mDisabled = disabled;
}

void Object::setPosition(const Vect3& position)
{
    mPosition = position;
}

void Object::setVelocity(const Vect3& velocity)
{
    mVelocity = velocity;
}

Vect3 Object::getPosition() const
{
    return mPosition;
}

Vect3 Object::getVelocity() const
{
    return mVelocity;
}

void Object::resetForces()
{
    mForce.x = 0.0;
    mForce.y = 0.0;
    mForce.z = 0.0;
}

void Object::addForce(const Vect3& force)
{
    mForce += force;
}

void Object::update(double elapsedTime)
{

    mTotalElapsedTime += elapsedTime;

    Vect3 acceleratedSpeed = elapsedTime * getAcceleration();

    //#error Túl nagy a gyorsulás ezért simán átlépi a fénysebességet

    mVelocity = (1.0/(1.0 + (acceleratedSpeed*mVelocity)/(SPEED_OF_LIGHT*SPEED_OF_LIGHT))) * (acceleratedSpeed + mVelocity);

    mPosition += mVelocity*elapsedTime;

/*
    mVelocity += getAcceleration()*elapsedTime;
    mPosition += mVelocity * elapsedTime;*/
}

Vect3 Object::getAcceleration() const
{
    double mass = getMass();

    if(mass>0.0){
        return mForce/getMass();
    }else{
        return Vect3();
    }
}

double Object::getTimestepForDistance(double maxDistance) const
{
    Vect3 vAcc = getAcceleration();

    double acc = abs(sqrt(vAcc*vAcc));
    double vel = abs(sqrt(mVelocity*mVelocity));

    double minTime = 0.0;
    bool hasAccTime = false;

    if(acc >0.0){
        hasAccTime = false;
        minTime = sqrt(2*maxDistance / acc);
    }

    if(vel >0.0){
        double velTime = maxDistance / vel;

        minTime = hasAccTime?qMin(velTime,minTime):velTime;
    }

    return minTime;
}

double Object::getMass() const
{
    return getVolume()*mRho;
}

void Object::draw(double scale)
{
    drawObject(scale);

    if(mShowHelper){
        drawHelper(scale);
    }

#ifdef ENABLE_TRACE
    drawTrace(scale);
#endif
}

void Object::printInfo() const
{
    if(mDisabled){
        qDebug()<<mName<<" info(Disabled):";
    }else{
        qDebug()<<mName<<" info("<<mTotalElapsedTime<<"):";
    }

    Vect3 acc = getAcceleration();
    qDebug()<<"\tVolume:"<<getVolume()<<" m3";
    qDebug()<<"\tMass:"<<getMass()<<" kg";
    qDebug()<<"\tAcc :"<<sqrt(acc*acc)<<" m/s^2";
    qDebug("\tVel: %f km/s",sqrt(mVelocity*mVelocity)/1000.);
}

QColor Object::getColor() const
{
    return mColor;
}

void Object::glUseColor() const
{
    glColor3f(mColor.redF(),mColor.greenF(),mColor.blueF());
}

void Object::drawHelper(double scale) const
{
    glPushMatrix();

    Vect3 pos = getPosition();

    glTranslatef(pos.x*scale, pos.y*scale, pos.z*scale);

    glBegin(GL_LINES);

    glUseColor();

    double step = PI/180.0;
    for (double angle = 0; angle < 2*PI; angle += step) {
        glVertex3f(qSin(angle) * HELPER_RADIUS, qCos(angle) * HELPER_RADIUS,0.);
        glVertex3f(qSin(angle+step) * HELPER_RADIUS, qCos(angle+step) * HELPER_RADIUS,0.);
    }

    glEnd();

    glPopMatrix();
}

void Object::drawTrace(double scale)
{
    if(mPrevDrawPos.length()>0){
        Vect3 dist = mPosition-mPrevDrawPos.last();

        if(sqrt(dist*dist) >= getMaximumSize()){
            mPrevDrawPos.append(mPosition);
        }
    }else{
        mPrevDrawPos.append(mPosition);
    }

    while (mPrevDrawPos.length()>TRACE_MAX_LENGTH) {
        mPrevDrawPos.removeFirst();
    }

    if(mShowTracer){
        glPushMatrix();

        glBegin(GL_LINES);

        glUseColor();

        for (int i = 0; i < mPrevDrawPos.length()-1; ++i) {
            glVertex3f(mPrevDrawPos[i].x*scale, mPrevDrawPos[i].y*scale,mPrevDrawPos[i].z*scale);
            glVertex3f(mPrevDrawPos[i+1].x*scale, mPrevDrawPos[i+1].y*scale,mPrevDrawPos[i+1].z*scale);
        }

        glEnd();

        glPopMatrix();
    }
}

bool Object::isHelperVisible() const
{
    return mShowHelper;
}

void Object::setShowHelper(bool show)
{
    mShowHelper = show;
}

bool Object::isTracerVisible() const
{
    return mShowTracer;
}


void Object::setShowTracer(bool show)
{
    mShowTracer = show;
}
