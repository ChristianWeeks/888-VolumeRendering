#include "ray.h"
#include "boundingbox.h"

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

BoundingBox::BoundingBox(){}


BoundingBox::BoundingBox(const lux::Vector& c, const lux::Vector& l) : center(c), len(l){
    setBounds(c, l);
}
BoundingBox::BoundingBox(const BoundingBox& bb) : center(bb.center), len(bb.len){
    setBounds(bb.center, bb.len);
}

BoundingBox::~BoundingBox(){}

void BoundingBox::setBounds(const lux::Vector& c, const lux::Vector& l) {
    center = c;
    len = l;
    bounds[0] = center - (len/2.0);
    bounds[1] = center + (len/2.0);
}

std::vector<float> BoundingBox::intersect(const Ray &r, float t0, float t1) const {
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  std::vector<float> noIntersect = {-1.0};
  tmin = (bounds[r.sign[0]][0] - r.origin[0]) * r.inv_direction[0];
  tmax = (bounds[1-r.sign[0]][0] - r.origin[0]) * r.inv_direction[0];
  tymin = (bounds[r.sign[1]][1] - r.origin[1]) * r.inv_direction[1];
  tymax = (bounds[1-r.sign[1]][1] - r.origin[1]) * r.inv_direction[1];
  if ( (tmin > tymax) || (tymin > tmax) )
    return noIntersect;
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;
  tzmin = (bounds[r.sign[2]][2] - r.origin[2]) * r.inv_direction[2];
  tzmax = (bounds[1-r.sign[2]][2] - r.origin[2]) * r.inv_direction[2];
  if ( (tmin > tzmax) || (tzmin > tmax) )
    return noIntersect;
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  if ((tmin < t1) && (tmax > t0)){
    std::vector<float> intersectionPoints = {tmin, tmax};
    return intersectionPoints;
  }
  return noIntersect;
}
