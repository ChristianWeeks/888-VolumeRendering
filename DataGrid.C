#include "DataGrid.h"

using namespace lux;
//-------------------------------------------------------------------------------------------------------------------------------
//Grid Data Structures
//-------------------------------------------------------------------------------------------------------------------------------
SparseGrid::SparseGrid(int xvoxels, int yvoxels, int zvoxels, int p) :
    Grid(xvoxels, yvoxels, zvoxels, p),
    xPartitions(xvoxels / p),
    yPartitions(yvoxels / p),
    zPartitions(zvoxels / p){
    setDefaultValue(0.0);

    data = new float*[xPartitions*yPartitions*zPartitions];
    for(int i = 0; i < xPartitions*yPartitions*zPartitions; i++){
        data[i] = NULL;
    }
    std::cout << "Initializing SparseGrid\n";
}

SparseGrid::~SparseGrid(){
    for (int i = 0; i < xPartitions*yPartitions*zPartitions; i++){
        delete data[i];
    }
};

const float SparseGrid::get(int i, int j, int k) const{
    if(i < 0 || i > xVoxels || j < 0 || j > yVoxels || k < 0 || k > zVoxels){
        return defaultValue;
    }
    int ii = i/partitionSize;
    int jj = j/partitionSize;
    int kk = k/partitionSize;
    //Get our partition index first
    int partitionIndex = kk + jj*zPartitions + ii*yPartitions*zPartitions;
    if(data[partitionIndex] == NULL){
        return defaultValue;
    }
    int iii = (i % partitionSize);
    int jjj = (j % partitionSize);
    int kkk = (k % partitionSize);
    return data[partitionIndex][kkk + partitionSize*(jjj + iii*partitionSize)];
};

void SparseGrid::set(int i, int j, int k, float value){
    if (value == defaultValue) return;
    //ii, jj, kk are our partition indices
    int ii = i/partitionSize;
    int jj = j/partitionSize;
    int kk = k/partitionSize;
    //Get our partition index first
    int partitionIndex = kk + jj*zPartitions + ii*yPartitions*zPartitions;

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
        //After creating block, instantiate a bounding box around the new grid cells
    }
    //Set value now that block definitely exists
    int iii = (i % partitionSize);
    int jjj = (j % partitionSize);
    int kkk = (k % partitionSize);
    data[partitionIndex][kkk + partitionSize*(jjj + iii*partitionSize)] = value;
};

