
#ifndef __VOLUME_IMPLICITS_H__
#define __VOLUME_IMPLICITS_H__

#include "Vector.h"
#include "Matrix.h"
#include "Color.h"
#include "simplexnoise.h"
#include "boundingbox.h"
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

class FloatVolume{
  public:

    FloatVolume(){};

   virtual ~FloatVolume(){};

   virtual const float eval( const Vector& P ) const { float base = 0; return base; };
   virtual const Vector grad( const Vector& P ) const { Vector G; return G;};
};

class VectorVolume{
  public:

    VectorVolume(){};

   virtual ~VectorVolume(){};

   virtual const Vector eval( const Vector& P ) const { Vector base; return base; };
   virtual const Matrix grad( const Vector& P ) const { Matrix G; return G;};
};

class ColorVolume{
  public:

    ColorVolume(){};

   virtual ~ColorVolume(){};

   virtual const Color eval( const Vector& P ) const { Color base; return base; };
   virtual const int grad( const Vector& P ) const {return 0;};
};

typedef std::shared_ptr<FloatVolume> FloatVolumePtr;
typedef std::shared_ptr<VectorVolume> VectorVolumePtr;
typedef std::shared_ptr<ColorVolume> ColorVolumePtr;

/*template< typename U>
class dumb{
    public:
        dumb(){};
        ~dumb(){};
        U eval(){return 1;}
};

typedef dumb<float> dumbf;
class dumber : public dumbf{
    public:
        dumber(){};
        ~dumber(){};
        float eval(){return 2;}
};

typedef std::shared_ptr<dumbf > DumbPtr;

class dumbBase{
    public: 
        dumbBase(){};
        dumbBase(dumbf * d){};
        ~dumbBase(){};
};*/
class FloatVolumeBase : public FloatVolumePtr {
    public:
        FloatVolumeBase() : BB(Vector(0, 0, 0), Vector(4, 4, 4)){};
        FloatVolumeBase(FloatVolume* f) : FloatVolumePtr(f), BB(Vector(0, 0, 0), Vector(4, 4, 4)){};
        ~FloatVolumeBase(){};
        const FloatVolume* get() const {return FloatVolumePtr::get();};
        BoundingBox BB;
};

class VectorVolumeBase : public VectorVolumePtr {
    public:
        VectorVolumeBase() : BB(Vector(0, 0, 0), Vector(4, 4, 4)){};
        VectorVolumeBase(VectorVolume* f) : VectorVolumePtr(f){};
        ~VectorVolumeBase(){};
        const VectorVolume* get() const {return VectorVolumePtr::get();};
        BoundingBox BB;
};

class ColorVolumeBase : public ColorVolumePtr {
    public:
        ColorVolumeBase(){};
        ColorVolumeBase(ColorVolume* f) : ColorVolumePtr(f){};
        ~ColorVolumeBase(){};
};

class ConstantVolumef : public FloatVolume{
    public:

        ConstantVolumef(float val) : value(val){}
       ~ConstantVolumef(){}

       const float eval( const Vector& P ) const { return value;};
       const Vector grad( const Vector& P ) const { Vector G(0,0,0); return G;};

    private:
       float value;
};

class ConstantVolumev : public VectorVolume{
    public:

        ConstantVolumev(float x, float y, float z) : value(x, y, z){}
       ~ConstantVolumev(){}

       const Vector eval( const Vector& P ) const { return value;};
       const Matrix grad( const Vector& P ) const { Matrix G; return G;};

    private:
       Vector value;
};


class SphereDistVolume : public FloatVolume{
    public:

        SphereDistVolume(float rad) : r(rad){};
       ~SphereDistVolume(){};

       const float eval( const Vector& P ) const {
           float d = r - P.magnitude();
            if (d <= 0) return 0;
            return d;};
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

    private:
       float r;
};
class SphereVolume : public FloatVolume{
    public:

        SphereVolume(float rad) : r(rad){};
       ~SphereVolume(){};

       const float eval( const Vector& P ) const {
            if ((r - P.magnitude()) <= 0) return 0;
            return 1;};
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

    private:
       float r;
};

class PyroSphereVolume : public FloatVolume{
    public:

        PyroSphereVolume(float rad, float d, float e, SimplexNoiseObject s) : 
            r(rad),
            exponent(e),
            dBound(d),
            simplex(s){}
       ~PyroSphereVolume(){}

       const float eval( const Vector& P ) const {
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
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

    private:
       float r;
       float exponent;
       float dBound;
       SimplexNoiseObject simplex;
};

class SimplexNoiseVolume : public FloatVolume{
    public:
        SimplexNoiseVolume(SimplexNoiseObject s) : simplex(s){};
        ~SimplexNoiseVolume(){};

       const float eval( const Vector& P ) const {
           return simplex.eval(P[0], P[1], P[2]);};
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

    private:
       SimplexNoiseObject simplex;
};

class FloatGradientVolume : public FloatVolume{
    FloatGradientVolume(const Vector& d, const Vector& start, const float l, const float mn, const float mx) :
        direction(d.unitvector()),
        position(start),
        length(l),
        min(mn),
        max(mx){};

    ~FloatGradientVolume(){};

    const float eval(const Vector& P) const{
        //find the scalar projection onto our line
        Vector u = (direction*length) + position;
        Vector v = position + P;
        float dot = u*v;
        float scalarProjection = dot / u.magnitude();

        //then interpolate to between the min and max values
        return (scalarProjection / length) * (max - min);
    }
    const Vector grad(const Vector& P) const { Vector G(0, 0, 0); return G;};

    private:
        Vector direction;
        Vector position;
        float length;
        float min;
        float max;

};
/*class NoiseSphere : public FloatVolume{

};*/
class SimplexNoiseVectorVolume : public VectorVolume{
    public:
        SimplexNoiseVectorVolume(SimplexNoiseObject s, float xO, float yO, float zO) : 
            simplex(s),
            xOffset(xO),
            yOffset(yO),
            zOffset(zO){};
        ~SimplexNoiseVectorVolume(){};

       const Vector eval( const Vector& P ) const {
           Vector v;
           v[0] = simplex.eval(P[0] + xOffset, P[1] + xOffset, P[2] + xOffset);
           v[1] = simplex.eval(P[0] + yOffset, P[1] + yOffset, P[2] + yOffset);
           v[2] = simplex.eval(P[0] + zOffset, P[1] + zOffset, P[2] + zOffset);

           return v;};
       const Matrix grad( const Vector& P ) const { Matrix G; return G;};

    private:
       SimplexNoiseObject simplex;
       float xOffset, yOffset, zOffset;

};

class SimplexNoiseColorVolume : public ColorVolume{
    public:
        SimplexNoiseColorVolume(SimplexNoiseObject s, float x, float y, float z) : 
            simplex(s),
            rOffset(x),
            gOffset(y),
            bOffset(z){};
        ~SimplexNoiseColorVolume(){};

       const Color eval( const Vector& P ) const {
           Color c;
           c[0] = simplex.eval(P[0] + rOffset, P[1] + rOffset, P[2] + rOffset);
           c[1] = simplex.eval(P[0] + gOffset, P[1] + gOffset, P[2] + gOffset);
           c[2] = simplex.eval(P[0] + bOffset, P[1] + bOffset, P[2] + bOffset);
           return c;};
       const int grad( const Vector& P ) const {return 0;};

    private:
       SimplexNoiseObject simplex;
       float rOffset;
       float gOffset;
       float bOffset;

};
class BoxVolume : public FloatVolume{
    public:

        BoxVolume(float rad, float c) : r(rad), cExp(c){}
       ~BoxVolume(){}

       const float eval( const Vector& P ) const {
           float exponent = 2 * cExp;
           return pow(r, exponent) - pow(P[0], exponent) - pow(P[1], exponent) - pow(P[2], exponent);};
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

    private:
       float r;
       float cExp;
};

class CylinderVolume : public FloatVolume{
    public:

        CylinderVolume(Vector normal, float rad) :
            n(normal),
            r(rad){};
       ~CylinderVolume(){}

       const float eval( const Vector& P ) const {
            Vector v = P - (P * n)*n;
            return r - v.magnitude();};
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

    private:
       Vector n;
       float r;
};

class PlaneVolume : public FloatVolume{
    public:

        PlaneVolume(Vector normal, Vector center) :
            n(normal),
            c(center){};
       ~PlaneVolume(){}

       const float eval( const Vector& P ) const { return -(P - c) * n;};
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

    private:
       Vector n;
       Vector c;
};

class ConeVolume : public FloatVolume{
    public:

        ConeVolume(Vector normal, float height, float ang) :
            n(normal),
            h(height),
            angle(ang){};
       ~ConeVolume(){};

       const float eval( const Vector& P ) const {
            float dot = P * n;
            if(dot < 0) return dot;
            else if (dot > h) return h - dot;
            else return dot - P.magnitude()*std::cos(angle);};
       const Vector grad( const Vector& P ) const {  Vector G(0, 0, 0); return G;};

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
