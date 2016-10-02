
#ifndef __VOLUME_IMPLICITS_H__
#define __VOLUME_IMPLICITS_H__

#include "Vector.h"
#include "Matrix.h"
#include "Color.h"
#include "simplexnoise.h"
#include <memory>

namespace lux{
//-----------------------------------------------------------------------------
// Setting up logic to be able to determine the data type of the gradient
template <typename U>
struct GradType
{
   typedef int GType;
};

//If U is float, GType is vector
template<>
struct GradType<float>
{
   typedef Vector GType;
};

//If U is vector, GType is Matrix
template<>
struct GradType<Vector>
{
   typedef Matrix GType;
};

//Color and Matrix volumes gradients will be disregarded
template<>
struct GradType<Color>{
    typedef int GType;
};

template<>
struct GradType<Matrix>{
    typedef int GType;
};
//-----------------------------------------------------------------------------

template< typename U >
class Volume{
  public:

    Volume(){}

   virtual ~Volume(){}

   typedef U volumeDataType;
   typedef typename GradType<U>::GType volumeGradType;

   virtual const volumeDataType eval( const Vector& P ) const;
   virtual const volumeGradType grad( const Vector& P ) const;
};

template< typename U >
class ConstantVolume : public Volume<U>{
    public:

        ConstantVolume(U val) : value(val){}
       ~ConstantVolume(){}

       const typename Volume<U>::volumeDataType eval( const Vector& P ) const { return value;};
       const typename Volume<U>::volumeGradType grad( const Vector& P ) const { typename Volume<U>::volumeGradType G; return G;};

    private:
       U value;
};

template< typename U >
class SphereVolume : public Volume<U>{
    public:

        SphereVolume(float rad) : r(rad){}
       ~SphereVolume(){}

       const typename Volume<U>::volumeDataType eval( const Vector& P ) const { return r - P.magnitude();};
       const typename Volume<U>::volumeGradType grad( const Vector& P ) const { typename Volume<U>::volumeGradType G; return G;};

    private:
       float r;
};

class PyroSphereVolume : public Volume<float>{
    public:

        PyroSphereVolume(float rad, float d) : r(rad), dBound(d){}
       ~PyroSphereVolume(){}

       const typename Volume<float>::volumeDataType eval( const Vector& P ) const {
            //calculate distance from sphere center. Assume sphere center to be (0, 0, 0)
            float d = P.magnitude();
            //If distance is less than the radius, we are inside there sphere and have full density.
            //Outside of sphere and bounding range, density is 0
            if(d > (r + dBound)){
                return 0;
            }
            else if(d < r){
                return 1;
            }
            lux::Vector n(P.unitvector());
            //float f = r - d + octave_noise_3d(1, 0.5, 1.0, n[0], n[1], n[2]);
            //float f = r - d + std::pow(std::abs(octave_noise_3d(1, 0.5, 1.0, n[0], n[1], n[2])), 2)
            float f = r - d + std::pow(std::abs(scaled_octave_noise_3d(4, 0.3, 1.5, -1.5, 1.5, n[0], n[1], n[2])), 1);
            //std::cout << f << "\n";
            if(f > 0) return 1;
            return 0;};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

    private:
       float r;
       float dBound;
};

template< typename U >
class BoxVolume : public Volume<U>{
    public:

        BoxVolume(float rad, float c) : r(rad), cExp(c){}
       ~BoxVolume(){}

       const typename Volume<U>::volumeDataType eval( const Vector& P ) const {
           float exponent = 2 * cExp;
           return pow(r, exponent) - pow(P[0], exponent) - pow(P[1], exponent) - pow(P[2], exponent);};
       const typename Volume<U>::volumeGradType grad( const Vector& P ) const { typename Volume<U>::volumeGradType G; return G;};

    private:
       float r;
       float cExp;
};

template< typename U >
class CylinderVolume : public Volume<U>{
    public:

        CylinderVolume(Vector normal, float rad) :
            n(normal),
            r(rad){};
       ~CylinderVolume(){}

       const typename Volume<U>::volumeDataType eval( const Vector& P ) const {
            Vector v = P - (P * n)*n;
            return r - v.magnitude();};
       const typename Volume<U>::volumeGradType grad( const Vector& P ) const { typename Volume<U>::volumeGradType G; return G;};

    private:
       Vector n;
       float r;
};

template< typename U >
class PlaneVolume : public Volume<U>{
    public:

        PlaneVolume(Vector normal, Vector center) :
            n(normal),
            c(center){};
       ~PlaneVolume(){}

       const typename Volume<U>::volumeDataType eval( const Vector& P ) const { return -(P - c) * n;};
       const typename Volume<U>::volumeGradType grad( const Vector& P ) const { typename Volume<U>::volumeGradType G; return G;};

    private:
       Vector n;
       Vector c;
};

template< typename U >
class ConeVolume : public Volume<U>{
    public:

        ConeVolume(Vector normal, float height, float ang) :
            n(normal),
            h(height),
            angle(ang){};
       ~ConeVolume(){}

       const typename Volume<U>::volumeDataType eval( const Vector& P ) const {
            float dot = P * n;
            if(dot < 0) return dot;
            else if (dot > h) return h - dot;
            else return dot - P.magnitude()*std::cos(angle);};
       const typename Volume<U>::volumeGradType grad( const Vector& P ) const { typename Volume<U>::volumeGradType G; return G;};

    private:
       //axis
       Vector n;
       //height
       float h;
       //cone angle at peak
       float angle;
};
}
#endif
