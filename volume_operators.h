
#ifndef __VOLUME_OPERATORS_H__
#define __VOLUME_OPERATORS_H__
#include "volume_implicits.h"
#include "ColorSlider.h"
namespace lux{

//-------------------------------------------------------------------------------------------------------------------------------
//Basic Arithmetic Operations
//-------------------------------------------------------------------------------------------------------------------------------
class AddVolumef: public FloatVolume{
    public:
        AddVolumef(FloatVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~AddVolumef(){};

        float eval( const Vector& P) const{ return a.get()->eval(P) + b.get()->eval(P);};
        Vector grad( const Vector& P) const{ return a.get()->grad(P) + b.get()->grad(P);};

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
};

class AddVolumev: public VectorVolume{
    public:
        AddVolumev(VectorVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~AddVolumev(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P) + b.get()->eval(P);};
        Matrix grad( const Vector& P) const{ return a.get()->grad(P) + b.get()->grad(P);};

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};

class SubtractVolumef: public FloatVolume{
    public:
        SubtractVolumef(FloatVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~SubtractVolumef(){};

        float eval( const Vector& P) const{ return a.get()->eval(P) - b.get()->eval(P);};
        Vector grad( const Vector& P) const{ return a.get()->grad(P) - b.get()->grad(P);};

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
};

class SubtractVolumev: public VectorVolume{
    public:
        SubtractVolumev(VectorVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~SubtractVolumev(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P) - b.get()->eval(P);};
        Matrix grad( const Vector& P) const{ return a.get()->grad(P) - b.get()->grad(P);};

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};

class MultVolumef: public FloatVolume{
    public:
        MultVolumef(FloatVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~MultVolumef(){};

        float eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        Vector grad( const Vector& P) const{ return a.get()->eval(P) * b.get()->grad(P) + a.get()->grad(P) * b.get()->eval(P);};

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
};

class ColorVolumeFromDensity: public ColorVolume{
    public:
        ColorVolumeFromDensity(FloatVolumeBase f, const ColorSlider& cSlider, float min, float max) :
            a(f),
            c(cSlider),
            cMin(min),
            cMax(max){};
       ~ColorVolumeFromDensity(){}; 

        Color eval( const Vector& P) const{ 
            float val = (a.get()->eval(P) - cMin) / (cMax - cMin);
            return c.getColor(val);};
        int grad( const Vector& P) const{ return 0;};

    private:
        FloatVolumeBase a;
        ColorSlider c;
        float cMin;
        float cMax;
};

/*class MultVolumev: public VectorVolume{
    public:
        MultVolumev(VectorVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~MultVolumev(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        Matrix grad( const Vector& P) const{ return a.get()->eval(P) * b.get()->grad(P) + a.get()->grad(P) * b.get()->eval(P);};

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};*/

class Mult_SV_Volume: public VectorVolume{
    public:
        Mult_SV_Volume(FloatVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};

        Mult_SV_Volume( VectorVolumeBase g, FloatVolumeBase f) :
            a(f),
            b(g){};
        ~Mult_SV_Volume(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        Matrix grad( const Vector& P) const{ return b.get()->grad(P);};

    private:
        FloatVolumeBase a;
        VectorVolumeBase b;
};

class Advect_SL_Volume: public FloatVolume{
    public:
        Advect_SL_Volume(FloatVolumeBase f, VectorVolumeBase g, float t) :
            advectTime(t),
            a(f),
            b(g){};

        Advect_SL_Volume( VectorVolumeBase g, FloatVolumeBase f, float t) :
            advectTime(t),
            a(f),
            b(g){};
        Advect_SL_Volume(){};

        float eval( const Vector& P) const{ return a.get()->eval(P - b.get()->eval(P) * advectTime);};
        Vector grad( const Vector& P) const{ return a.get()->grad(P);};

        float advectTime;
    private:
        FloatVolumeBase a;
        VectorVolumeBase b;
};

class Advect_MMC_Volume: public FloatVolume{
    public:
        Advect_MMC_Volume(FloatVolumeBase f, VectorVolumeBase g, const float newTime) :
            a(f),
            b(g),
            advectTime(newTime){};

        Advect_MMC_Volume( VectorVolumeBase g, FloatVolumeBase f, const float newTime) :
            a(f),
            b(g),
            advectTime(newTime){};
        ~Advect_MMC_Volume(){};
        void setTime(const float newTime){ advectTime = newTime;};

        float eval( const Vector& P) const{
            float semiLagrangian = a.get()->eval(P - b.get()->eval(P) * advectTime);
            float errorTerm = a.get()->eval(P) - semiLagrangian;
            return semiLagrangian - errorTerm * 0.5;};
        Vector grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        FloatVolumeBase a;
        VectorVolumeBase b;
        float advectTime;
};


//TODO: Matrix and Vector Multiplication
/*class Mult_VM_Volume: public VectorVolume{
    public:
        Mult_VM_Volume( VectorVolumeBase f, std::shared_ptr<Volume<Matrix> > g) :
            a(f),
            b(g){};
        ~Mult_VM_Volume(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};//TODO
        Matrix grad( const Vector& P) const{ return b.get()->grad(P);};//TODO

    private:
        VectorVolumeBase a;
        std::shared_ptr<Volume<Matrix> > b;
};

class Mult_MV_Volume: public Volume<Matrix>{
    public:
        Mult_MV_Volume( std::shared_ptr<Volume<Matrix> > f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~Mult_MV_Volume(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};//TODO
        Matrix grad( const Vector& P) const{ return b.get()->grad(P);};//TODO: Gradient of matrix does not matter

    private:
        std::shared_ptr<Volume<Matrix> > a;
        VectorVolumeBase b;
};*/

class DivideVolume: public FloatVolume{
    public:
        DivideVolume(FloatVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~DivideVolume(){};

        float eval( const Vector& P) const{ return a.get()->eval(P) / b.get()->eval(P);};
        Vector grad( const Vector& P) const{
            return (a.get()->grad(P) * b.get()->eval(P) - a.get()->eval(P) * b.get()->grad(P)) /
                (b.get()->grad(P) * b.get()->grad(P));};

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
};

class Divide_SV_Volume: public VectorVolume{
    public:
        Divide_SV_Volume(FloatVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~Divide_SV_Volume(){};

        Vector eval( const Vector& P) const{ return b.get()->eval(P) / a.get()->eval(P);};
        Matrix grad( const Vector& P) const{ return b.get()->grad(P);};

    private:
        FloatVolumeBase a;
        VectorVolumeBase b;
};

class ExpVolumef: public FloatVolume{
    public:
        ExpVolumef(FloatVolumeBase f) :
            a(f){};
        ~ExpVolumef(){};

        float eval( const Vector& P) const{ return std::exp(a.get()->eval(P));};
        Vector grad( const Vector& P) const{ return a.get()->grad(P) * eval(P);};

    private:
        FloatVolumeBase a;
};

class ExpVolumev: public VectorVolume{
    public:
        ExpVolumev(VectorVolumeBase f) :
            a(f){};
        ~ExpVolumev(){};

        Vector eval( const Vector& P) const{
            Vector result = a.get()->eval(P);
            result[0] = std::exp(result[0]);
            result[1] = std::exp(result[1]);
            result[2] = std::exp(result[2]);
            return result;};
        Matrix grad( const Vector& P) const{ return /*eval(P) * */a.get()->grad(P);};

    private:
        VectorVolumeBase a;
};

//-------------------------------------------------------------------------------------------------------------------------------
//Union, Intersection, Cutout
//-------------------------------------------------------------------------------------------------------------------------------
class UnionVolumef: public FloatVolume{
    public:
        UnionVolumef(FloatVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~UnionVolumef(){};

        float eval( const Vector& P) const{ return std::max(a.get()->eval(P), b.get()->eval(P));};
        Vector grad( const Vector& P) const{
            if(a.get()->eval(P) > b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
};

class UnionVolumev: public VectorVolume{
    public:
        UnionVolumev(VectorVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~UnionVolumev(){};

        Vector eval( const Vector& P) const{ return std::max(a.get()->eval(P), b.get()->eval(P));};
        Matrix grad( const Vector& P) const{
            if(a.get()->eval(P) > b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};

class IntersectVolumef: public FloatVolume{
    public:
        IntersectVolumef(FloatVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~IntersectVolumef(){};

        float eval( const Vector& P) const{ return std::min(a.get()->eval(P), b.get()->eval(P));};
        Vector grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
};

class IntersectVolumev: public VectorVolume{
    public:
        IntersectVolumev(VectorVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~IntersectVolumev(){};

        Vector eval( const Vector& P) const{ return std::min(a.get()->eval(P), b.get()->eval(P));};
        Matrix grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return b.get()->grad(P);
            }
        };

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};

class CutoutVolumef: public FloatVolume{
    public:
        CutoutVolumef(FloatVolumeBase f, FloatVolumeBase g) :
            a(f),
            b(g){};
        ~CutoutVolumef(){};

        float eval( const Vector& P) const{ return std::min(a.get()->eval(P), -b.get()->eval(P));};
        Vector grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return -b.get()->grad(P);
            }
        };

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
};

class CutoutVolumev: public VectorVolume{
    public:
        CutoutVolumev(VectorVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~CutoutVolumev(){};

        Vector eval( const Vector& P) const{ return std::min(a.get()->eval(P), -b.get()->eval(P));};
        Matrix grad( const Vector& P) const{
            if(a.get()->eval(P) < b.get()->eval(P)){
                return a.get()->grad(P);
            }
            else {
                return -b.get()->grad(P);
            }
        };

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};

//-------------------------------------------------------------------------------------------------------------------------------
class SinVolume: public FloatVolume{
    public:
        SinVolume(FloatVolumeBase f) :
            a(f){};
        ~SinVolume(){};

        float eval( const Vector& P) const{ return std::sin(a.get()->eval(P));};
        Vector grad( const Vector& P) const{ return a.get()->grad(P) * std::cos(a.get()->eval(P));};

    private:
        FloatVolumeBase a;
};

class CosVolume: public FloatVolume{
    public:
        CosVolume(FloatVolumeBase f) :
            a(f){};
        ~CosVolume(){};

        float eval( const Vector& P) const{ return std::cos(a.get()->eval(P));};
        Vector grad( const Vector& P) const{ return a.get()->grad(P) * -std::sin(a.get()->eval(P));};

    private:
        FloatVolumeBase a;
};

//-------------------------------------------------------------------------------------------------------------------------------
//Linear Algebra
//-------------------------------------------------------------------------------------------------------------------------------
class DotProductVolume: public FloatVolume{
    public:
        DotProductVolume(VectorVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~DotProductVolume(){};

        float eval( const Vector& P) const{ return a.get()->eval(P) * b.get()->eval(P);};
        Vector grad( const Vector& P) const{ return Vector(0,0,0);};

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};

class CrossProductVolume: public VectorVolume{
    public:
        CrossProductVolume(VectorVolumeBase f, VectorVolumeBase g) :
            a(f),
            b(g){};
        ~CrossProductVolume(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P) ^ b.get()->eval(P);};
        Matrix grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        VectorVolumeBase a;
        VectorVolumeBase b;
};
//-------------------------------------------------------------------------------------------------------------------------------
//Transformations
//-------------------------------------------------------------------------------------------------------------------------------
class TranslateVolumef: public FloatVolume{
    public:
        TranslateVolumef(FloatVolumeBase f, Vector d) :
            a(f),
            delta(d){};
        ~TranslateVolumef(){};

        float eval( const Vector& P) const{ return a.get()->eval(P - delta);};
        Vector grad( const Vector& P) const{ return a.get()->grad(P - delta);};

    private:
        FloatVolumeBase a;
        Vector delta;
};

class TranslateVolumev: public VectorVolume{
    public:
        TranslateVolumev(VectorVolumeBase f, Vector d) :
            a(f),
            delta(d){};
        ~TranslateVolumev(){};

        Vector eval( const Vector& P) const{ return a.get()->eval(P - delta);};
        Matrix grad( const Vector& P) const{ return a.get()->grad(P - delta);};

    private:
        VectorVolumeBase a;
        Vector delta;
};

class Scale_S_Volume: public FloatVolume{
    public:
        Scale_S_Volume(FloatVolumeBase f, Vector p, float s) :
            a(f),
            pivot(p),
            scaleFactor(s){};
        ~Scale_S_Volume(){};

        float eval( const Vector& P) const{ return a.get()->eval(((P - pivot) / scaleFactor) + pivot);};
        Vector grad( const Vector& P) const{ return a.get()->grad(((P - pivot) / scaleFactor) + pivot);};

    private:
        FloatVolumeBase a;
        Vector pivot;
        float scaleFactor;
};

class Scale_V_Volume: public VectorVolume{
    public:
        Scale_V_Volume(VectorVolumeBase f, Vector p, float s) :
            a(f),
            pivot(p),
            scaleFactor(s){};
        ~Scale_V_Volume(){};

        Vector eval( const Vector& P) const{ return scaleFactor * a.get()->eval(((P - pivot) / scaleFactor) + pivot);};
        Matrix grad( const Vector& P) const{ return a.get()->grad(((P - pivot) / scaleFactor) + pivot);};

    private:
        VectorVolumeBase a;
        Vector pivot;
        float scaleFactor;
};

/*class Scale_M_Volume: public Volume<Matrix>{
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
};*/

class Rotate_S_Volume: public FloatVolume{
    public:
        Rotate_S_Volume(FloatVolumeBase f, Vector p, Vector s, float ang) :
            a(f),
            pivot(p),
            rotAxis(s.unitvector()),
            angle(ang){};
        ~Rotate_S_Volume(){};

        float eval( const Vector& P) const{
            Vector rotatedPoint = P * std::cos(angle);
            rotatedPoint += rotAxis * (P * rotAxis * (1 - std::cos(angle)));
            rotatedPoint += P ^ (rotAxis * std::sin(angle));
            return a.get()->eval(rotatedPoint);
        };
        Vector grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        FloatVolumeBase a;
        Vector pivot;
        Vector rotAxis;
        float angle;
};

class BlinnBlend_S_Volume: public FloatVolume{
    public:
        BlinnBlend_S_Volume(FloatVolumeBase f, FloatVolumeBase g, float Sf, float Sg, float Beta) :
            a(f),
            b(g),
            sf(Sf),
            sg(Sg),
            beta(Beta){};
        ~BlinnBlend_S_Volume(){};

        float eval( const Vector& P) const{
            return std::exp(a.get()->eval(P) / sf) + std::exp(b.get()->eval(P) / sg) - beta;}
        Vector grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        FloatVolumeBase a;
        FloatVolumeBase b;
        float sf;
        float sg;
        float beta;
};


class MaskVolume: public FloatVolume{
    public:
        MaskVolume(FloatVolumeBase f) :
            a(f){};
        ~MaskVolume(){};

        float eval( const Vector& P) const{
            if (a.get()->eval(P) > 0.0) return 1;
            return 0;};
        Vector grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        FloatVolumeBase a;
};

class ReduceVolume: public FloatVolume{
    public:
        ReduceVolume(FloatVolumeBase f, float c) :
            a(f),
            cutoff(c){};
        ~ReduceVolume(){};

        float eval( const Vector& P) const{
            float val = a.get()->eval(P);
            if (val < cutoff) return 0;
            return val;};
        Vector grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        FloatVolumeBase a;
        float cutoff;
};




class ClampVolume: public FloatVolume{
    public:
        ClampVolume(FloatVolumeBase f, float Min, float Max) :
            a(f),
            minVal(Min),
            maxVal(Max){};
        ~ClampVolume(){};

        float eval( const Vector& P) const{
            float val = a.get()->eval(P);
            if (val < minVal) return minVal;
            else if (val > maxVal) return maxVal;
            return val;};
        Vector grad( const Vector& P) const{ return a.get()->grad(P);};

    private:
        FloatVolumeBase a;
        float minVal;
        float maxVal;
};

//End namespace lux
}
#endif
