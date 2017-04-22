
#ifndef __DATAGRID_H__
#define __DATAGRID_H__

#include<iostream>
#include<memory>
namespace lux {
//-----------------------------------------------------------------------------------------------------
//Grid Data Structures
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

}

#endif
