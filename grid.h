#ifndef __GRID_H__
#define __GRID_H__
#include "volume_operators.h"
#include "light.h"
#include <boost/timer.hpp>
#include <random>

namespace lux {

//-----------------------------------------------------------------------------------------------------
//Grid Data Structes
//-----------------------------------------------------------------------------------------------------
class Grid{
    public:
        Grid(int xvoxels, int yvoxels, int zvoxels, float xlength, float ylength, float zlength, int p) : 
            partitionSize(p), 
            xVoxels(xvoxels),
            yVoxels(yvoxels),
            zVoxels(zvoxels),
            xLength(xlength),
            yLength(ylength),
            zLength(zlength){};
        virtual ~Grid(){};

        void setDefaultValue(float d){defaultValue = d;};

        //virtual void init() = 0;
        virtual const float get(int i, int j, int k) const = 0;
        virtual void set(int i, int j, int k, float value) = 0;

        const int partitionSize;

    protected:
        float defaultValue;
        const int xVoxels;
        const int yVoxels;
        const int zVoxels;
        const double xLength;
        const double yLength;
        const double zLength;

};

class DenseGrid : public Grid{
    public:
        DenseGrid(int xvoxels, int yvoxels, int zvoxels, float xlength, float ylength, float zlength) :
            Grid(xvoxels, yvoxels, zvoxels, xlength, ylength, zlength, -1), data(new float[xvoxels*yvoxels*zvoxels]){};
        ~DenseGrid(){};

        const float get(int i, int j, int k) const{ return data.get()[k + j*zVoxels + i*yVoxels*zVoxels];};
        void set(int i, int j, int k, float value){ data.get()[k + j*zVoxels + i*yVoxels*zVoxels] = value;};

    private:
        std::unique_ptr<float[]> data;

};

class SparseGrid : public Grid{
    public:
        SparseGrid(int xvoxels, int yvoxels, int zvoxels, float xlength, float ylength, float zlength, int p);
        ~SparseGrid();

        const float get(int i, int j, int k) const;
        void set(int i, int j, int k, float value);

    private:
        const int xPartitions;
        const int yPartitions;
        const int zPartitions;
        float **data;

};

//-----------------------------------------------------------------------------------------------------
//Grid Objects
//-----------------------------------------------------------------------------------------------------
class FloatGrid{
    public:
        FloatGrid(FloatVolumeBase f, const Vector& c, const Vector& s, const int& vx, const int& vy, const int& vz, const int& partitionSize);
        //FloatGrid(const FloatGrid& f);
        ~FloatGrid();
        const float trilinearInterpolate(const Vector& P) const;
        virtual void StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound);
        void StampField(const FloatVolumeBase& f, const BoundingBox& AABB, int operand);

        const Vector center;
        const Vector length;
    protected:

        FloatVolumeBase field;
        const Vector origin;
        const int xVoxels;
        const int yVoxels;
        const int zVoxels;
        const float voxelLength;
        const int totalCells;
        Grid *data;

        const Vector positionToIndex(const Vector& P) const;
        const Vector indexToPosition(const int i, const int j, const int k) const;
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
        DensityGrid(FloatVolumeBase f, Vector c, const Vector& s, int xv, int yv, int zv, int p);
        //DensityGrid(const DensityGrid& f);
        ~DensityGrid(){};

        void StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound);
        //Bake a dot - Used for baking wisps
        int bakeDot(const Vector& p, const float density);

};

class DeepShadowMap: public FloatGrid{
    public:
        DeepShadowMap(light l, float m, FloatVolumeBase f, Vector o, const Vector& s, int vx, int vy, int vz, int p);
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

#endif
