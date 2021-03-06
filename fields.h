#ifndef __FIELDS_WRAP_H__
#define __FIELDS_WRAP_H__
#include "FloatGrid.h"

lux::FloatVolumeBase Constantf(const float value);
lux::VectorVolumeBase Constantv(const float x, const float y, const float z);
lux::FloatVolumeBase Sphere(const float rad);
lux::FloatVolumeBase Spheredf(const float rad);
lux::FloatVolumeBase Box(const float rad, const float c);
lux::FloatVolumeBase Cylinder(const lux::Vector& n, const float rad);
lux::FloatVolumeBase Plane(const lux::Vector& n, const lux::Vector& c);
lux::FloatVolumeBase Cone(const lux::Vector& n, const float h, const float ang);
lux::FloatVolumeBase Torus(const lux::Vector& n, const float rMaj, const float rMin);
lux::FloatVolumeBase Icosohedron();
lux::FloatVolumeBase SteinerPatch();

lux::FloatVolumeBase Noisef(SimplexNoiseObject s);
lux::FloatVolumeBase NoiseSpheref(const lux::Vector& pos, const SimplexNoiseObject s, const float rad, const float f);
lux::FloatVolumeBase PyroSphere(const float rad, const float d, const float e, const float ds, SimplexNoiseObject s);
lux::FloatVolumeBase PyroSphereSDF(const float rad, const float d, const float e, const float ds, SimplexNoiseObject s);
lux::FloatVolumeBase PyroLevelSet(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase g, const float e, const float ds, SimplexNoiseObject s);

lux::FloatVolumeBase Gradientf(const lux::Vector& d, const lux::Vector& start, const float l, const float mn, const float mx);
lux::VectorVolumeBase Noisev(SimplexNoiseObject s, const float xO, const float yO, const float zO);
lux::VectorVolumeBase Radialv(const lux::Vector& origin, const int normalized, const float magnitude);
lux::VectorVolumeBase Identityv();
lux::ColorVolumeBase Noisec(SimplexNoiseObject s, const float xO, const float yO, const float zO);

//------------------------------------------------------------------------------------------------------------------------------
// TRANSFORMATIONS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Translatef( const lux::FloatVolumeBase& f,  const lux::Vector& d);
lux::VectorVolumeBase Translatev( const lux::VectorVolumeBase& f,  const lux::Vector& d);
lux::FloatVolumeBase Scalef( const lux::FloatVolumeBase& f,  const lux::Vector& p, const float s);
lux::FloatVolumeBase Rotatef(const lux::FloatVolumeBase f, const lux::Vector& p, const lux::Vector axis, float ang);

//------------------------------------------------------------------------------------------------------------------------------
// OPERATORS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Addf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)       ;
lux::FloatVolumeBase Subtractf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)  ;
lux::FloatVolumeBase Multf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)      ;
lux::VectorVolumeBase Multvf(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase& g)      ;
lux::FloatVolumeBase AdvectSLf(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase& g, const float t)  ;
lux::VectorVolumeBase AdvectSLv(const lux::VectorVolumeBase& f, const lux::VectorVolumeBase& g, const float t)  ;
lux::FloatVolumeBase AdvectMMCf(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase& g, const float t)  ;
lux::FloatVolumeBase Warpf( const lux::FloatVolumeBase& f,  const lux::VectorVolumeBase& g) ;

lux::FloatVolumeBase Unionf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)         ;
lux::FloatVolumeBase Intersectf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)     ;
lux::FloatVolumeBase Cutoutf(const lux::FloatVolumeBase& f, const lux::FloatVolumeBase& g)        ;
lux::FloatVolumeBase Shellf(const lux::FloatVolumeBase& f, const float w)        ;
lux::FloatVolumeBase Maskf(const lux::FloatVolumeBase& f)                                    ;
lux::FloatVolumeBase Clampf(const lux::FloatVolumeBase& f, const float Min, const float Max) ;
lux::FloatVolumeBase Reducef(const lux::FloatVolumeBase& f, const float cutoff) ;

lux::VectorVolumeBase Addv(const lux::VectorVolumeBase& f, const lux::VectorVolumeBase& g) ;
lux::VectorVolumeBase Subtractv(const lux::VectorVolumeBase& f, const lux::VectorVolumeBase& g) ;

lux::ColorVolumeBase ColorFromDensity(const lux::FloatVolumeBase& f, const lux::ColorSlider& cSlider, float min, float max) ;
//------------------------------------------------------------------------------------------------------------------------------
// GRIDS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatGridBase Gridf(const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
lux::VectorGridBase Gridv(const lux::VectorVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
lux::VectorGridBase GradientGrid(const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
lux::FloatGridBase GridConstf(float initValue, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
lux::FloatGridBase FrustumGridf(const float f, const lux::Camera& cam, int vx, int vy, int vz, int partitionSize);
lux::FloatVolumeBase GriddedVolf(const lux::FloatGridBase& g); 
lux::VectorVolumeBase GriddedVolv(const lux::VectorGridBase& g); 
lux::FloatVolumeBase AutoGriddedf(const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int partitionSize);
lux::DSMBase DSM(const lux::light& l, float m, const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int partitionSize, std::vector<BoundingBox> BBs);
#endif
