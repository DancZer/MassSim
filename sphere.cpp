#include <QGLWidget>
#include <qmath.h>

#include "sphere.h"

Sphere::Sphere(const QString &name, const QColor& color, double rho, double radius):
    Object(name,color,rho),
    mRadius(radius)
{
}

void Sphere::drawObject(double scale) const
{
    glPushMatrix();

    Vect3 pos = getPosition();
    glTranslatef(pos.x*scale, pos.y*scale, pos.z*scale);

    glBegin(GL_TRIANGLE_FAN);

    glUseColor();

    double step = PI/180.0;
    for (double angle = 0; angle < 2*PI; angle += step) {
        glVertex3f(qSin(angle) * mRadius*scale, qCos(angle) * mRadius*scale,0.0);
        glVertex3f(qSin(angle+step) * mRadius*scale, qCos(angle+step) * mRadius*scale,0.0);
    }
    glEnd();

    glPopMatrix();
}

double Sphere::getVolume() const
{
    return (4.0*PI*mRadius*mRadius*mRadius)/3.0;
}

double Sphere::getMaximumSize() const
{
    return mRadius;
}


