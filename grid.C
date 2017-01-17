#include "grid.h"

using namespace lux;

//-------------------------------------------------------------------------------------------------------------------------------
//Grid Data Structures
//-------------------------------------------------------------------------------------------------------------------------------
DenseGrid::DenseGrid(int xvoxels, int yvoxels, int zvoxels, float xlength, float ylength, float zlength) :
    Grid(xvoxels, yvoxels, zvoxels, xlength, ylength, zlength, -1), data(new float[xvoxels*yvoxels*zvoxels]){};
DenseGrid::~DenseGrid(){};

inline const float DenseGrid::get(int i, int j, int k) const{ return data.get()[k + j*zVoxels + i*yVoxels*zVoxels];};
inline void DenseGrid::set(int i, int j, int k, float value){ data.get()[k + j*zVoxels + i*yVoxels*zVoxels] = value;};

SparseGrid::SparseGrid(int xvoxels, int yvoxels, int zvoxels, float xlength, float ylength, float zlength, int p) :
    Grid(xvoxels, yvoxels, zvoxels, xlength, ylength, zlength, p),
    xPartitions(xvoxels / p),
    yPartitions(yvoxels / p),
    zPartitions(zvoxels / p){
    try{
        if ((xvoxels) % partitionSize != 0 || (yvoxels) % partitionSize != 0 || (zvoxels) % partitionSize != 0){
            throw 10;
        }
        if (xlength / xvoxels != ylength / yvoxels || xlength / xvoxels != zlength / zvoxels){
            throw 20;
        }

    }
    catch(int e){
        if (e == 10){
            std::cout << "Error: Voxels must be evenly divisible by partition size\n";
        }
        else if (e == 20){
            std::cout << "Error: Voxels must be of equal length on every side\n";
        }
    }
    setDefaultValue(0.0);

    data = new float*[xPartitions*yPartitions*zPartitions];
    for(int i = 0; i < xPartitions*yPartitions*zPartitions; i++){
        data[i] = NULL;
    }
}

SparseGrid::~SparseGrid(){
    for (int i = 0; i < xPartitions*yPartitions*zPartitions; i++){
        delete data[i];
    }
};

const float SparseGrid::get(int i, int j, int k) const{
    int ii = i/partitionSize;
    int jj = j/partitionSize;
    int kk = k/partitionSize;
    //Get our partition index first
    int partitionIndex = kk + jj*zPartitions + ii*yPartitions*zPartitions;
    int iii = (i % partitionSize);
    int jjj = (j % partitionSize);
    int kkk = (k % partitionSize);
    if(data[partitionIndex] == NULL){
        return defaultValue;
    }
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
    }
    //Set value now that block definitely exists
    int iii = (i % partitionSize);
    int jjj = (j % partitionSize);
    int kkk = (k % partitionSize);
    data[partitionIndex][kkk + partitionSize*(jjj + iii*partitionSize)] = value;
};

//-------------------------------------------------------------------------------------------------------------------------------
//floatgrid
//-------------------------------------------------------------------------------------------------------------------------------
FloatGrid::FloatGrid(FloatVolumeBase f, const Vector& c, const Vector& s, const int& xv, const int& yv, const int& zv, const int& partitionSize) :
    center(c),
    length(s),
    field(f),
    origin(c - (s/2.0)),
    xVoxels(xv),
    yVoxels(yv),
    zVoxels(zv),
    voxelLength(s[0] / (double)xv),
    totalCells(xv*yv*zv){

    if (partitionSize > 0){
        data = new SparseGrid(xv, yv, zv, s[0], s[1], s[2], partitionSize);
    }
    else{
        data = new DenseGrid(xv, yv,zv, s[0], s[1], s[2]);
    }
};

/*FloatGrid::FloatGrid(const FloatGrid& f) :
    center(f.center),
    length(f.length),
    field(f.field),
    origin(f.origin),
    voxels(f.voxels),
    voxelLength(f.voxelLength),
    totalCells(f.totalCells){
    std::cout << "FloatGrid Copy Constructor!\n";

    if (f.data->partitionSize > 0){
        data = new SparseGrid(voxels, length, f.data->partitionSize);
    }
    else{
        data = new DenseGrid(voxels, length);
    }

    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
            for(int k = 0; k < zVoxels; k++){
            data->set(i, j, k, f.data->get(i, j, k));
            }
        }
    }
};*/

FloatGrid::~FloatGrid(){
    delete data;
};

void FloatGrid::StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound){std::cout << "FloatGrid can't stamp wisps! Use a density grid.\n";};

//Converts a position in 3-D space to an index in our grid
const Vector FloatGrid::positionToIndex(const Vector& P) const{
    Vector P2 = (P - origin) / voxelLength;
    P2[0] = int(P2[0]);
    P2[1] = int(P2[1]);
    P2[2] = int(P2[2]);
    if(P2[0] <= 1 || P2[0] >= (xVoxels-1) || P2[1] <= 1 || P2[1] >= (yVoxels-1) || P2[2] <= 1 || P2[2] >= (zVoxels-1)){
        return Vector(-1, -1, -1);
    }
    return Vector(P2[0], P2[1], P2[2]);
}

const Vector FloatGrid::indexToPosition(int i, int j, int k) const{
    Vector P(i, j, k);
    return P * voxelLength + origin;
}

const float FloatGrid::trilinearInterpolate(const Vector& position) const{

    //First we have to find what grid points are around our given position

    //Contains the indices of all the grid points around our position
    Vector indices = positionToIndex(position);
    //If positionToIndex returns a negative value, we are at the border or beyond our grid
    if (indices[0] == -1)
        return 0.0;

    int x = indices[0];
    int y = indices[1];
    int z = indices[2];
    //now we can actually interpolate
    //d holds values between 0 to 1
    Vector d = (position - (indices * voxelLength + origin)) / voxelLength;

    //Interpolate along x
    float c00, c10, c01, c11;
    c00 = data->get(x, y, z)    * (1-d[0]) + data->get(x+1, y, z)     * d[0];
    c01 = data->get(x, y, z+1)  * (1-d[0]) + data->get(x+1, y, z+1)   * d[0];
    c10 = data->get(x, y+1, z)  * (1-d[0]) + data->get(x+1, y+1, z)   * d[0];
    c11 = data->get(x, y+1, z+1)* (1-d[0]) + data->get(x+1, y+1, z+1) * d[0];

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
    /*if(c00 > 4.0){
        std::cout << "c00: " << c00 << "\n";
        std::cout << "Pos: " << position << "\n";
        std::cout << "Index: " << c[0] << "\n";
    }*/
    return c00;
}

//-------------------------------------------------------------------------------------------------------------------------------
//DensityGrid
//-------------------------------------------------------------------------------------------------------------------------------
DensityGrid::DensityGrid(FloatVolumeBase f, Vector o, const Vector& s, int vx, int vy, int vz, int p)
    : FloatGrid(f, o, s, vx, vy, vz, p){
    //stamp the values into our grid
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
            for(int k = 0; k < zVoxels; k++){

                //First convert our indices to world space
                float ii = (float)i * voxelLength + origin[0];
                float jj = (float)j * voxelLength + origin[1];
                float kk = (float)k * voxelLength + origin[2];
                //std::cout << "(" << i << ", " << j << ", " << k << "): " << " (" << ii << ", " << jj << ", " << kk << ")\n";

                //now evaluate the field at that point
                data->set(i, j, k, field.get()->eval(Vector(ii, jj, kk)));

               // std::cout << values.get()[k + j*zVoxels + i*yVoxels*zVoxels] << "\n";
            }
        }
    }
}

//DensityGrid::DensityGrid(const DensityGrid& f) : FloatGrid(f.field, f.origin, f.length, f.voxels, f.data->partitionSize){std::cout << "Density Grid Copy Constructor!\n";};

//Wisp algorithm
void DensityGrid::StampWisp(float value, const Vector& P, const SimplexNoiseObject& noise1, const SimplexNoiseObject& noise2, float clump, float radius, float numDots, float offset, float dBound){

    std::random_device rnd;
    std::mt19937 rng(rnd());
    std::uniform_real_distribution<> udist(-1, 1);

    for(int i = 0; i < numDots; i++){

        //Generate our random point within the bounding box of our sphere
        float randX = udist(rng);
        float randY = udist(rng);
        float randZ =  udist(rng);

        lux::Vector d(randX, randY, randZ);

        //Displace our dot radially with first level of noise
        float radialDisp = std::pow(std::abs(noise1.eval(d[0] + offset, d[1] + offset, d[2] + offset)), clump);
        lux::Vector dSphere = d.unitvector();

        dSphere *= radialDisp;

        lux::Vector dot = P + dSphere * radius;

        //Displace our dot radially with second level of noise
        lux::Vector d2;
        /*d2[0] = noise2->eval(dSphere[0], dSphere[1], dSphere[2]);
        d2[1] = noise2->eval(dSphere[0] + offset, dSphere[1] + offset, dSphere[2] + offset);
        d2[2] = noise2->eval(dSphere[0] + offset*2, dSphere[1] + offset*2, dSphere[2] + offset*2);*/

        d2[0] = noise2.eval(dSphere[0] + 0.3421, dSphere[1] - 1.2313, dSphere[2] + 3.123);
        d2[1] = noise2.eval(dSphere[0] + 10.231, dSphere[1] + 1923.12, dSphere[2] + 3.31231);
        d2[2] = noise2.eval(dSphere[0] + 45.323, dSphere[1] + 93.324, dSphere[2] + 102.3142);
        dot += d2;

        lux::Vector diff = dot - P;
        //Only bake out dot if it is within the boundaries
        if (sqrt(diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2]) < dBound){
            bakeDot(dot, value);
        }
    }
}

int DensityGrid::bakeDot(const Vector& P, const float density){

    //(-x, -y, -z)
    Vector indices = positionToIndex(P);
    int x = indices[0];
    int y = indices[1];
    int z = indices[2];
    //now we can actually interpolate
    //d holds values between 0 to 1
    //Vector d = (P - indexToPosition(x, y, z)) / voxelLength;
    Vector d = (P - (indices * voxelLength + origin)) / voxelLength;
    /*//(-x, -y, z)
    c[1] = c[0] + 1;
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
    c[7] = c[6] + 1;*/


    //Weights
    float wx1, wx2, wy1, wy2, wz1, wz2;

    wx1 = d[0];
    wx2 = 1 - d[0];
    wy1 = d[1];
    wy2 = 1 - d[1];
    wz1 = d[2];
    wz2 = 1 - d[2];

    data->set(x, y, z,      data->get(x, y, z) +     density * wx1 * wy1 * wz1);
    data->set(x, y, z+1,    data->get(x, y, z+1) +   density * wx1 * wy1 * wz2);
    data->set(x, y+1, z,    data->get(x, y+1, z) +   density * wx1 * wy2 * wz1);
    data->set(x, y+1, z+1,  data->get(x, y+1, z+1) + density * wx1 * wy2 * wz2);
    data->set(x+1, y, z,    data->get(x+1, y, z) +   density * wx2 * wy1 * wz1);
    data->set(x+1, y, z+1,  data->get(x+1, y, z+1) + density * wx2 * wy1 * wz2);
    data->set(x+1, y+1, z,  data->get(x+1, y+1, z) + density * wx2 * wy2 * wz1);
    data->set(x+1, y+1, z+1,data->get(x+1, y+1, z+1)+density * wx2 * wy2 * wz2);
    /*data->set() += density * wx1 * wy1 * wz2;
    data->set() += density * wx1 * wy2 * wz1;
    data->set() += density * wx1 * wy2 * wz2;
    data->set() += density * wx2 * wy1 * wz1;
    data->set() += density * wx2 * wy1 * wz2;
    data->set() += density * wx2 * wy2 * wz1;
    data->set() += density * wx2 * wy2 * wz2;*/
    //std::cout << c[0] << ":  " << values.get()[c[0]] << "\n";

    return 1;
}
//-------------------------------------------------------------------------------------------------------------------------------
//Deep Shadow Map
//-------------------------------------------------------------------------------------------------------------------------------
DeepShadowMap::DeepShadowMap(light l, float m, FloatVolumeBase f, Vector o, const Vector& s, int vx, int vy, int vz, int p)
    : FloatGrid(f, o, s, vx, vy, vz, p),
    sourceLight(l),
    marchStep(m){

    std::cout << "Building Deep Shadow Map\n";
    //stamp the values into our grid
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
            for(int k = 0; k < zVoxels; k++){

                //First convert our indices to world space
                float ii = (float)i * voxelLength + origin[0];
                float jj = (float)j * voxelLength + origin[1];
                float kk = (float)k * voxelLength + origin[2];
                //std::cout << "(" << i << ", " << j << ", " << k << "): " << " (" << ii << ", " << jj << ", " << kk << ")\n";

                //now evaluate the field at that point
                data->set(i, j, k, rayMarchLightScatter(Vector(ii, jj, kk)));

            }
        }
    }
}

//Returns the denisty integral at this position
double DeepShadowMap::rayMarchLightScatter(const Vector& x){

    double densityInt = 0.0;
    double marchLen = 0.0;

    lux::Vector toLight(sourceLight.pos - x);
    double distanceToLight = toLight.magnitude();
    toLight.normalize();
    lux::Vector x1(x);
    //If density at our grid point is 0, there's no point in calculating the integral, as the point will never be lit
    if(field.get()->eval(x1) <= 0.0) return 0;

    while (marchLen < distanceToLight){

        double density = field.get()->eval(x1);
        //Do not add density if it is negative
        if(density > 0.0){
            densityInt += density * marchStep;
        }
        x1 += marchStep * toLight;
        marchLen += marchStep;

    }
    return densityInt;
}

