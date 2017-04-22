#include "FloatGrid.h"

class ColorGrid{
    public:
        ColorGrid(ColorVolumeBase f, const Vector& c, const Vector& s, const int& vx, const int& vy, const int& vz, const int& partitionSize);
        //ColorGrid(const ColorGrid& f);
        ~ColorGrid();

        virtual const float getVoxelLength(const int x, const int y, const int z) const;
        const Color trilinearInterpolate(const Vector& P) const;
        void StampWisp(float value, const Vector& P, const SimplexNoiseObject& n1, const SimplexNoiseObject& n2, float clump, float radius, float numDots, float offset, float dBound, const Vector& normal, int numSteps, float streakLength);
        virtual void StampField(const FloatVolumeBase& f, const BoundingBox& AABB, int operand);
        virtual void StampvdbLevelSet(const std::string filename, const std::string levelsetName, const BoundingBox& AABB, int operand);
        virtual void createBoundingBoxes();

        const Vector center;
        const Vector length;

        float stampXMin;
        float stampXMax;
        float stampYMin;
        float stampYMax;
        float stampZMin;
        float stampZMax;
        std::vector<BoundingBox> gridBBs;
    protected:

        ColorVolumeBase ColorField;
        Grid *red;
        Grid *green;
        Grid *blue;
        const Vector origin;
        const int xVoxels;
        const int yVoxels;
        const int zVoxels;
        const float voxelLength;
        const int totalCells;

        virtual const Vector positionToIndex(const Vector& P) const;
        virtual const Vector indexToPosition(const int i, const int j, const int k) const;
        const int isInGrid(int x, int y, int z) const;
        const int isInGrid(const Vector& x) const;
        int bakeDot(const Vector& p, const float density);
};

typedef std::shared_ptr<ColorGrid> ColorGridPtr;

class ColorGridBase :  public ColorGridPtr{
    public:
        ColorGridBase(){};
        ColorGridBase(ColorGrid* f) : ColorGridPtr(f){};
        ~ColorGridBase(){};
        ColorGrid* getRef() { return ColorGridPtr::get();};
};
