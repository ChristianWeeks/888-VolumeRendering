
#include "fields.h"

//------------------------------------------------------------------------------------------------------------------------------
//IMPLICIT VOLUMES 
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Constantf(const float value)               { return lux::FloatVolumeBase(new lux::ConstantVolumef(value));};
lux::FloatVolumeBase Sphere(const float rad)                    { return lux::FloatVolumeBase(new lux::SphereVolume(rad));};
lux::FloatVolumeBase Box(const float rad, const float c)        { return lux::FloatVolumeBase(new lux::BoxVolume(rad, c));};
lux::FloatVolumeBase Cylinder(const lux::Vector& n, const float rad) { return lux::FloatVolumeBase(new lux::CylinderVolume(n, rad));};
lux::FloatVolumeBase Plane(const lux::Vector& n, const lux::Vector& c)    { return lux::FloatVolumeBase(new lux::PlaneVolume(n, c));};
lux::FloatVolumeBase Cone(const lux::Vector& n, const float h, const float ang)    { return lux::FloatVolumeBase(new lux::ConeVolume(n, h, ang));};
lux::FloatVolumeBase Noisef(SimplexNoiseObject s)               { return lux::FloatVolumeBase(new lux::SimplexNoiseVolume(s));};
lux::FloatVolumeBase PyroSphere(const float rad, const float d, const float e, SimplexNoiseObject s){
    return lux::FloatVolumeBase(new lux::PyroSphereVolume(rad, d, e, s));};

lux::VectorVolumeBase Noisev(SimplexNoiseObject s, const float xO, const float yO, const float zO) {
    return lux::VectorVolumeBase(new lux::SimplexNoiseVectorVolume(s, xO, yO, zO));};

lux::ColorVolumeBase Noisec(SimplexNoiseObject s, const float xO, const float yO, const float zO) {
    return lux::ColorVolumeBase(new lux::SimplexNoiseColorVolume(s, xO, yO, zO));};

//------------------------------------------------------------------------------------------------------------------------------
// TRANSFORMATIONS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Translatef( const lux::FloatVolumeBase& f,  const lux::Vector& d)       { return lux::FloatVolumeBase(new lux::TranslateVolumef(f, d));};
lux::VectorVolumeBase Translatev( const lux::VectorVolumeBase& f,  const lux::Vector& d)       { return lux::VectorVolumeBase(new lux::TranslateVolumev(f, d));};

//------------------------------------------------------------------------------------------------------------------------------
// OPERATORS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Addf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)       { return lux::FloatVolumeBase(new lux::AddVolumef(f, g));};
lux::FloatVolumeBase Subtractf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)  { return lux::FloatVolumeBase(new lux::SubtractVolumef(f, g));};
lux::FloatVolumeBase Multf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)      { return lux::FloatVolumeBase(new lux::MultVolumef(f, g));};
lux::FloatVolumeBase AdvectSLf( const lux::FloatVolumeBase& f,  const lux::VectorVolumeBase& g, const float t)  {
    return lux::FloatVolumeBase(new lux::Advect_SL_Volume(f, g, t));};
lux::FloatVolumeBase AdvectMMCf( const lux::FloatVolumeBase& f,  const lux::VectorVolumeBase& g, const float t)  {
    return lux::FloatVolumeBase(new lux::Advect_MMC_Volume(f, g, t));};

lux::FloatVolumeBase Unionf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)         { return lux::FloatVolumeBase(new lux::UnionVolumef(f, g));};
lux::FloatVolumeBase Intersectf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)     { return lux::FloatVolumeBase(new lux::IntersectVolumef(f, g));};
lux::FloatVolumeBase Cutoutf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)        { return lux::FloatVolumeBase(new lux::CutoutVolumef(f, g));};
lux::FloatVolumeBase Maskf( const lux::FloatVolumeBase& f)                                    { return lux::FloatVolumeBase(new lux::MaskVolume(f));};
lux::FloatVolumeBase Clampf( const lux::FloatVolumeBase& f, const float Min, const float Max) { return lux::FloatVolumeBase(new lux::ClampVolume(f, Min, Max));};

lux::VectorVolumeBase Addv( const lux::VectorVolumeBase& f,  const lux::VectorVolumeBase& g) { return lux::VectorVolumeBase(new lux::AddVolumev(f, g));};
lux::VectorVolumeBase Subtractv( const lux::VectorVolumeBase& f,  const lux::VectorVolumeBase& g) { return lux::VectorVolumeBase(new lux::SubtractVolumev(f, g));};

lux::ColorVolumeBase ColorFromDensity(const lux::FloatVolumeBase& f, const lux::ColorSlider& cSlider, float min, float max) { return lux::ColorVolumeBase(new lux::ColorVolumeFromDensity(f, cSlider, min, max));};
//------------------------------------------------------------------------------------------------------------------------------
// GRIDS
//-----------------------------------------------------------------------------------------------------------------------------
lux::FloatGridBase Gridf(const lux::FloatVolumeBase& f, const lux::Vector& o, double s,  int v, int p) { return lux::FloatGridBase(new lux::DensityGrid(f, o, s, v, p));};

lux::FloatVolumeBase GriddedVolf(const lux::FloatGridBase& g)   { 
    lux::FloatVolumeBase gridded(new lux::GriddedVolume(g));
    gridded.BB.setBounds(g->center, g->length);
    return gridded;
};

lux::FloatVolumeBase AutoGriddedf(const lux::FloatVolumeBase& f, const lux::Vector& o, double s,  int v, int p) { 
    lux::FloatVolumeBase gridded( new lux::GriddedVolume(lux::FloatGridBase(new lux::DensityGrid(f, o, s, v, p))));
    gridded.BB.setBounds(o, s);
    return gridded;
};
lux::DSMBase DSM(const lux::light l, float m, const lux::FloatVolumeBase& f, const lux::Vector& c, double s,  int v, int p)   { return lux::DSMBase(new lux::DeepShadowMap(l, m, f, c, s, v, p));};