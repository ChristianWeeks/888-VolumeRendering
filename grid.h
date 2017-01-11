#ifndef __GRID_H__
#define __GRID_H__
#include "volume_operators.h"
#include "light.h"
#include <random>

namespace lux {

class FloatGrid{
    public:
        FloatGrid(FloatVolumeBase f, const Vector& c, const double& l, const int& v);
        FloatGrid(const FloatGrid& f);
        ~FloatGrid();
        const float trilinearInterpolate(const Vector& P) const;
        virtual void StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound);
        const Vector center;
        const double length;
    protected:

        FloatVolumeBase field;
        const Vector origin;
        const int voxels;
        const float voxelLength;
        const int totalCells;
        std::unique_ptr<float[]> values;

        const int positionToIndex(const Vector& p) const;
        const Vector indexToPosition(const int i) const;
};

typedef std::shared_ptr<FloatGrid> FloatGridPtr;
//typedef std::shared_ptr<DensityGrid> DensityGridPtr;

class FloatGridBase :  public FloatGridPtr{
    public:
        FloatGridBase(){};
        FloatGridBase(FloatGrid* f) : FloatGridPtr(f){};
        ~FloatGridBase(){};
        FloatGrid* getRef() { return FloatGridPtr::get();};
};

class DensityGrid: public FloatGrid{
    public:
        DensityGrid(FloatVolumeBase f, Vector c, double s, int v);
        DensityGrid(const DensityGrid& f);
        ~DensityGrid(){};

        void StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound);
        //Bake a dot - Used for baking wisps
        int bakeDot(const Vector& p, const float density);

};

class DeepShadowMap: public FloatGrid{
    public:
        DeepShadowMap(light l, float m, FloatVolumeBase f, Vector o, double s, int v);
        ~DeepShadowMap(){};
        light sourceLight;

    private:
        const float marchStep;

        //Raymarch from position x to the light, return the integrated density
        double rayMarchLightScatter(const Vector& x);
};

typedef std::shared_ptr<DeepShadowMap > DSMPtr;

class DSMBase :  public DSMPtr{
    public:
        DSMBase(){};
        DSMBase(DeepShadowMap* f) : DSMPtr(f){};
        ~DSMBase(){};
};

//-------------------------------------------------------------------------------------------------------------------------------
//Gridded Volumes
//-------------------------------------------------------------------------------------------------------------------------------
class GriddedVolume: public FloatVolume{
    public:
        GriddedVolume(FloatGridBase grid) : g(grid){};
        ~GriddedVolume(){};

        const float eval( const Vector& P) const{ return g.get()->trilinearInterpolate(P);};
        const Vector grad( const Vector& P) const{ return Vector(0, 0, 0);};

    private:
        FloatGridBase g;
};
}

class Grid{

    public:
        Grid(int v, int l) : voxels(v), length(l){};
        ~Grid(){};


        void setDefaultValue(float d){defaultValue = d;};

        virtual void init() = 0;
        virtual float get(int index) const = 0;
        virtual void set(int index, float value) = 0;

    private:

        float defaultValue;
        const int voxels;
        const double length;

}

class DenseGrid : public Grid{

        std::unique_ptr<float[]> values;

}

class SparseGrid : public Grid{

}
#endif
