#include "grid.h"

using namespace lux;

FloatGrid::FloatGrid(std::shared_ptr<Volume<float> > f, Vector o, double s, int v) :
    origin(o),
    size(s),
    voxels(v),
    voxelLength(size / (double)voxels),
    totalCells(v*v*v){
        values = new float[totalCells];
        //stamp the values into our grid
        for(int i = 0; i < voxels; i++){
            for(int j = 0; j < voxels; j++){
                for(int k = 0; k < voxels; k++){

                    //First convert our indices to world space
                    float ii = (float)i * voxelLength + origin[0];
                    float jj = (float)j * voxelLength + origin[1];
                    float kk = (float)k * voxelLength + origin[2];
                    //std::cout << "(" << i << ", " << j << ", " << k << "): " << " (" << ii << ", " << jj << ", " << kk << ")\n";

                    //now evaluate the field at that point
                    values[k + j*voxels + i*voxels*voxels] = f.get()->eval(Vector(ii, jj, kk));

                   // std::cout << values[k + j*voxels + i*voxels*voxels] << "\n";
                }
            }
        }
}

//Converts a position in 3-D space to an index in our grid
const int FloatGrid::positionToIndex(const Vector& P) const{
    Vector P2 = (P - origin) / voxelLength;
    return (int)P2[2] + ((int)P2[1])*voxels + ((int)P2[0])*voxels*voxels;
}

const Vector FloatGrid::indexToPosition(int i) const{
    Vector P;
    P[2] = i % voxels;
    P[1] = (i % (voxels * voxels)) / voxels;
    P[0] = i / (voxels * voxels);
//    P[1] = i %
    return P * voxelLength + origin;
}

const float FloatGrid::trilinearInterpolate(Vector position) const{

    //First we have to find what grid points are around our given position

    //Contains the indices of all the grid points around our position
    int c[8];
    //(-x, -y, -z)
    c[0] = positionToIndex(position);
    //(-x, -y, z)
    c[1] = c[0] -+ 1;
    //(-x, y, -z)
    c[2] = c[0] + voxels;
    //(-x, y, z)
    c[3] = c[2] + 1;
    //(x, -y, -z)
    c[4] = c[0] + (voxels*voxels);
    //(x, -y, z)
    c[5] = c[4] + 1;
    //(x, y, -z)
    c[6] = c[4] + voxels;
    //(x, y, z)
    c[7] = c[6] + 1;
    //check for edge cases.  If its on the edge, we just don't apply at all
    //+x side
    for(int i = 0; i < 8; i++){
        if (c[i] > totalCells || c[i] < 0){
            std::cout << "Out of Bounds: " << c[i] << "\n";
            return 0;
        }
    }
    //now we can actually interpolate
    //d holds values between 0 to 1
    Vector d = (position - indexToPosition(c[0])) / voxelLength;

    //Interpolate along x
    float c00, c10, c01, c11;
    c00 = values[c[0]] * (1-d[0]) + values[c[4]] * d[0];
    c01 = values[c[1]] * (1-d[0]) + values[c[5]] * d[0];
    c10 = values[c[2]] * (1-d[0]) + values[c[6]] * d[0];
    c11 = values[c[3]] * (1-d[0]) + values[c[7]] * d[0];

    //interpolate along y
    float c0, c1;
    c0 = c00 * (1-d[1]) + c10 * d[1];
    c1 = c01 * (1-d[1]) + c11 * d[1];

    //finally, interpolate along z
    c00 = c0 * (1-d[2]) + c1 * d[2];
    //simpler way of checking for edge cases
    //if(vecMagn(c00) > 7.0)
    //    return vField[c[0]];
    //if (c00 > 0) std::cout << c00 << "\n";
    return c00;
}
