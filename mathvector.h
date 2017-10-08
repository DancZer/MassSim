#ifndef MATHVECTOR_H
#define MATHVECTOR_H

#include <qmath.h>

struct Vect3{

    double x,y,z;

    Vect3(){
        x = y = z = 0.0;
    }

    Vect3(double _x, double _y, double _z){
        x = _x;
        y = _y;
        z = _z;
    }

    Vect3(const Vect3 &vect){
        x=vect.x;
        y=vect.y;
        z=vect.z;
    }

    Vect3 & operator += (const Vect3 & v2){
        this->x += v2.x;
        this->y += v2.y;
        this->z += v2.z;
        return *this;
    }
};

inline double operator * (const Vect3 & v1, const Vect3 &v2 ) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vect3 operator + (const Vect3&v1, const Vect3 &v2){
    return Vect3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}

inline Vect3 operator * (double c, const Vect3 & v)
{
    return Vect3(v.x *c, v.y * c, v.z * c );
}

inline Vect3 operator * (const Vect3 & v, double c)
{
    return c*v;
}

inline Vect3 operator - (const Vect3&v1, const Vect3 &v2){
    return Vect3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}

inline Vect3 operator / (const Vect3 & v, double c)
{
    return Vect3(v.x /c, v.y / c, v.z / c );
}

#endif // MATHVECTOR_H
