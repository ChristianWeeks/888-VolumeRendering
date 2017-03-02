#ifndef __GRID_H__
#define __GRID_H__
#include "volume_operators.h"
#include "light.h"
#include "Camera.h"
#include "boundingbox.h"
#include "vdbTypes.h"
#include <openvdb/openvdb.h>
#include <string>
#include <boost/timer.hpp>
#include <random>
#include <omp.h>

namespace lux {

//-----------------------------------------------------------------------------------------------------
//Grid Data Structes
//-----------------------------------------------------------------------------------------------------
class Grid{
    public:
        Grid(int xvoxels, int yvoxels, int zvoxels, int p) : 
            partitionSize(p), 
            xVoxels(xvoxels),
            yVoxels(yvoxels),
            zVoxels(zvoxels){};
        virtual ~Grid(){};

        void setDefaultValue(float d){defaultValue = d;};

        //virtual void init() = 0;
        virtual const float get(int i, int j, int k) const = 0;
        virtual void set(int i, int j, int k, float value) = 0;
        virtual int isAllocated(int i) const{return 1;};

        const int partitionSize;

    protected:
        float defaultValue;
        const int xVoxels;
        const int yVoxels;
        const int zVoxels;

};

class DenseGrid : public Grid{
    public:
        DenseGrid(int xvoxels, int yvoxels, int zvoxels) :
            Grid(xvoxels, yvoxels, zvoxels, -1), data(new float[xvoxels*yvoxels*zvoxels]){};
        ~DenseGrid(){};

        const float get(int i, int j, int k) const{ return data.get()[k + j*zVoxels + i*yVoxels*zVoxels];};
        void set(int i, int j, int k, float value){ data.get()[k + j*zVoxels + i*yVoxels*zVoxels] = value;};

    private:
        std::unique_ptr<float[]> data;

};

class SparseGrid : public Grid{
    public:
        SparseGrid(int xvoxels, int yvoxels, int zvoxels, int p);
        ~SparseGrid();

        const float get(int i, int j, int k) const;
        void set(int i, int j, int k, float value);
        int isAllocated(int i) const{
            if(data[i] == NULL)
                return 0;
            return 1;
        }

        const int xPartitions;
        const int yPartitions;
        const int zPartitions;
    private:
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

        virtual const float getVoxelLength(const int x, const int y, const int z) const;
        const float trilinearInterpolate(const Vector& P) const;
        void StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound, const Vector& normal, int numSteps, float streakLength);
        virtual void StampField(const FloatVolumeBase& f, const BoundingBox& AABB, int operand);
        virtual void StampvdbLevelSet(const std::string filename, const std::string levelsetName, const BoundingBox& AABB, int operand);
        virtual void createBoundingBoxes();

        const Vector center;
        const Vector length;

        float stampXMin;
        float stampXMax;
        float stampYMin;
        float stampYMax;
        float stampZMin;
        float stampZMax;
        std::vector<BoundingBox> gridBBs;
    protected:

        FloatVolumeBase field;
        const Vector origin;
        const int xVoxels;
        const int yVoxels;
        const int zVoxels;
        const float voxelLength;
        const int totalCells;
        Grid *data;

        virtual const Vector positionToIndex(const Vector& P) const;
        virtual const Vector indexToPosition(const int i, const int j, const int k) const;
        const int isInGrid(int x, int y, int z) const;
        const int isInGrid(const Vector& x) const;
        int bakeDot(const Vector& p, const float density);
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

class FrustumGrid: public FloatGrid{
    public:
        FrustumGrid(float initValue, const Camera& cam, int vx, int vy, int vz, int p);
        FrustumGrid(FloatVolumeBase f, const Camera& cam, int vx, int vy, int vz, int p);
        ~FrustumGrid(){}
        void StampField(const FloatVolumeBase& f, const BoundingBox& AABB, int operand);
        void createBoundingBoxes(){};

    protected:
        const float getVoxelLength(const int x, const int y, const int z) const;
        const Vector positionToIndex(const Vector& P) const;
        const Vector indexToPosition(const int i, const int j, const int k) const;
        Camera camera;
        float zVoxelLength;
};

class DensityGrid: public FloatGrid{
    public:
        DensityGrid(FloatVolumeBase f, Vector c, const Vector& s, int xv, int yv, int zv, int p);
        DensityGrid(float f, Vector c, const Vector& s, int xv, int yv, int zv, int p);
        //DensityGrid(const DensityGrid& f);
        ~DensityGrid(){};
};

class DeepShadowMap: public FloatGrid{
    public:
        DeepShadowMap(light l, float m, FloatVolumeBase f, Vector o, const Vector& s, int vx, int vy, int vz, int p, std::vector<BoundingBox> bbs);
        ~DeepShadowMap(){};
        light sourceLight;

    private:
        const float marchStep;
        std::vector<BoundingBox> boundingboxes;

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

        float eval( const Vector& P) const{ return g.get()->trilinearInterpolate(P);};
        Vector grad( const Vector& P) const{ return Vector(0, 0, 0);};

    private:
        FloatGridBase g;
};
}

#endif
