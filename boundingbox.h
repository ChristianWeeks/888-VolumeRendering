#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include <assert.h>
#include <vector>
#include "Vector.h"
#include "ray.h"

/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class BoundingBox {
  public:
    BoundingBox();

    BoundingBox(const lux::Vector &min, const lux::Vector &max);

    ~BoundingBox();

    void setBounds(const lux::Vector &min, const lux::Vector &max);
    // (t0, t1) is the interval for valid hits
    std::vector<float> intersect(const Ray &, float t0, float t1) const;

    // corners
    lux::Vector bounds[2];
};

#endif // _BOUNDING_BOX_H_
