#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include <QColor>

class Sphere : public Object
{
public:
    explicit Sphere(const QString &name, const QColor& color, double rho, double radius);

    double getVolume() const;
    double getMaximumSize()const;

protected:
    void drawObject(double scale) const;

private:
    double mRadius;
};

#endif // SPHERE_H
