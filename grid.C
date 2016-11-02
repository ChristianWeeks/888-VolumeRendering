#include "grid.h"

using namespace lux;

//-------------------------------------------------------------------------------------------------------------------------------
//FloatGrid
//-------------------------------------------------------------------------------------------------------------------------------
FloatGrid::FloatGrid(std::shared_ptr<Volume<float> > f, Vector o, double s, int v) :
    field(f),
    origin(o),
    size(s),
    voxels(v),
    voxelLength(size / (double)voxels),
    totalCells(v*v*v),
    values(new float[v*v*v]){}

FloatGrid::FloatGrid(const FloatGrid& f) :
    field(f.field),
    origin(f.origin),
    size(f.size),
    voxels(f.voxels),
    voxelLength(f.voxelLength),
    totalCells(f.totalCells),
    values(new float[f.totalCells]){
    std::cout << "FloatGrid Copy Constructor!\n";
    for(int i = 0; i < totalCells; i++){
        values.get()[i] = f.values.get()[i];
    }

}
FloatGrid::~FloatGrid(){}


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

const float FloatGrid::trilinearInterpolate(const Vector& position) const{

    //First we have to find what grid points are around our given position

    //Contains the indices of all the grid points around our position
    int c[8];
    //(-x, -y, -z)
    c[0] = positionToIndex(position);
    //(-x, -y, z)
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
    c[7] = c[6] + 1;
    //check for edge cases.  If its on the edge, we just don't apply at all
    //+x side
    for(int i = 0; i < 8; i++){
        if (c[i] > totalCells || c[i] < 0){
            std::cout << "Interp: Out of Bounds: " << c[i] << "; "  << "\n";
            return 1;
        }
    }
    //now we can actually interpolate
    //d holds values between 0 to 1
    Vector d = (position - indexToPosition(c[0])) / voxelLength;

    //Interpolate along x
    float c00, c10, c01, c11;
    c00 = values.get()[c[0]] * (1-d[0]) + values.get()[c[4]] * d[0];
    c01 = values.get()[c[1]] * (1-d[0]) + values.get()[c[5]] * d[0];
    c10 = values.get()[c[2]] * (1-d[0]) + values.get()[c[6]] * d[0];
    c11 = values.get()[c[3]] * (1-d[0]) + values.get()[c[7]] * d[0];

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

//-------------------------------------------------------------------------------------------------------------------------------
//DensityGrid
//-------------------------------------------------------------------------------------------------------------------------------
DensityGrid::DensityGrid(std::shared_ptr<Volume<float> > f, Vector o, double s, int v)
    : FloatGrid(f, o, s, v){
    //stamp the values.get() into our grid
    for(int i = 0; i < voxels; i++){
        for(int j = 0; j < voxels; j++){
            for(int k = 0; k < voxels; k++){

                //First convert our indices to world space
                float ii = (float)i * voxelLength + origin[0];
                float jj = (float)j * voxelLength + origin[1];
                float kk = (float)k * voxelLength + origin[2];
                //std::cout << "(" << i << ", " << j << ", " << k << "): " << " (" << ii << ", " << jj << ", " << kk << ")\n";

                //now evaluate the field at that point
                values.get()[k + j*voxels + i*voxels*voxels] = field.get()->eval(Vector(ii, jj, kk));

               // std::cout << values.get()[k + j*voxels + i*voxels*voxels] << "\n";
            }
        }
    }
}

//Wisp algorithm
void DensityGrid::StampWisp(const Vector& P, const SimplexNoiseObject& noise1, const SimplexNoiseObject& noise2, float clump, float radius, float numDots){

    std::random_device rnd;
    std::mt19937 rng(rnd());
    std::uniform_real_distribution<> udist(-radius, radius);

    for(int i = 0; i < numDots; i++){

        //Generate our random point within the bounding box of our sphere
        float randX = udist(rng);
        float randY = udist(rng);
        float randZ =  udist(rng);

        lux::Vector d(randX, randY, randZ);

        //Displace our dot radially with first level of noise
        float radialDisp = std::pow(std::abs(noise1.eval(d[0], d[1], d[2])), clump);
        lux::Vector dSphere = d.unitvector();

        dSphere *= radialDisp;

        lux::Vector dot = P + dSphere * radius;

        //Displace our dot radially with second level of noise
        lux::Vector d2;
        d2[0] = noise2.eval(dSphere[0], dSphere[1], dSphere[2]);
        d2[1] = noise2.eval(dSphere[0] + 0.1, dSphere[1] + 0.1, dSphere[2] + 0.1);
        d2[2] = noise2.eval(dSphere[0] - 0.1, dSphere[1] - 0.1, dSphere[2] - 0.1);

        dot += d2;
        bakeDot(dot, 1.0);
    }
}

int DensityGrid::bakeDot(const Vector& P, const float density){

    int c[8];
    //(-x, -y, -z)
    c[0] = positionToIndex(P);
    //(-x, -y, z)
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
    c[7] = c[6] + 1;

    for(int i = 0; i < 8; i++){
        if (c[i] > totalCells || c[i] < 0){
            std::cout << "BakeDot: Out of Bounds Index: " << c[i] << "; Position" << P << "\n";
            return 0;
        }
    }

    Vector d = (P - indexToPosition(c[0])) / voxelLength;

    //Weights
    float wx1, wx2, wy1, wy2, wz1, wz2;

    wx1 = d[0];
    wx2 = 1 - d[0];
    wy1 = d[1];
    wy2 = 1 - d[1];
    wz1 = d[2];
    wz2 = 1 - d[2];

    values.get()[c[0]] += density * wx1 * wy1 * wz1;
    values.get()[c[1]] += density * wx1 * wy1 * wz2;
    values.get()[c[2]] += density * wx1 * wy2 * wz1;
    values.get()[c[3]] += density * wx1 * wy2 * wz2;
    values.get()[c[4]] += density * wx2 * wy1 * wz1;
    values.get()[c[5]] += density * wx2 * wy1 * wz2;
    values.get()[c[6]] += density * wx2 * wy2 * wz1;
    values.get()[c[7]] += density * wx2 * wy2 * wz2;
    //std::cout << c[0] << ":  " << values.get()[c[0]] << "\n";

    return 1;
}
//-------------------------------------------------------------------------------------------------------------------------------
//Deep Shadow Map
//-------------------------------------------------------------------------------------------------------------------------------
DeepShadowMap::DeepShadowMap(light l, float m, std::shared_ptr<Volume<float> > f, Vector o, double s, int v)
    : FloatGrid(f, o, s, v),
    sourceLight(l),
    marchStep(m){

    std::cout << "Building Deep Shadow Map\n";
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
                values.get()[k + j*voxels + i*voxels*voxels] = rayMarchLightScatter(Vector(ii, jj, kk));

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

