#ifndef __GRID_H__
#define __GRID_H__
#include "volume_operators.h"
#include "light.h"
#include <random>

namespace lux {

class FloatGrid{
    public:
        FloatGrid(FloatVolumeBase f, const Vector& o, const double& s, const int& v);
        FloatGrid(const FloatGrid& f);
        ~FloatGrid();
        const float trilinearInterpolate(const Vector& P) const;
    protected:

        FloatVolumeBase field;
        const Vector origin;
        const double size;
        const int voxels;
        const float voxelLength;
        const int totalCells;
        std::unique_ptr<float[]> values;

        const int positionToIndex(const Vector& p) const;
        const Vector indexToPosition(const int i) const;
};

typedef std::shared_ptr<FloatGrid> FloatGridPtr;

class FloatGridBase :  public FloatGridPtr{
    public:
        FloatGridBase(){};
        FloatGridBase(FloatGrid* f) : FloatGridPtr(f){};
        ~FloatGridBase(){};
};

class DensityGrid: public FloatGrid{
    public:
        DensityGrid(FloatVolumeBase f, Vector o, double s, int v);
        ~DensityGrid(){};

        void StampWisp(const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset);
        //Bake a dot - Used for baking wisps
        int bakeDot(const Vector& p, const float density);

};

class DeepShadowMap: public FloatGrid{
    public:
        DeepShadowMap(light l, float m, FloatVolume* f, Vector o, double s, int v);
        ~DeepShadowMap(){};

    private:
        light sourceLight;
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
        GriddedVolume(FloatGrid* grid) : g(grid){};
        ~GriddedVolume(){};

        const float eval( const Vector& P) const{ return g.get()->trilinearInterpolate(P);};
        const Vector grad( const Vector& P) const{ return Vector(0, 0, 0);};

    private:
        FloatGridBase g;
};
}
#endif
