#include "volume_operators.h"
#include "light.h"
#include <random>

namespace lux {
class FloatGrid{
    public:
        FloatGrid(std::shared_ptr<Volume<float> > f, Vector o, double s, int v);
        FloatGrid(const FloatGrid& f);
        ~FloatGrid();
        const float trilinearInterpolate(const Vector& P) const;
    protected:

        std::shared_ptr<Volume<float> > field;
        const Vector origin;
        const double size;
        const int voxels;
        const float voxelLength;
        const int totalCells;
        std::unique_ptr<float[]> values;

        const int positionToIndex(const Vector& p) const;
        const Vector indexToPosition(const int i) const;
};

class DensityGrid: public FloatGrid{
    public:
        DensityGrid(std::shared_ptr<Volume<float> > f, Vector o, double s, int v);
        ~DensityGrid(){};

        void StampWisp(const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset);
        //Bake a dot - Used for baking wisps
        int bakeDot(const Vector& p, const float density);

};

class DeepShadowMap: public FloatGrid{
    public:
        DeepShadowMap(light l, float m, std::shared_ptr<Volume<float> > f, Vector o, double s, int v);
        ~DeepShadowMap(){};

    private:
        light sourceLight;
        const float marchStep;

        //Raymarch from position x to the light, return the integrated density
        double rayMarchLightScatter(const Vector& x);
};
//-------------------------------------------------------------------------------------------------------------------------------
//Gridded Volumes
//-------------------------------------------------------------------------------------------------------------------------------
class GriddedVolume: public Volume<float>{
    public:
        GriddedVolume(const std::shared_ptr<FloatGrid> grid) : g(grid){};
        ~GriddedVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return g.get()->trilinearInterpolate(P);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return Vector(0, 0, 0);};

    private:
        std::shared_ptr<FloatGrid> g;
};
}
