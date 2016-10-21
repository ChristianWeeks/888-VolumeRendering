#include "volume_operators.h"

namespace lux {
class FloatGrid{
    public:
        FloatGrid(std::shared_ptr<Volume<float> > f, Vector o, double s, int v);
        ~FloatGrid(){};
        const float trilinearInterpolate(Vector P) const;
        int bakeDensity(const Vector& p, const float density);

    private:

        float *values;
        const Vector origin;
        const double size;
        const int voxels;
        const float voxelLength;
        const int totalCells;

        const int positionToIndex(const Vector& p) const;
        const Vector indexToPosition(const int i) const;
};

//-------------------------------------------------------------------------------------------------------------------------------
//Gridded Volumes
//-------------------------------------------------------------------------------------------------------------------------------
class GriddedVolume: public Volume<float>{
    public:
        GriddedVolume(FloatGrid grid) : g(grid){};
        ~GriddedVolume(){};

        const typename Volume<float>::volumeDataType eval( const Vector& P) const{ return g.trilinearInterpolate(P);};
        const typename Volume<float>::volumeGradType grad( const Vector& P) const{ return Vector(0, 0, 0);};

    private:
        FloatGrid g;
};
}
