#include "grid.h"

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
    stampXMin(-1),
    stampXMax(1),
    stampYMin(-1),
    stampYMax(1),
    stampZMin(-1),
    stampZMax(1),
    field(f),
    origin(c - (s/2.0)),
    xVoxels(xv),
    yVoxels(yv),
    zVoxels(zv),
    voxelLength(s[0] / (double)xv),
    totalCells(xv*yv*zv){

    if (partitionSize > 0){
        try{
            if ((xv) % partitionSize != 0 || (yv) % partitionSize != 0 || (zv) % partitionSize != 0){
                throw 10;
            }
            if (s[0] / xv != s[1] / yv || s[0] / xv != s[2] / zv){
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
        data = new SparseGrid(xv, yv, zv, partitionSize);
    }
    else{
        data = new DenseGrid(xv, yv, zv);
    }
};

inline const float FloatGrid::getVoxelLength(const int x, const int y, const int z) const{
    return voxelLength;
}

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

//void FloatGrid::StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound, const Vector& normal, int numSteps, float streakLength){std::cout << "FloatGrid can't stamp wisps! Use a density grid.\n";};

//Converts a position in 3-D space to an index in our grid
const Vector FloatGrid::positionToIndex(const Vector& P) const{
    Vector P2 = (P - origin) / voxelLength;
    P2[0] = int(P2[0]);
    P2[1] = int(P2[1]);
    P2[2] = int(P2[2]);
    return P2;
}

const Vector FloatGrid::indexToPosition(int i, int j, int k) const{
    Vector P(i, j, k);
    return P * voxelLength + origin;
}

const int FloatGrid::isInGrid(int x, int y, int z) const{
    if (x < 0 || y < 0 || z < 0 || x >= (xVoxels - 1) || y >= (yVoxels - 1) || z >= (zVoxels - 1))
        return 0;
    return 1;
}

const int FloatGrid::isInGrid(const Vector& x) const{
    if (x[0] < 0 || x[1] < 0 || x[2] < 0 || x[0] >= (xVoxels - 1) || x[1] >= (yVoxels - 1) || x[2] >= (zVoxels - 1))
        return 0;
    return 1;
}

const float FloatGrid::trilinearInterpolate(const Vector& position) const{

    //First we have to find what grid points are around our given position

    //Contains the indices of all the grid points around our position
    Vector indices = positionToIndex(position);
    //If positionToIndex returns a negative value, we are at the border or beyond our grid
    if(!isInGrid(indices)) return -0.0001;

    int x = indices[0];
    int y = indices[1];
    int z = indices[2];
    Vector worldPos = indexToPosition(x, y, z);
    //now we can actually interpolate
    //d holds values between 0 to 1
    Vector d = (position - worldPos) / getVoxelLength(x, y, z);
    d.clamp(0, 1);
    //if(d[0] > 1.0 || d[1] > 1.0 || d[2] > 1.0){
    //    std::cout << "d: " << d;
    //}

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

void FloatGrid::StampField(const FloatVolumeBase& f, const BoundingBox& AABB, int operand){

    //First we need to get the indices within the bounding box.
    //Add 1 to the returned indices because indices cast to integers will be 1 voxel outside of the grid
    Vector startPos = positionToIndex(AABB.bounds[0]);
    Vector endPos = positionToIndex(AABB.bounds[1]);
    int x1 = startPos[0] + 1;
    int y1 = startPos[1] + 1;
    int z1 = startPos[2] + 1;
    int x2 = endPos[0];
    int y2 = endPos[1];
    int z2 = endPos[2];
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (z1 < 0)
        z1 = 0;
    if (x2 > xVoxels)
        x2 = xVoxels;
    if (y2 > yVoxels)
        y2 = yVoxels;
    if (z2 > zVoxels)
        z2 = zVoxels;

    for (int i = x1; i < x2; i++){
        for (int j = y1; j < y2; j++){
#pragma omp parallel for
            for (int k = z1; k < z2; k++){

                Vector worldPos = indexToPosition(i, j, k);
                float ii = worldPos[0];
                float jj = worldPos[1];
                float kk = worldPos[2];
                //Replace
                if (operand == 0){
                    data->set(i, j, k, f.get()->eval(Vector(ii, jj, kk)));
                }
                //Max
                else if(operand == 1){
                    float value = f.get()->eval(Vector(ii, jj, kk));
                    if (value > data->get(i, j, k)){
                        data->set(i, j, k, value);
                    }
                }
                //Add
                else if(operand == 2){
                    data->set(i, j, k, f.get()->eval(Vector(ii, jj, kk)) + data->get(i, j, k));
                }
            }
        }
    }
}

//Wisp algorithm
void FloatGrid::StampWisp(float value, const Vector& P, const SimplexNoiseObject& noise1, const SimplexNoiseObject& noise2, float clump, float radius, float numDots, float offset, float dBound, const Vector& n, int numSteps, float streakLength){

    std::mt19937 rng(20);
    std::uniform_real_distribution<> udistx(stampXMin, stampXMax);
    std::uniform_real_distribution<> udisty(stampYMin, stampYMax);
    std::uniform_real_distribution<> udistz(stampZMin, stampZMax);

    for(int i = 0; i < numDots; i++){

        //Generate our random point within the bounding box of our sphere
        float randX = udistx(rng);
        float randY = udisty(rng);
        float randZ =  udistz(rng);

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
        lux::Vector normal = diff.unitvector();
        //Only bake out dot if it is within the boundaries
        if (sqrt(diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2]) < dBound){

            bakeDot(dot, value);
            if (streakLength != 0){
                for (int j = 1; j < numSteps; j++){
                    float stepLength = streakLength / float(numSteps);
                    float stampValue = value * (float((numSteps - j)) / float(numSteps));
                    lux::Vector newPosition = diff + normal*j*stepLength;
                    bakeDot(newPosition, stampValue);
                }
            }
        }
    }
}

int FloatGrid::bakeDot(const Vector& P, const float density){

    //(-x, -y, -z)
    Vector indices = positionToIndex(P);
    int x = indices[0];
    int y = indices[1];
    int z = indices[2];
    if(!isInGrid(indices)) return 0;
    Vector WorldPos = indexToPosition(x, y, z);

    //now we can actually interpolate
    //d is the distance from our position to its closest index, normalized from 0 - 1
    Vector d = (P - (WorldPos)) / getVoxelLength(x, y, z);
    d.clamp(0, 1);
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
//Frustum Grid
//-------------------------------------------------------------------------------------------------------------------------------
FrustumGrid::FrustumGrid(float initValue, const Camera& cam, int vx, int vy, int vz, int p) :
    FloatGrid(FloatVolumeBase(new ConstantVolumef(initValue)), cam.position, Vector(1, 1, 1), vx, vy, vz, p),
    camera(cam),
    zVoxelLength((camera.far - camera.near) / float(vz)){
    std::cout << "------------------------------------------------------\n";
    std::cout << "Initializing Frustum Grid\n";
    float voxelRatio = float(vx) / float(vy);
    if(voxelRatio != camera.aspect_ratio){
        std::cerr << "Voxel ratio is not equivalent to aspect ratio; VR: " << voxelRatio << " AR:  " << camera.aspect_ratio << "\n";
        //return;
    }
    boost::timer gridTimer;
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
            for(int k = 0; k < zVoxels; k++){

                data->set(i, j, k, initValue);
            }
        }
    }
    std::cout << "Frustum Grid Initialized in: " << gridTimer.elapsed() <<" seconds\n";
    std::cout << "------------------------------------------------------\n";
}

FrustumGrid::FrustumGrid(FloatVolumeBase f, const Camera& cam, int vx, int vy, int vz, int p) :
    FloatGrid(f, cam.position, Vector(1, 1, 1), vx, vy, vz, p),
    camera(cam),
    zVoxelLength((camera.far - camera.near) / float(vz)){
    std::cout << "------------------------------------------------------\n";
    std::cout << "Initializing Frustum Grid\n";
    float voxelRatio = float(vx) / float(vy);
    if(voxelRatio != camera.aspect_ratio){
        std::cerr << "Voxel ratio is not equivalent to aspect ratio; VR: " << voxelRatio << " AR:  " << camera.aspect_ratio << "\n";
        //return;
    }
    double startTime, endTime;
    startTime = omp_get_wtime();
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
#pragma omp parallel for
            for(int k = 0; k < zVoxels; k++){

                //First convert our indices to world space
                Vector worldPos = indexToPosition(i, j, k);
                //std::cout << "Index: " << i << ", " << j << ", " << k << "\n";
                //std::cout << "Pos: " << worldPos << "\n";
                //std::cout << "(" << i << ", " << j << ", " << k << "): " << " (" << ii << ", " << jj << ", " << kk << ")\n";
                //now evaluate the field at that point
                data->set(i, j, k, field.get()->eval(worldPos));

               // std::cout << values.get()[k + j*zVoxels + i*yVoxels*zVoxels] << "\n";
            }
        }
    }
    endTime = omp_get_wtime();
    std::cout << "Frustum Grid Initialized in: " << (startTime - endTime) <<" seconds\n";
    std::cout << "------------------------------------------------------\n";
}


const float FrustumGrid::getVoxelLength(const int x, const int y, const int z) const{
    float zDepth = camera.near + (float(z) * zVoxelLength);
    float xylength = sin(camera.FOV / 2.0) * zDepth;
    return xylength;

    /*Vector worldPos1 = indexToPosition(x, y, z);
    Vector worldPos2 = indexToPosition(x+1, y, z);
    float diff =  (worldPos2 - worldPos1).magnitude();
    //std::cout << diff << "\n";
    return diff;*/
}

const Vector FrustumGrid::positionToIndex(const Vector& P) const{
    //First convert P to UV coordinates on our camera image plane
    //Map P to a point on our image plane Q
    Vector normPos = P - camera.position;
    float dot = camera.axis_view * normPos;
    Vector q = normPos / dot;
    q = q - camera.axis_view;
    float u = q * camera.axis_right;
    float v = q * camera.axis_up;
    float x = 0.5 * ((u / tan(camera.FOV / 2.0)) + 1);
    float y = 0.5 * (((v*camera.aspect_ratio) / tan(camera.FOV / 2.0)) + 1);
    float z = (normPos.magnitude() - camera.near) / (camera.far - camera.near);
    Vector P2 = Vector(int(x*xVoxels), int(y*yVoxels), int(z*zVoxels));
    return P2;
}


const Vector FrustumGrid::indexToPosition(const int i, const int j, const int k) const{
    float u = (2*(float(i)/float(xVoxels)) - 1)*tan(camera.FOV / 2.0);
    float v = (2*(float(j)/float(yVoxels)) - 1)*tan(camera.FOV / 2.0) / camera.aspect_ratio;
    float Z = camera.near + (float(k)/float(zVoxels))*(camera.far - camera.near);
    //std::cout << "u: " << u << ", v: " << v << ", Z: " << Z << "\n";
    Vector q = u * camera.axis_right + v * camera.axis_up;
    Vector X = camera.position + Z * (q + camera.axis_view) / ((q + camera.axis_view).magnitude());
    //std::cout << "X: " << X << "\n";
    return X;
}

//Frustum Grid needs its own StampField method because it needs to make extra calculations to properly
//generate a bounding box to stamp in
void FrustumGrid::StampField(const FloatVolumeBase& f, const BoundingBox& AABB, int operand){

    //First we need to get the indices within the bounding box.
    //Add 1 to the returned indices because indices cast to integers will be 1 voxel outside of the grid
    Vector start = AABB.bounds[0];
    Vector end = AABB.bounds[1];
    Vector gB[8];

    //Get index positions of all world space coordinates for the bounding box
    gB[0] = positionToIndex(start);
    gB[1] = positionToIndex(Vector(start[0], start[1], end[2]));
    gB[2] = positionToIndex(Vector(start[0], end[1], start[2]));
    gB[3] = positionToIndex(Vector(start[0], end[1], end[2]));
    gB[4] = positionToIndex(Vector(end[0], start[1], start[2]));
    gB[5] = positionToIndex(Vector(end[0], start[1], end[2]));
    gB[6] = positionToIndex(Vector(end[0], end[1], start[2]));
    gB[7] = positionToIndex(end);

    int x1 = gB[0][0];
    int y1 = gB[0][1];
    int z1 = gB[0][2];
    int x2 = gB[0][0];
    int y2 = gB[0][1];
    int z2 = gB[0][2];

    //Find our maximum index bounds
    for(int i = 0; i < 8; i++){
        if (x1 > gB[i][0])
            x1 = gB[i][0];
        if (y1 > gB[i][1])
            y1 = gB[i][1];
        if (z1 > gB[i][2])
            z1 = gB[i][2];

        if (x2 < gB[i][0])
            x2 = gB[i][0];
        if (y2 < gB[i][1])
            y2 = gB[i][1];
        if (z2 < gB[i][2])
            z2 = gB[i][2];
    }

    //Now make sure those bounds are within the bounds of our grid
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (z1 < 0)
        z1 = 0;
    if (x2 > xVoxels)
        x2 = xVoxels;
    if (y2 > yVoxels)
        y2 = yVoxels;
    if (z2 > zVoxels)
        z2 = zVoxels;
    /*std::cout << "------------------------\n";
    std::cout << "origin: " << origin << "\n";
    std::cout << "start: " << AABB.bounds[0] << "\n";
    std::cout << "end: " << AABB.bounds[1] << "\n";
    std::cout << "start: " << x1 << ", " << y1 << ", " << z1 << "\n";
    std::cout << "end: " << x2 << ", " << y2 << ", " << z2 << "\n";
*/
    for (int i = x1; i < x2; i++){
        for (int j = y1; j < y2; j++){
#pragma omp parallel for 
            for (int k = z1; k < z2; k++){

                Vector worldPos = indexToPosition(i, j, k);
                float ii = worldPos[0];
                float jj = worldPos[1];
                float kk = worldPos[2];
                //Replace
                if (operand == 0){
                    data->set(i, j, k, f.get()->eval(Vector(ii, jj, kk)));
                }
                //Max
                else if(operand == 1){
                    float value = f.get()->eval(Vector(ii, jj, kk));
                    if (value > data->get(i, j, k)){
                        data->set(i, j, k, value);
                    }
                }
                //Add
                else if(operand == 2){
                    data->set(i, j, k, f.get()->eval(Vector(ii, jj, kk)) + data->get(i, j, k));
                }
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------------
//DensityGrid
//-------------------------------------------------------------------------------------------------------------------------------
DensityGrid::DensityGrid(FloatVolumeBase f, Vector o, const Vector& s, int vx, int vy, int vz, int p)
    : FloatGrid(f, o, s, vx, vy, vz, p){
    //stamp the values into our grid
    std::cout << "------------------------------------------------------\n";
    std::cout << "Initializing Dense Grid\n";
    boost::timer gridTimer;
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
    std::cout << "Dense Grid Initialized in: " << gridTimer.elapsed() <<" seconds\n";
    std::cout << "------------------------------------------------------\n";
}

//DensityGrid::DensityGrid(const DensityGrid& f) : FloatGrid(f.field, f.origin, f.length, f.voxels, f.data->partitionSize){std::cout << "Density Grid Copy Constructor!\n";};

//-------------------------------------------------------------------------------------------------------------------------------
//Deep Shadow Map
//-------------------------------------------------------------------------------------------------------------------------------
DeepShadowMap::DeepShadowMap(light l, float m, FloatVolumeBase f, Vector o, const Vector& s, int vx, int vy, int vz, int p)
    : FloatGrid(f, o, s, vx, vy, vz, p),
    sourceLight(l),
    marchStep(m){

    std::cout << "------------------------------------------------------\n";
    std::cout << "Building Deep Shadow Map\n";
    double startTime, endTime;
    startTime = omp_get_wtime();
    //stamp the values into our grid
    for(int i = 0; i < xVoxels; i++){
        for(int j = 0; j < yVoxels; j++){
#pragma omp parallel for
            for(int k = 0; k < zVoxels; k++){

                //First convert our indices to world space
                Vector worldPos = indexToPosition(i, j, k);
                float ii = worldPos[0];
                float jj = worldPos[1];
                float kk = worldPos[2];
//std::cout << "(" << i << ", " << j << ", " << k << "): " << " (" << ii << ", " << jj << ", " << kk << ")\n";


                //now evaluate the field at that point
                data->set(i, j, k, rayMarchLightScatter(Vector(ii, jj, kk)));
            }
        }
    }
    endTime = omp_get_wtime();
    std::cout << "Deep Shadow Map Initialized in: " << (endTime - startTime) <<" seconds\n";
    std::cout << "------------------------------------------------------\n";
}

//Returns the denisty integral at this position
double DeepShadowMap::rayMarchLightScatter(const Vector& x){

    double densityInt = 0.0;
    double marchLen = 0.0;

    Vector toLight(sourceLight.pos - x);
    double distanceToLight = toLight.magnitude();
    toLight.normalize();
    Vector x1(x);
    //If density at our grid point is 0, there's no point in calculating the integral, as the point will never be lit
    //Check all points around our grid point
    int mustEvaluate = 0;
    for (float i = -1; i < 2; i++){
        for (float j = -1; j < 2; j++){
            for (float k = -1; k < 2; k++){
                Vector x2(x1[0] + i*voxelLength, x1[1] + j*voxelLength, x1[2] + k*voxelLength);
                /*std::cout << "------------------------------------\n";
                std::cout << "i, j, k: " << i << ", " << j << ", " << k << "\n";
                std::cout << x1 << "\n";
                std::cout << x2 << "\n";
                float density = field.get()->eval(x2);
                std::cout << density << "\n";*/
                if (field.get()->eval(x2) > 0.0){
                    mustEvaluate = 1;
                    break;
                }
            }
        }
    }

    if(mustEvaluate == 0) return 0.0;

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

