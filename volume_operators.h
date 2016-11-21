
#ifndef __VOLUME_OPERATORS_H__
#define __VOLUME_OPERATORS_H__
#include "volume_implicits.h"
namespace lux{
//-------------------------------------------------------------------------------------------------------------------------------
//Basic Arithmetic Operations
//-------------------------------------------------------------------------------------------------------------------------------
template< typename U >
class AddVolume: public Volume<U>{
    public:
        AddVolume(std::shared_ptr<Volume<U> > f, std::shared_ptr<Volume<U> > g) :
            a(f),
            b(g){};
        ~AddVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) + b.get()->eval(P);};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) + b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<U> > a;
        std::shared_ptr<Volume<U> > b;
};

template< typename U >
class SubtractVolume: public Volume<U>{
    public:
        SubtractVolume(std::shared_ptr<Volume<U> > f, std::shared_ptr<Volume<U> > g) :
            a(f),
            b(g){};
        ~SubtractVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) - b.get()->eval(P);};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) - b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<U> > a;
        std::shared_ptr<Volume<U> > b;
};

template< typename U >
class MultVolume: public Volume<U>{
    public:
        MultVolume(std::shared_ptr<Volume<U> > f, std::shared_ptr<Volume<U> > g) :
            a(f),
            b(g){};
        ~MultVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{ return a.get()->eval(P) * b.get()->grad(P) + a.get()->grad(P) * b.get()->eval(P);};

    private:
        std::shared_ptr<Volume<U> > a;
        std::shared_ptr<Volume<U> > b;
};

class Mult_SV_Volume: public Volume<Vector>{
    public:
        Mult_SV_Volume(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};

        Mult_SV_Volume( std::shared_ptr<Volume<Vector> > g, std::shared_ptr<Volume<float> > f) :
            a(f),
            b(g){};
        ~Mult_SV_Volume(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<Vector> > b;
};

class Advect_SL_Volume: public Volume<float>{
    public:
        Advect_SL_Volume(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};

        Advect_SL_Volume( std::shared_ptr<Volume<Vector> > g, std::shared_ptr<Volume<float> > f) :
            a(f),
            b(g){};
        Advect_SL_Volume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P, const double t) const{ return a.get()->eval(P - b.get()->eval(P) * t);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<Vector> > b;
};

class Advect_MMC_Volume: public Volume<float>{
    public:
        Advect_MMC_Volume(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<Vector> > g, const float newTime) :
            a(f),
            b(g),
            t(newTime){};

        Advect_MMC_Volume( std::shared_ptr<Volume<Vector> > g, std::shared_ptr<Volume<float> > f, const float newTime) :
            a(f),
            b(g),
            t(newTime){};
        ~Advect_MMC_Volume(){};
        void setTime(const float newTime){ t = newTime;};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{
            float semiLagrangian = a.get()->eval(P - b.get()->eval(P) * t);
            float errorTerm = a.get()->eval(P) - semiLagrangian; 
            return semiLagrangian - errorTerm * 0.5;};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        float t;
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<Vector> > b;
};


//TODO: Matrix and Vector Multiplication
/*class Mult_VM_Volume: public Volume<Vector>{
    public:
        Mult_VM_Volume( std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Matrix> > g) :
            a(f),
            b(g){};
        ~Mult_VM_Volume(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};//TODO
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return b.get()->grad(P);};//TODO

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Matrix> > b;
};

class Mult_MV_Volume: public Volume<Matrix>{
    public:
        Mult_MV_Volume( std::shared_ptr<Volume<Matrix> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~Mult_MV_Volume(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};//TODO
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return b.get()->grad(P);};//TODO: Gradient of matrix does not matter

    private:
        std::shared_ptr<Volume<Matrix> > a;
        std::shared_ptr<Volume<Vector> > b;
};*/

class DivideVolume: public Volume<float>{
    public:
        DivideVolume(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~DivideVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) / b.get()->eval(P);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{
            return (a.get()->grad(P) * b.get()->eval(P) - a.get()->eval(P) * b.get()->grad(P)) /
                (b.get()->grad(P) * b.get()->grad(P));};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
};

class Divide_SV_Volume: public Volume<Vector>{
    public:
        Divide_SV_Volume(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~Divide_SV_Volume(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return b.get()->eval(P) / a.get()->eval(P);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<Vector> > b;
};

template< typename U >
class ExpVolume: public Volume<U>{
    public:
        ExpVolume(std::shared_ptr<Volume<U> > f) :
            a(f){};
        ~ExpVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return std::exp(a.get()->eval(P));};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) * eval(P);};

    private:
        std::shared_ptr<Volume<U> > a;
};

//-------------------------------------------------------------------------------------------------------------------------------
//Union, Intersection, Cutout
//-------------------------------------------------------------------------------------------------------------------------------
template< typename U >
class UnionVolume: public Volume<U>{
    public:
        UnionVolume(std::shared_ptr<Volume<U> > f, std::shared_ptr<Volume<U> > g) :
            a(f),
            b(g){};
        ~UnionVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return std::max(a.get()->eval(P), b.get()->eval(P));};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) > b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<U> > a;
        std::shared_ptr<Volume<U> > b;
};

template< typename U >
class IntersectVolume: public Volume<U>{
    public:
        IntersectVolume(std::shared_ptr<Volume<U> > f, std::shared_ptr<Volume<U> > g) :
            a(f),
            b(g){};
        ~IntersectVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return std::min(a.get()->eval(P), b.get()->eval(P));};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<U> > a;
        std::shared_ptr<Volume<U> > b;
};

template< typename U >
class CutoutVolume: public Volume<U>{
    public:
        CutoutVolume(std::shared_ptr<Volume<U> > f, std::shared_ptr<Volume<U> > g) :
            a(f),
            b(g){};
        ~CutoutVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return std::min(a.get()->eval(P), -b.get()->eval(P));};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return -b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<U> > a;
        std::shared_ptr<Volume<U> > b;
};

//-------------------------------------------------------------------------------------------------------------------------------
class SinVolume: public Volume<float>{
    public:
        SinVolume(std::shared_ptr<Volume<float> > f) :
            a(f){};
        ~SinVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return std::sin(a.get()->eval(P));};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) * std::cos(a.get()->eval(P));};

    private:
        std::shared_ptr<Volume<float> > a;
};

class CosVolume: public Volume<float>{
    public:
        CosVolume(std::shared_ptr<Volume<float> > f) :
            a(f){};
        ~CosVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return std::cos(a.get()->eval(P));};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) * -std::sin(a.get()->eval(P));};

    private:
        std::shared_ptr<Volume<float> > a;
};

//-------------------------------------------------------------------------------------------------------------------------------
//Linear Algebra
//-------------------------------------------------------------------------------------------------------------------------------
class DotProductVolume: public Volume<float>{
    public:
        DotProductVolume(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~DotProductVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return Vector(0,0,0);};

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
};

class CrossProductVolume: public Volume<Vector>{
    public:
        CrossProductVolume(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~CrossProductVolume(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) ^ b.get()->eval(P);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
};
//-------------------------------------------------------------------------------------------------------------------------------
//Transformations
//-------------------------------------------------------------------------------------------------------------------------------
template< typename U >
class TranslateVolume: public Volume<U>{
    public:
        TranslateVolume(std::shared_ptr<Volume<U> > f, Vector d) :
            a(f),
            delta(d){};
        ~TranslateVolume(){};

        const typename Volume<U>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P - delta);};
        const typename Volume<U>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P - delta);};

    private:
        std::shared_ptr<Volume<U> > a;
        Vector delta;
};

class Scale_S_Volume: public Volume<float>{
    public:
        Scale_S_Volume(std::shared_ptr<Volume<float> > f, Vector p, float s) :
            a(f),
            pivot(p),
            scaleFactor(s){};
        ~Scale_S_Volume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(((P - pivot) / scaleFactor) + pivot);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(((P - pivot) / scaleFactor) + pivot);};

    private:
        std::shared_ptr<Volume<float> > a;
        Vector pivot;
        float scaleFactor;
};

class Scale_V_Volume: public Volume<Vector>{
    public:
        Scale_V_Volume(std::shared_ptr<Volume<Vector> > f, Vector p, float s) :
            a(f),
            pivot(p),
            scaleFactor(s){};
        ~Scale_V_Volume(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return scaleFactor * a.get()->eval(((P - pivot) / scaleFactor) + pivot);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(((P - pivot) / scaleFactor) + pivot);};

    private:
        std::shared_ptr<Volume<Vector> > a;
        Vector pivot;
        float scaleFactor;
};

class Scale_M_Volume: public Volume<Matrix>{
    public:
        Scale_M_Volume(std::shared_ptr<Volume<Matrix> > f, Vector p, float s) :
            a(f),
            pivot(p),
            scaleFactor(s){};
        ~Scale_M_Volume(){};

        const typename Volume<Matrix>::volumeDataType eval( const Vector& P) const{ return scaleFactor * scaleFactor * a.get()->eval(((P - pivot) / scaleFactor) + pivot);};
        const typename Volume<Matrix>::volumeGradType grad( const Vector& P) const{ return 2 * scaleFactor * a.get()->grad(((P - pivot) / scaleFactor) + pivot);};

    private:
        std::shared_ptr<Volume<Matrix> > a;
        Vector pivot;
        float scaleFactor;
};

class Rotate_S_Volume: public Volume<float>{
    public:
        Rotate_S_Volume(std::shared_ptr<Volume<float> > f, Vector p, Vector s, float ang) :
            a(f),
            pivot(p),
            rotAxis(s.unitvector()),
            angle(ang){};
        ~Rotate_S_Volume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{
            Vector rotatedPoint = P * std::cos(angle);
            rotatedPoint += rotAxis * (P * rotAxis * (1 - std::cos(angle)));
            rotatedPoint += P ^ (rotAxis * std::sin(angle));
            return a.get()->eval(rotatedPoint);
        };
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        Vector pivot;
        Vector rotAxis;
        float angle;
};

class BlinnBlend_S_Volume: public Volume<float>{
    public:
        BlinnBlend_S_Volume(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g, float Sf, float Sg, float Beta) :
            a(f),
            b(g),
            sf(Sf),
            sg(Sg),
            beta(Beta){};
        ~BlinnBlend_S_Volume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{
            return std::exp(a.get()->eval(P) / sf) + std::exp(b.get()->eval(P) / sg) - beta;}
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
        float sf;
        float sg;
        float beta;
};


class MaskVolume: public Volume<float>{
    public:
        MaskVolume(std::shared_ptr<Volume<float> > f) :
            a(f){};
        ~MaskVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{
            if (a.get()->eval(P) > 0.0) return 1;
            return 0;};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
};

class ClampVolume: public Volume<float>{
    public:
        ClampVolume(std::shared_ptr<Volume<float> > f, float Min, float Max) :
            a(f),
            minVal(Min),
            maxVal(Max){};
        ~ClampVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{
            float val = a.get()->eval(P);
            if (val < minVal) return minVal;
            else if (val > maxVal) return maxVal;
            return val;};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        float minVal;
        float maxVal;
};

//End namespace lux
}
#endif
