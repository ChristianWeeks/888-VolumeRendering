#ifndef __VECTORGRID_H__
#define __VECTORGRID_H__
#include <string>
#include <boost/timer.hpp>
#include <random>
#include <omp.h>

#include "volume_operators.h"
#include "DataGrid.h"

namespace lux {

class VectorGrid{
    public:
        VectorGrid(VectorVolumeBase f, const Vector& c, const Vector& s, const int& vx, const int& vy, const int& vz, const int& partitionSize);
        VectorGrid(const Vector& v, const Vector& c, const Vector& s, const int& vx, const int& vy, const int& vz, const int& partitionSize);
        //Passing a floatvolumebase will auto calculate the gradient
        VectorGrid(FloatVolumeBase f, const Vector& c, const Vector& s, const int& vx, const int& vy, const int& vz, const int& partitionSize);

        //VectorGrid(const VectorGrid& f);
        ~VectorGrid();

        virtual const float getVoxelLength(const int x, const int y, const int z) const;
        const Vector trilinearInterpolate(const Vector& P) const;
        //virtual void StampField(const FloatVolumeBase& f, const BoundingBox& AABB, int operand);

        const Vector center;
        const Vector length;

    protected:

        VectorVolumeBase VectorField;
        Grid *xGrid;
        Grid *yGrid;
        Grid *zGrid;
        const Vector origin;
        const int xVoxels;
        const int yVoxels;
        const int zVoxels;
        const float voxelLength;
        const int totalCells;

        void initGrid(VectorVolumeBase v);
        void initGrid(const Vector& v);
        virtual const Vector positionToIndex(const Vector& P) const;
        virtual const Vector indexToPosition(const int i, const int j, const int k) const;
        const int isInGrid(int x, int y, int z) const;
        const int isInGrid(const Vector& x) const;
};

typedef std::shared_ptr<VectorGrid> VectorGridPtr;

class VectorGridBase :  public VectorGridPtr{
    public:
        VectorGridBase(){};
        VectorGridBase(VectorGrid* f) : VectorGridPtr(f){};
        ~VectorGridBase(){};
        VectorGrid* getRef() { return VectorGridPtr::get();};
};

//-------------------------------------------------------------------------------------------------------------------------------
//Gridded Vector Volumes
//-------------------------------------------------------------------------------------------------------------------------------
class GriddedVolumev: public VectorVolume{
    public:
        GriddedVolumev(VectorGridBase grid) : g(grid){};
        ~GriddedVolumev(){};

        Vector eval( const Vector& P) const{ return g.get()->trilinearInterpolate(P);};
        Matrix grad( const Vector& P) const{ Matrix G; return G;};

    private:
        VectorGridBase g;
};
}

#endif
