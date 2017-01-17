#ifndef __FIELDS_WRAP_H__
#define __FIELDS_WRAP_H__
#include "grid.h"

lux::FloatVolumeBase Constantf(const float value);
lux::VectorVolumeBase Constantv(const float x, const float y, const float z);
lux::FloatVolumeBase Sphere(const float rad);
lux::FloatVolumeBase Spheredf(const float rad);
lux::FloatVolumeBase Box(const float rad, const float c);
lux::FloatVolumeBase Cylinder(const lux::Vector& n, const float rad);
lux::FloatVolumeBase Plane(const lux::Vector& n, const lux::Vector& c);
lux::FloatVolumeBase Cone(const lux::Vector& n, const float h, const float ang);
lux::FloatVolumeBase Noisef(SimplexNoiseObject s);
lux::FloatVolumeBase PyroSphere(const float rad, const float d, const float e, SimplexNoiseObject s);
lux::VectorVolumeBase Noisev(SimplexNoiseObject s, const float xO, const float yO, const float zO);
lux::ColorVolumeBase Noisec(SimplexNoiseObject s, const float xO, const float yO, const float zO);

//------------------------------------------------------------------------------------------------------------------------------
// TRANSFORMATIONS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Translatef( const lux::FloatVolumeBase& f,  const lux::Vector& d);
lux::VectorVolumeBase Translatev( const lux::VectorVolumeBase& f,  const lux::Vector& d);

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

lux::ColorVolumeBase ColorFromDensity(const lux::FloatVolumeBase& f, const lux::ColorSlider& cSlider, float min, float max) ;
//------------------------------------------------------------------------------------------------------------------------------
// GRIDS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatGridBase Gridf(const lux::FloatVolumeBase& f, const lux::Vector& o, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
lux::FloatVolumeBase GriddedVolf(const lux::FloatGridBase& g); 
lux::FloatVolumeBase AutoGriddedf(const lux::FloatVolumeBase& f, const lux::Vector& o, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
lux::DSMBase DSM(const lux::light& l, float m, const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
#endif
