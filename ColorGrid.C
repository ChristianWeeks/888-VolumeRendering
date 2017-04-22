#include "ColorGrid.h"
using namespace lux;

//-------------------------------------------------------------------------------------------------------------------------------
//floatgrid
//-------------------------------------------------------------------------------------------------------------------------------
ColorGrid::ColorGrid(ColorVolumeBase f, const Vector& c, const Vector& s, const int& xv, const int& yv, const int& zv, const int& partitionSize) :
    center(c),
    length(s),
    stampXMin(-1),
    stampXMax(1),
    stampYMin(-1),
    stampYMax(1),
    stampZMin(-1),
    stampZMax(1),
    ColorField(f),
    origin(c - (s/2.0)),
    xVoxels(xv),
    yVoxels(yv),
    zVoxels(zv),
    voxelLength(s[0] / (double)xv),
    totalCells(xv*yv*zv){

    if (partitionSize > 0){
        if ((xv) % partitionSize != 0 || (yv) % partitionSize != 0 || (zv) % partitionSize != 0){
            std::cout << "\033[1;31mError: Voxels must be evenly divisible by partition size\033[0m\n";
            return;
        }
        red = new SparseGrid(xv, yv, zv, partitionSize);
        green = new SparseGrid(xv, yv, zv, partitionSize);
        blue = new SparseGrid(xv, yv, zv, partitionSize);
    }
    else{
        red = new DenseGrid(xv, yv, zv);
        green = new DenseGrid(xv, yv, zv);
        blue = new DenseGrid(xv, yv, zv);
    }

    std::cout << "------------------------------------------------------\n";
    std::cout << "Initializing Color Grid\n";
    boost::timer gridTimer;
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
#pragma omp parallel for
            for(int k = 0; k < zVoxels; k++){

                Color c ColorField.get()->eval(Vector(ii, jj, kk));

                //First convert our indices to world space
                float ii = (float)i * voxelLength + origin[0];
                float jj = (float)j * voxelLength + origin[1];
                float kk = (float)k * voxelLength + origin[2];
                //std::cout << "(" << i << ", " << j << ", " << k << "): " << " (" << ii << ", " << jj << ", " << kk << ")\n";

                //now evaluate the field at that point
                red->set(i, j, k, c[0]);
                green->set(i, j, k, c[1]);
                blue->set(i, j, k, c[2]);

               // std::cout << values.get()[k + j*zVoxels + i*yVoxels*zVoxels] << "\n";
            }
        }
    }
    std::cout << "Color Grid Initialized in: " << gridTimer.elapsed() <<" seconds\n";
    std::cout << "------------------------------------------------------\n";


};

inline const float ColorGrid::getVoxelLength(const int x, const int y, const int z) const{
    return voxelLength;
}

ColorGrid::~ColorGrid(){
    delete data;
};

//Converts a position in 3-D space to an index in our grid
const Vector ColorGrid::positionToIndex(const Vector& P) const{
    Vector P2 = (P - origin) / voxelLength;
    P2[0] = int(P2[0]);
    P2[1] = int(P2[1]);
    P2[2] = int(P2[2]);
    return P2;
}

const Vector ColorGrid::indexToPosition(int i, int j, int k) const{
    Vector P(i, j, k);
    return P * voxelLength + origin;
}

const int ColorGrid::isInGrid(int x, int y, int z) const{
    if (x < 0 || y < 0 || z < 0 || x >= (xVoxels - 1) || y >= (yVoxels - 1) || z >= (zVoxels - 1))
        return 0;
    return 1;
}

const int ColorGrid::isInGrid(const Vector& x) const{
    if (x[0] < 0 || x[1] < 0 || x[2] < 0 || x[0] >= (xVoxels - 1) || x[1] >= (yVoxels - 1) || x[2] >= (zVoxels - 1))
        return 0;
    return 1;
}
