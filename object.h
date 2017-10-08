#ifndef OBJECT_H
#define OBJECT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QColor>

#include "mathvector.h"

#define PI 3.14159265

class Object : public QObject
{
    Q_OBJECT
public:
    explicit Object(const QString &name, const QColor& color, double rho);

    Object(const Object &object);

    void setRho(double rho);
    double getRho()const;

    bool isDisabled()const;
    void setDisabled(bool disabled);

    bool isHelperVisible()const;
    void setShowHelper(bool show);

    bool isTracerVisible()const;
    void setShowTracer(bool show);

    void setPosition(const Vect3& position);
    void setVelocity(const Vect3& velocity);

    Vect3 getPosition() const;
    Vect3 getVelocity() const;

    void resetForces();

    void addForce(const Vect3& force);

    void update(double elapsedTime);

    Vect3 getAcceleration() const;

    double getTimestepForDistance(double maxDistance)const;

    virtual double getMass() const;

    virtual double getVolume() const = 0;
    virtual double getMaximumSize()const = 0;

    virtual void draw(double scale);

    virtual void printInfo() const;

    QColor getColor()const;

protected:
    virtual void drawObject(double scale) const = 0;
    void glUseColor() const;

private:
    bool mDisabled;
    bool mShowHelper;
    bool mShowTracer;
    double mTotalElapsedTime;

    Vect3 mPosition;
    Vect3 mVelocity;

    Vect3 mForce;

    double mRho;

    QString mName;
    QColor mColor;

    void drawHelper(double scale) const;

    QList<Vect3> mPrevDrawPos;
    void drawTrace(double scale);
};

#endif // OBJECT_H
