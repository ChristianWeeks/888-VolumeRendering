#ifndef _RAY_H_
#define _RAY_H_

#include "Vector.h"

/*
 * Ray class, for use with the optimized ray-box intersection test
 * described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 * 
 */

class Ray {
  public:
    Ray() { }
    Ray(lux::Vector o, lux::Vector d) {
      origin = o;
      direction = d;
      inv_direction = lux::Vector(1/d[0], 1/d[1], 1/d[2]);
      sign[0] = (inv_direction[0] < 0);
      sign[1] = (inv_direction[1] < 0);
      sign[2] = (inv_direction[2] < 0);
    }
    Ray(const Ray &r) {
      origin = r.origin;
      direction = r.direction;
      inv_direction = r.inv_direction;
      sign[0] = r.sign[0]; sign[1] = r.sign[1]; sign[2] = r.sign[2];
    }

    lux::Vector origin;
    lux::Vector direction;
    lux::Vector inv_direction;
    int sign[3];
};

#endif // _RAY_H_
