
#ifndef __VOLUME_OPERATORS_H__
#define __VOLUME_OPERATORS_H__
#include "volume_implicits.h"
namespace lux{
//-------------------------------------------------------------------------------------------------------------------------------
//Basic Arithmetic Operations
//-------------------------------------------------------------------------------------------------------------------------------
class AddVolumef: public Volume<float>{
    public:
        AddVolumef(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~AddVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) + b.get()->eval(P);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) + b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
};

class AddVolumev: public Volume<Vector>{
    public:
        AddVolumev(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~AddVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) + b.get()->eval(P);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) + b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
};

class SubtractVolumef: public Volume<float>{
    public:
        SubtractVolumef(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~SubtractVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) - b.get()->eval(P);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) - b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
};

class SubtractVolumev: public Volume<Vector>{
    public:
        SubtractVolumev(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~SubtractVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) - b.get()->eval(P);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) - b.get()->grad(P);};

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
};

class MultVolumef: public Volume<float>{
    public:
        MultVolumef(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~MultVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->eval(P) * b.get()->grad(P) + a.get()->grad(P) * b.get()->eval(P);};

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
};

/*class MultVolumev: public Volume<Vector>{
    public:
        MultVolumev(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~MultVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return a.get()->eval(P) * b.get()->grad(P) + a.get()->grad(P) * b.get()->eval(P);};

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
};*/

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

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P - b.get()->eval(P) * advectTime);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P);};

        float advectTime;
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
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<Vector> > b;
        float t;
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

class ExpVolumef: public Volume<float>{
    public:
        ExpVolumef(std::shared_ptr<Volume<float> > f) :
            a(f){};
        ~ExpVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return std::exp(a.get()->eval(P));};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P) * eval(P);};

    private:
        std::shared_ptr<Volume<float> > a;
};

class ExpVolumev: public Volume<Vector>{
    public:
        ExpVolumev(std::shared_ptr<Volume<Vector> > f) :
            a(f){};
        ~ExpVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{
            Vector result = a.get()->eval(P);
            result[0] = std::exp(result[0]);
            result[1] = std::exp(result[1]);
            result[2] = std::exp(result[2]);
            return result;};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return /*eval(P) * */a.get()->grad(P);};

    private:
        std::shared_ptr<Volume<Vector> > a;
};

//-------------------------------------------------------------------------------------------------------------------------------
//Union, Intersection, Cutout
//-------------------------------------------------------------------------------------------------------------------------------
class UnionVolumef: public Volume<float>{
    public:
        UnionVolumef(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~UnionVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return std::max(a.get()->eval(P), b.get()->eval(P));};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) > b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
};

class UnionVolumev: public Volume<Vector>{
    public:
        UnionVolumev(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~UnionVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return std::max(a.get()->eval(P), b.get()->eval(P));};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) > b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
};

class IntersectVolumef: public Volume<float>{
    public:
        IntersectVolumef(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~IntersectVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return std::min(a.get()->eval(P), b.get()->eval(P));};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
};

class IntersectVolumev: public Volume<Vector>{
    public:
        IntersectVolumev(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~IntersectVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return std::min(a.get()->eval(P), b.get()->eval(P));};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
};

class CutoutVolumef: public Volume<float>{
    public:
        CutoutVolumef(std::shared_ptr<Volume<float> > f, std::shared_ptr<Volume<float> > g) :
            a(f),
            b(g){};
        ~CutoutVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return std::min(a.get()->eval(P), -b.get()->eval(P));};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return -b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<float> > a;
        std::shared_ptr<Volume<float> > b;
};

class CutoutVolumev: public Volume<Vector>{
    public:
        CutoutVolumev(std::shared_ptr<Volume<Vector> > f, std::shared_ptr<Volume<Vector> > g) :
            a(f),
            b(g){};
        ~CutoutVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return std::min(a.get()->eval(P), -b.get()->eval(P));};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return -b.get()->grad(P);
            }
        };

    private:
        std::shared_ptr<Volume<Vector> > a;
        std::shared_ptr<Volume<Vector> > b;
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
class TranslateVolumef: public Volume<float>{
    public:
        TranslateVolumef(std::shared_ptr<Volume<float> > f, Vector d) :
            a(f),
            delta(d){};
        ~TranslateVolumef(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P - delta);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P - delta);};

    private:
        std::shared_ptr<Volume<float> > a;
        Vector delta;
};

class TranslateVolumev: public Volume<Vector>{
    public:
        TranslateVolumev(std::shared_ptr<Volume<Vector> > f, Vector d) :
            a(f),
            delta(d){};
        ~TranslateVolumev(){};

        const typename Volume<Vector>::volumeDataType eval( const Vector& P) const{ return a.get()->eval(P - delta);};
        const typename Volume<Vector>::volumeGradType grad( const Vector& P) const{ return a.get()->grad(P - delta);};

    private:
        std::shared_ptr<Volume<Vector> > a;
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
