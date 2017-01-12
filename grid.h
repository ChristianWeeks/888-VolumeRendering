#ifndef __GRID_H__
#define __GRID_H__
#include "volume_operators.h"
#include "light.h"
#include <random>

namespace lux {

//-----------------------------------------------------------------------------------------------------
//Grid Data Structes
//-----------------------------------------------------------------------------------------------------
class Grid{
    public:
        Grid(int v, float l, int p) : partitionSize(p), voxels(v), length(l){};
        virtual ~Grid(){};

        void setDefaultValue(float d){defaultValue = d;};

        //virtual void init() = 0;
        virtual const float get(int i, int j, int k) const = 0;
        virtual void set(int i, int j, int k, float value) = 0;

        const int partitionSize;

    protected:
        float defaultValue;
        const int voxels;
        const double length;

};

class DenseGrid : public Grid{
    public:
        DenseGrid(int v, float l) : Grid(v, l, -1), data(new float[v*v*v]){};
        ~DenseGrid(){};

        const float get(int i, int j, int k) const{ return data.get()[k + j*voxels + i*voxels*voxels];};
        void set(int i, int j, int k, float value){ data.get()[k + j*voxels + i*voxels*voxels] = value;};

    private:
        std::unique_ptr<float[]> data;

};

class SparseGrid : public Grid{
    public:
        SparseGrid(int v, float l, int p) : Grid(v, l, p), numPartitions(v / p){
            try{
                if ((v) % partitionSize != 0)
                    throw 10;
            }
            catch(int e){
                std::cout << "Error: Voxels must be evenly divisible by partition size\n";
            }
            setDefaultValue(0.0);

            data = new float*[numPartitions*numPartitions*numPartitions];
            for(int i = 0; i < numPartitions*numPartitions*numPartitions; i++){
                data[i] = NULL;
            }

        };
        ~SparseGrid(){
            for (int i = 0; i < numPartitions; i++){
                delete data[i];
            }
        };

        const float get(int i, int j, int k) const{
            int ii = i/partitionSize;
            int jj = j/partitionSize;
            int kk = k/partitionSize;
            //Get our partition index first
            int partitionIndex = kk + numPartitions * (jj + numPartitions * ii);
            int iii = (i % partitionSize);
            int jjj = (j % partitionSize);
            int kkk = (k % partitionSize);
            if(data[partitionIndex] == NULL){
                return defaultValue;
            }
            return data[partitionIndex][kkk + partitionSize*(jjj + iii*partitionSize)];
        };

        void set(int i, int j, int k, float value){
            if (value == defaultValue) return;
            //ii, jj, kk are our partition indices
            int ii = i/partitionSize;
            int jj = j/partitionSize;
            int kk = k/partitionSize;
            //Get our partition index first
            int partitionIndex = kk + numPartitions * (jj + numPartitions * ii);

            if (data[partitionIndex] == NULL){
                //if block does not exist, we must create and initialize it
                data[partitionIndex] = new float[partitionSize*partitionSize*partitionSize];
                for (int iii = 0; iii < partitionSize; iii++){
                    for (int jjj = 0; jjj < partitionSize; jjj++){
                        for (int kkk = 0; kkk < partitionSize; kkk++){
                            data[partitionIndex][kkk +  partitionSize*(jjj + iii*partitionSize)] = defaultValue;
                        }
                    }
                }
            }
            //Set value now that block definitely exists
            int iii = (i % partitionSize);
            int jjj = (j % partitionSize);
            int kkk = (k % partitionSize);
            data[partitionIndex][kkk + partitionSize*(jjj + iii*partitionSize)] = value;
        };

    private:
        const int numPartitions;
        float **data;

};

//-----------------------------------------------------------------------------------------------------
//Grid Objects
//-----------------------------------------------------------------------------------------------------
class FloatGrid{
    public:
        FloatGrid(FloatVolumeBase f, const Vector& c, const double& l, const int& v, const int& partitionSize);
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
        DensityGrid(FloatVolumeBase f, Vector c, double s, int v, int p);
        DensityGrid(const DensityGrid& f);
        ~DensityGrid(){};

        void StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound);
        //Bake a dot - Used for baking wisps
        int bakeDot(const Vector& p, const float density);

};

class DeepShadowMap: public FloatGrid{
    public:
        DeepShadowMap(light l, float m, FloatVolumeBase f, Vector o, double s, int v, int p);
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
