#ifndef __FIELDS_WRAP_H__
#define __FIELDS_WRAP_H__
#include "grid.h"
lux::FloatVolumeBase Sphere_v(const float rad);

lux::FloatVolumeBase Constantf(const float value);
lux::FloatVolumeBase Sphere(const float rad);
lux::FloatVolumeBase Box(const float rad, const float c);
lux::FloatVolumeBase Cylinder(const lux::Vector& n, const float rad);
lux::FloatVolumeBase Plane(const lux::Vector& n, const lux::Vector& c);
lux::FloatVolumeBase Cone(const lux::Vector& n, const float h, const float ang);
lux::FloatVolumeBase Noisef(SimplexNoiseObject s);
lux::FloatVolumeBase PyroSphere(const float rad, const float d, const float e, SimplexNoiseObject s);
lux::VectorVolumeBase Noisev(SimplexNoiseObject s, const float xO, const float yO, const float zO);
lux::ColorVolumeBase Noisec(SimplexNoiseObject s, const float xO, const float yO, const float zO);

//------------------------------------------------------------------------------------------------------------------------------
// OPERATORS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Addf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)       ;
lux::FloatVolumeBase Subtractf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)  ;
lux::FloatVolumeBase Multf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)      ;
lux::FloatVolumeBase AdvectSLf(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase& g, const float t)  ;
lux::FloatVolumeBase AdvectMMCf(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase& g, const float t)  ;

lux::FloatVolumeBase Unionf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)         ;
lux::FloatVolumeBase Intersectf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)     ;
lux::FloatVolumeBase Cutoutf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)        ;
lux::FloatVolumeBase Maskf(const lux::FloatVolumeBase& f)                                    ;
lux::FloatVolumeBase Clampf(const lux::FloatVolumeBase& f, const float Min, const float Max) ;

lux::VectorVolumeBase Addv(const lux::VectorVolumeBase& f, const lux::VectorVolumeBase& g) ;
lux::VectorVolumeBase Subtractv(const lux::VectorVolumeBase& f, const lux::VectorVolumeBase& g) ;
//------------------------------------------------------------------------------------------------------------------------------
// GRIDS
//------------------------------------------------------------------------------------------------------------------------------
#endif
