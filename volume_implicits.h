
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

typedef std::shared_ptr<Volume<float> > FloatVolumePtr;
typedef std::shared_ptr<Volume<Vector> > VectorVolumePtr;
typedef std::shared_ptr<Volume<Color> > ColorVolumePtr;

class FloatVolumeBase : public FloatVolumePtr {
    public:
        FloatVolumeBase();
        FloatVolumeBase(Volume<float>* f) : FloatVolumePtr(f){};
        ~FloatVolumeBase();
};

class VectorVolumeBase : public VectorVolumePtr {
    public:
        VectorVolumeBase();
        VectorVolumeBase(Volume<Vector>* f) : VectorVolumePtr(f){};
        ~VectorVolumeBase();
};

class ColorVolumeBase : public ColorVolumePtr {
    public:
        ColorVolumeBase();
        ColorVolumeBase(Volume<Color>* f) : ColorVolumePtr(f){};
        ~ColorVolumeBase();
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

class SphereVolume : public Volume<float>{
    public:

        SphereVolume(float rad) : r(rad){}
       ~SphereVolume(){}

       const typename Volume<float>::volumeDataType eval( const Vector& P ) const { return r - P.magnitude();};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

    private:
       float r;
};

class PyroSphereVolume : public Volume<float>{
    public:

        PyroSphereVolume(float rad, float d, float e, SimplexNoiseObject s) : 
            r(rad),
            exponent(e),
            dBound(d),
            simplex(s){}
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
            float f = r - d + std::pow(std::abs(simplex.eval(n[0], n[1], n[2])), exponent);
            if(f > 0) return 1;
            return 0;};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

    private:
       float r;
       float exponent;
       float dBound;
       SimplexNoiseObject simplex;
};

class SimplexNoiseVolume : public Volume<float>{
    public:
        SimplexNoiseVolume(SimplexNoiseObject s) : simplex(s){};
        ~SimplexNoiseVolume(){};

       const typename Volume<float>::volumeDataType eval( const Vector& P ) const {
           return simplex.eval(P[0], P[1], P[2]);};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

    private:
       SimplexNoiseObject simplex;
};

class SimplexNoiseVectorVolume : public Volume<Vector>{
    public:
        SimplexNoiseVectorVolume(SimplexNoiseObject s, float xO, float yO, float zO) : 
            simplex(s),
            xOffset(xO),
            yOffset(yO),
            zOffset(zO){};
        ~SimplexNoiseVectorVolume(){};

       const typename Volume<Vector>::volumeDataType eval( const Vector& P ) const {
           Vector v;
           v[0] = simplex.eval(P[0] + xOffset, P[1] + xOffset, P[2] + xOffset);
           v[1] = simplex.eval(P[0] + yOffset, P[1] + yOffset, P[2] + yOffset);
           v[2] = simplex.eval(P[0] + zOffset, P[1] + zOffset, P[2] + zOffset);

           return v;};
       const typename Volume<Vector>::volumeGradType grad( const Vector& P ) const { typename Volume<Vector>::volumeGradType G; return G;};

    private:
       SimplexNoiseObject simplex;
       float xOffset, yOffset, zOffset;

};

class SimplexNoiseColorVolume : public Volume<Color>{
    public:
        SimplexNoiseColorVolume(SimplexNoiseObject s, float x, float y, float z) : 
            simplex(s),
            rOffset(x),
            gOffset(y),
            bOffset(z){};
        ~SimplexNoiseColorVolume(){};

       const typename Volume<Color>::volumeDataType eval( const Vector& P ) const {
           Color c;
           c[0] = simplex.eval(P[0] + rOffset, P[1] + rOffset, P[2] + rOffset);
           c[1] = simplex.eval(P[0] + gOffset, P[1] + gOffset, P[2] + gOffset);
           c[2] = simplex.eval(P[0] + bOffset, P[1] + bOffset, P[2] + bOffset);
           return c;};
       const typename Volume<Color>::volumeGradType grad( const Vector& P ) const { typename Volume<Color>::volumeGradType G; return G;};

    private:
       SimplexNoiseObject simplex;
       float rOffset;
       float gOffset;
       float bOffset;

};
class BoxVolume : public Volume<float>{
    public:

        BoxVolume(float rad, float c) : r(rad), cExp(c){}
       ~BoxVolume(){}

       const typename Volume<float>::volumeDataType eval( const Vector& P ) const {
           float exponent = 2 * cExp;
           return pow(r, exponent) - pow(P[0], exponent) - pow(P[1], exponent) - pow(P[2], exponent);};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

    private:
       float r;
       float cExp;
};

class CylinderVolume : public Volume<float>{
    public:

        CylinderVolume(Vector normal, float rad) :
            n(normal),
            r(rad){};
       ~CylinderVolume(){}

       const typename Volume<float>::volumeDataType eval( const Vector& P ) const {
            Vector v = P - (P * n)*n;
            return r - v.magnitude();};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

    private:
       Vector n;
       float r;
};

class PlaneVolume : public Volume<float>{
    public:

        PlaneVolume(Vector normal, Vector center) :
            n(normal),
            c(center){};
       ~PlaneVolume(){}

       const typename Volume<float>::volumeDataType eval( const Vector& P ) const { return -(P - c) * n;};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

    private:
       Vector n;
       Vector c;
};

class ConeVolume : public Volume<float>{
    public:

        ConeVolume(Vector normal, float height, float ang) :
            n(normal),
            h(height),
            angle(ang){};
       ~ConeVolume(){}

       const typename Volume<float>::volumeDataType eval( const Vector& P ) const {
            float dot = P * n;
            if(dot < 0) return dot;
            else if (dot > h) return h - dot;
            else return dot - P.magnitude()*std::cos(angle);};
       const typename Volume<float>::volumeGradType grad( const Vector& P ) const { typename Volume<float>::volumeGradType G; return G;};

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
