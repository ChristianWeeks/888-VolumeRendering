#include "VectorGrid.h"

using namespace lux;

//-------------------------------------------------------------------------------------------------------------------------------
//Vector Grid
//-------------------------------------------------------------------------------------------------------------------------------

//Initialize with a vector field
VectorGrid::VectorGrid(VectorVolumeBase f, const Vector& c, const Vector& s, const int& xv, const int& yv, const int& zv, const int& partitionSize) :
    center(c),
    length(s),
    VectorField(f),
    origin(c - (s/2.0)),
    xVoxels(xv),
    yVoxels(yv),
    zVoxels(zv),
    voxelLength(s[0] / (double)xv),
    totalCells(xv*yv*zv){

    //Make sure voxels are the same size;
    if (s[0] / xv != s[1] / yv || s[0] / xv != s[2] / zv){
        std::cout << "\033[1;31mError: Voxels must be of equal length on every side\033[0m\n";
        return;
    }

    if (partitionSize > 0){
        if ((xv) % partitionSize != 0 || (yv) % partitionSize != 0 || (zv) % partitionSize != 0){
            std::cout << "\033[1;31mError: Voxels must be evenly divisible by partition size\033[0m\n";
            return;
        }
        xGrid = new SparseGrid(xv, yv, zv, partitionSize);
        yGrid = new SparseGrid(xv, yv, zv, partitionSize);
        zGrid = new SparseGrid(xv, yv, zv, partitionSize);
    }
    else{
        xGrid = new DenseGrid(xv, yv, zv);
        yGrid = new DenseGrid(xv, yv, zv);
        zGrid = new DenseGrid(xv, yv, zv);
    }
    initGrid(f);
};

//Initialize with a vector constant
VectorGrid::VectorGrid(const Vector& v, const Vector& c, const Vector& s, const int& xv, const int& yv, const int& zv, const int& partitionSize) :
    center(c),
    length(s),
    VectorField(VectorVolumeBase(new ConstantVolumev(0, 0, 0))),
    origin(c - (s/2.0)),
    xVoxels(xv),
    yVoxels(yv),
    zVoxels(zv),
    voxelLength(s[0] / (double)xv),
    totalCells(xv*yv*zv){

    //Make sure voxels are the same size;
    if (s[0] / xv != s[1] / yv || s[0] / xv != s[2] / zv){
        std::cout << "\033[1;31mError: Voxels must be of equal length on every side\033[0m\n";
        return;
    }

    if (partitionSize > 0){
        if ((xv) % partitionSize != 0 || (yv) % partitionSize != 0 || (zv) % partitionSize != 0){
            std::cout << "\033[1;31mError: Voxels must be evenly divisible by partition size\033[0m\n";
            return;
        }
        xGrid = new SparseGrid(xv, yv, zv, partitionSize);
        yGrid = new SparseGrid(xv, yv, zv, partitionSize);
        zGrid = new SparseGrid(xv, yv, zv, partitionSize);
    }
    else{
        xGrid = new DenseGrid(xv, yv, zv);
        yGrid = new DenseGrid(xv, yv, zv);
        zGrid = new DenseGrid(xv, yv, zv);
    }
    initGrid(v);
};

//Initialize with the gradient of a floatvolumebase
VectorGrid::VectorGrid(FloatVolumeBase f, const Vector& c, const Vector& s, const int& xv, const int& yv, const int& zv, const int& partitionSize) :
    center(c),
    length(s),
    VectorField(VectorVolumeBase(new ConstantVolumev(0, 0, 0))),
    origin(c - (s/2.0)),
    xVoxels(xv),
    yVoxels(yv),
    zVoxels(zv),
    voxelLength(s[0] / (double)xv),
    totalCells(xv*yv*zv){

    //Make sure voxels are the same size;
    if (s[0] / xv != s[1] / yv || s[0] / xv != s[2] / zv){
        std::cout << "\033[1;31mError: Voxels must be of equal length on every side\033[0m\n";
        return;
    }

    if (partitionSize > 0){
        if ((xv) % partitionSize != 0 || (yv) % partitionSize != 0 || (zv) % partitionSize != 0){
            std::cout << "\033[1;31mError: Voxels must be evenly divisible by partition size\033[0m\n";
            return;
        }
        xGrid = new SparseGrid(xv, yv, zv, partitionSize);
        yGrid = new SparseGrid(xv, yv, zv, partitionSize);
        zGrid = new SparseGrid(xv, yv, zv, partitionSize);
    }
    else{
        xGrid = new DenseGrid(xv, yv, zv);
        yGrid = new DenseGrid(xv, yv, zv);
        zGrid = new DenseGrid(xv, yv, zv);
    }

    for(int i = 0; i < xv; i++){
        for(int j = 0; j < yv; j++){
#pragma omp parallel for
            for(int k = 0; k < zv; k++){

                // Simple edge boundary case handling
                if(!i || !j || !k || i == (xv-1) || j == (yv -1) || k == (zv - 1)){
                    xGrid->set(i, j, k, 0.0);
                    yGrid->set(i, j, k, 0.0);
                    zGrid->set(i, j, k, 0.0);
                }
                else{
                    //Finite Difference Gradient Calculation
                    Vector vecVoxelPos = indexToPosition(i, j, k);
                    float xGradient = f.get()->eval(vecVoxelPos + Vector(voxelLength, 0, 0)) - f.get()->eval(vecVoxelPos - Vector(voxelLength, 0, 0));
                    xGradient /= (voxelLength * 2.0);
                    float yGradient = f.get()->eval(vecVoxelPos + Vector(0, voxelLength, 0)) - f.get()->eval(vecVoxelPos - Vector(0, voxelLength, 0));
                    yGradient /= (voxelLength * 2.0);
                    float zGradient = f.get()->eval(vecVoxelPos + Vector(0, 0, voxelLength)) - f.get()->eval(vecVoxelPos - Vector(0, 0, voxelLength));
                    zGradient /= (voxelLength * 2.0);
                    xGrid->set(i, j, k, xGradient);
                    yGrid->set(i, j, k, yGradient);
                    zGrid->set(i, j, k, zGradient);
                }
            }
        }
    }
};

VectorGrid::~VectorGrid(){
    delete xGrid;
    delete yGrid;
    delete zGrid;
}

void VectorGrid::initGrid(VectorVolumeBase initField){

    // Now Initialize the grids
    //stamp the values into our grid
    boost::timer gridTimer;
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
#pragma omp parallel for
            for(int k = 0; k < zVoxels; k++){

                //First convert our indices to world space
                float ii = (float)i * voxelLength + origin[0];
                float jj = (float)j * voxelLength + origin[1];
                float kk = (float)k * voxelLength + origin[2];
                Vector v = initField.get()->eval(Vector(ii, jj, kk));

                //now evaluate the field at that point
                xGrid->set(i, j, k, v[0]);
                yGrid->set(i, j, k, v[1]);
                zGrid->set(i, j, k, v[2]);
            }
        }
    }

}

void VectorGrid::initGrid(const Vector& initVec){

    // Now Initialize the grids
    //stamp the values into our grid
    boost::timer gridTimer;
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
#pragma omp parallel for
            for(int k = 0; k < zVoxels; k++){
                xGrid->set(i, j, k, initVec[0]);
                yGrid->set(i, j, k, initVec[1]);
                zGrid->set(i, j, k, initVec[2]);
            }
        }
    }

}

const Vector VectorGrid::trilinearInterpolate(const Vector& position) const{

    //First we have to find what grid points are around our given position

    //Contains the indices of all the grid points around our position
    Vector indices = positionToIndex(position);
    //If positionToIndex returns a negative value, we are at the border or beyond our grid
    if(!isInGrid(indices)) return Vector(0, 0, 0); 

    int x = indices[0];
    int y = indices[1];
    int z = indices[2];
    Vector worldPos = indexToPosition(x, y, z);

    //now we can actually interpolate
    //d holds values between 0 to 1
    Vector d = (position - worldPos) / getVoxelLength(x, y, z);
    d.clamp(0, 1);

    Vector v;

    //Interpolate along x
    float c00, c10, c01, c11;
    c00 = xGrid->get(x, y, z)    * (1-d[0]) + xGrid->get(x+1, y, z)     * d[0];
    c01 = xGrid->get(x, y, z+1)  * (1-d[0]) + xGrid->get(x+1, y, z+1)   * d[0];
    c10 = xGrid->get(x, y+1, z)  * (1-d[0]) + xGrid->get(x+1, y+1, z)   * d[0];
    c11 = xGrid->get(x, y+1, z+1)* (1-d[0]) + xGrid->get(x+1, y+1, z+1) * d[0];

    //interpolate along y
    float c0, c1;
    c0 = c00 * (1-d[1]) + c10 * d[1];
    c1 = c01 * (1-d[1]) + c11 * d[1];

    //finally, interpolate along z
    c00 = c0 * (1-d[2]) + c1 * d[2];
    v[0] = c00;

    //Repeat for y and z dimensions
    c00 = yGrid->get(x, y, z)    * (1-d[0]) + yGrid->get(x+1, y, z)     * d[0];
    c01 = yGrid->get(x, y, z+1)  * (1-d[0]) + yGrid->get(x+1, y, z+1)   * d[0];
    c10 = yGrid->get(x, y+1, z)  * (1-d[0]) + yGrid->get(x+1, y+1, z)   * d[0];
    c11 = yGrid->get(x, y+1, z+1)* (1-d[0]) + yGrid->get(x+1, y+1, z+1) * d[0];

    c0 = c00 * (1-d[1]) + c10 * d[1];
    c1 = c01 * (1-d[1]) + c11 * d[1];

    c00 = c0 * (1-d[2]) + c1 * d[2];
    v[1] = c00;

    //Interpolate along x
    c00 = zGrid->get(x, y, z)    * (1-d[0]) + zGrid->get(x+1, y, z)     * d[0];
    c01 = zGrid->get(x, y, z+1)  * (1-d[0]) + zGrid->get(x+1, y, z+1)   * d[0];
    c10 = zGrid->get(x, y+1, z)  * (1-d[0]) + zGrid->get(x+1, y+1, z)   * d[0];
    c11 = zGrid->get(x, y+1, z+1)* (1-d[0]) + zGrid->get(x+1, y+1, z+1) * d[0];

    c0 = c00 * (1-d[1]) + c10 * d[1];
    c1 = c01 * (1-d[1]) + c11 * d[1];

    c00 = c0 * (1-d[2]) + c1 * d[2];
    v[2] = c00;

    return v;
}

inline const float VectorGrid::getVoxelLength(const int x, const int y, const int z) const{
    return voxelLength;
}

//Converts a position in 3-D space to an index in our grid
const Vector VectorGrid::positionToIndex(const Vector& P) const{
    Vector P2 = (P - origin) / voxelLength;
    P2[0] = int(P2[0]);
    P2[1] = int(P2[1]);
    P2[2] = int(P2[2]);
    return P2;
}

const Vector VectorGrid::indexToPosition(int i, int j, int k) const{
    Vector P(i, j, k);
    return P * voxelLength + origin;
}

const int VectorGrid::isInGrid(int x, int y, int z) const{
    if (x < 0 || y < 0 || z < 0 || x >= (xVoxels - 1) || y >= (yVoxels - 1) || z >= (zVoxels - 1))
        return 0;
    return 1;
}

const int VectorGrid::isInGrid(const Vector& x) const{
    if (x[0] < 0 || x[1] < 0 || x[2] < 0 || x[0] >= (xVoxels - 1) || x[1] >= (yVoxels - 1) || x[2] >= (zVoxels - 1))
        return 0;
    return 1;
}
