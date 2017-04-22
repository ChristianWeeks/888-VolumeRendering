
#include "fields.h"

//------------------------------------------------------------------------------------------------------------------------------
//IMPLICIT VOLUMES
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Constantf(const float value)               { return lux::FloatVolumeBase(new lux::ConstantVolumef(value));};
lux::VectorVolumeBase Constantv(const float x, const float y, const float z){return lux::VectorVolumeBase(new lux::ConstantVolumev(x, y, z));};
lux::FloatVolumeBase Sphere(const float rad)                    { return lux::FloatVolumeBase(new lux::SphereVolume(rad));};
lux::FloatVolumeBase Spheredf(const float rad)                    { return lux::FloatVolumeBase(new lux::SphereDistVolume(rad));};
lux::FloatVolumeBase Box(const float rad, const float c)        { return lux::FloatVolumeBase(new lux::BoxVolume(rad, c));};
lux::FloatVolumeBase Cylinder(const lux::Vector& n, const float rad) { return lux::FloatVolumeBase(new lux::CylinderVolume(n, rad));};
lux::FloatVolumeBase Plane(const lux::Vector& n, const lux::Vector& c)    { return lux::FloatVolumeBase(new lux::PlaneVolume(n, c));};
lux::FloatVolumeBase Cone(const lux::Vector& n, const float h, const float ang)    { return lux::FloatVolumeBase(new lux::ConeVolume(n, h, ang));};
lux::FloatVolumeBase Torus(const lux::Vector& n, const float rMaj, const float rMin){return lux::FloatVolumeBase(new lux::TorusVolume(n, rMaj, rMin));};
lux::FloatVolumeBase Icosohedron(){return lux::FloatVolumeBase(new lux::IcosohedronVolume());};
lux::FloatVolumeBase SteinerPatch(){return lux::FloatVolumeBase(new lux::SteinerPatchVolume());};

lux::FloatVolumeBase Noisef(SimplexNoiseObject s)               { return lux::FloatVolumeBase(new lux::SimplexNoiseVolume(s));};
lux::FloatVolumeBase NoiseSpheref(const lux::Vector& pos, const SimplexNoiseObject s, const float rad, const float f)   {return lux::FloatVolumeBase(new lux::NoiseSphere(pos, s, rad, f));};

lux::FloatVolumeBase PyroSphere(const float rad, const float d, const float e, const float ds, SimplexNoiseObject s)    {return lux::FloatVolumeBase(new lux::MaskVolume(new lux::PyroSphereVolume(rad, d, e, ds, s)));};
lux::FloatVolumeBase PyroSphereSDF(const float rad, const float d, const float e, const float ds, SimplexNoiseObject s) {return lux::FloatVolumeBase(new lux::PyroSphereVolume(rad, d, e, ds, s));};
lux::FloatVolumeBase PyroLevelSet(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase g, const float e, const float ds, SimplexNoiseObject s){ return lux::FloatVolumeBase(new lux::PyroLevelSetVolume(f, g, e, ds, s));};

lux::FloatVolumeBase Gradientf(const lux::Vector& d, const lux::Vector& start, const float l, const float mn, const float mx){ return lux::FloatVolumeBase(new lux::FloatGradientVolume(d, start, l, mn, mx));};
lux::VectorVolumeBase Noisev(SimplexNoiseObject s, const float xO, const float yO, const float zO)      {return lux::VectorVolumeBase(new lux::SimplexNoiseVectorVolume(s, xO, yO, zO));};
lux::VectorVolumeBase Radialv(const lux::Vector& origin, const int normalized, const float magnitude)   {return lux::VectorVolumeBase(new lux::RadialVectorVolume(origin, normalized, magnitude));};
lux::VectorVolumeBase Identityv(){return lux::VectorVolumeBase(new lux::IdentityVectorVolume());};

lux::ColorVolumeBase Noisec(SimplexNoiseObject s, const float xO, const float yO, const float zO)       {return lux::ColorVolumeBase(new lux::SimplexNoiseColorVolume(s, xO, yO, zO));};

//------------------------------------------------------------------------------------------------------------------------------
// TRANSFORMATIONS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Translatef( const lux::FloatVolumeBase& f,  const lux::Vector& d)       { return lux::FloatVolumeBase(new lux::TranslateVolumef(f, d));};
lux::VectorVolumeBase Translatev( const lux::VectorVolumeBase& f,  const lux::Vector& d)       { return lux::VectorVolumeBase(new lux::TranslateVolumev(f, d));};
lux::FloatVolumeBase Scalef( const lux::FloatVolumeBase& f,  const lux::Vector& p, const float s){return lux::FloatVolumeBase(new lux::Scale_S_Volume(f, p, s));};
lux::FloatVolumeBase Rotatef(const lux::FloatVolumeBase f, const lux::Vector& p, const lux::Vector axis, float ang){return lux::FloatVolumeBase(new lux::Rotate_S_Volume(f, p, axis, ang));};

//------------------------------------------------------------------------------------------------------------------------------
// OPERATORS
//------------------------------------------------------------------------------------------------------------------------------
lux::FloatVolumeBase Addf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)       { return lux::FloatVolumeBase(new lux::AddVolumef(f, g));};
lux::FloatVolumeBase Subtractf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)  { return lux::FloatVolumeBase(new lux::SubtractVolumef(f, g));};
lux::FloatVolumeBase Multf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)      { return lux::FloatVolumeBase(new lux::MultVolumef(f, g));};
lux::VectorVolumeBase Multvf(const lux::FloatVolumeBase& f, const lux::VectorVolumeBase& g)      { return lux::VectorVolumeBase(new lux::Mult_SV_Volume(f, g));};
lux::FloatVolumeBase AdvectSLf( const lux::FloatVolumeBase& f,  const lux::VectorVolumeBase& g, const float t)  {return lux::FloatVolumeBase(new lux::Advect_SL_Volume(f, g, t));};
lux::VectorVolumeBase AdvectSLv( const lux::VectorVolumeBase& f, const lux::VectorVolumeBase& g, const float t)  {return lux::VectorVolumeBase(new lux::AdvectSLVectorVolume(f, g, t));};
lux::FloatVolumeBase AdvectMMCf( const lux::FloatVolumeBase& f,  const lux::VectorVolumeBase& g, const float t) {return lux::FloatVolumeBase(new lux::Advect_MMC_Volume(f, g, t));};
lux::FloatVolumeBase Warpf( const lux::FloatVolumeBase& f,  const lux::VectorVolumeBase& g)  {return lux::FloatVolumeBase(new lux::WarpVolume(f, g));};

lux::FloatVolumeBase Unionf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)         { return lux::FloatVolumeBase(new lux::UnionVolumef(f, g));};
lux::FloatVolumeBase Intersectf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)     { return lux::FloatVolumeBase(new lux::IntersectVolumef(f, g));};
lux::FloatVolumeBase Cutoutf( const lux::FloatVolumeBase& f,  const lux::FloatVolumeBase& g)        { return lux::FloatVolumeBase(new lux::CutoutVolumef(f, g));};
lux::FloatVolumeBase Shellf(const lux::FloatVolumeBase& f, const float w)                           { return lux::FloatVolumeBase(new lux::ShellVolumef(f, w));};
lux::FloatVolumeBase Maskf( const lux::FloatVolumeBase& f)                                    { return lux::FloatVolumeBase(new lux::MaskVolume(f));};
lux::FloatVolumeBase Clampf( const lux::FloatVolumeBase& f, const float Min, const float Max) { return lux::FloatVolumeBase(new lux::ClampVolume(f, Min, Max));};
lux::FloatVolumeBase Reducef(const lux::FloatVolumeBase& f, const float cutoff){ return lux::FloatVolumeBase(new lux::ReduceVolume(f, cutoff));};

lux::VectorVolumeBase Addv( const lux::VectorVolumeBase& f,  const lux::VectorVolumeBase& g) { return lux::VectorVolumeBase(new lux::AddVolumev(f, g));};
lux::VectorVolumeBase Subtractv( const lux::VectorVolumeBase& f,  const lux::VectorVolumeBase& g) { return lux::VectorVolumeBase(new lux::SubtractVolumev(f, g));};

lux::ColorVolumeBase ColorFromDensity(const lux::FloatVolumeBase& f, const lux::ColorSlider& cSlider, float min, float max) { return lux::ColorVolumeBase(new lux::ColorVolumeFromDensity(f, cSlider, min, max));};
//------------------------------------------------------------------------------------------------------------------------------
// GRIDS
//-----------------------------------------------------------------------------------------------------------------------------
lux::FloatGridBase Gridf(const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int p) { return lux::FloatGridBase(new lux::DensityGrid(f, c, s, vx, vy, vz, p));};
lux::VectorGridBase Gridv(const lux::VectorVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int p){ return lux::VectorGridBase(new lux::VectorGrid(f, c, s, vx, vy, vz, p));};
lux::VectorGridBase GradientGrid(const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int p) { return lux::VectorGridBase(new lux::VectorGrid(f, c, s, vx, vy, vz, p));};
lux::FloatGridBase GridConstf(float initValue, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int p) { return lux::FloatGridBase(new lux::DensityGrid(initValue, c, s, vx, vy, vz, p));};
lux::FloatGridBase FrustumGridf(const float f, const lux::Camera& cam, int vx, int vy, int vz, int p) { return lux::FloatGridBase(new lux::FrustumGrid(f, cam, vx, vy, vz, p));};

lux::FloatVolumeBase GriddedVolf(const lux::FloatGridBase& g)   {
    lux::FloatVolumeBase gridded(new lux::GriddedVolume(g));
    gridded.BB.setBounds(g->center, g->length);
    return gridded;
};
lux::VectorVolumeBase GriddedVolv(const lux::VectorGridBase& g)   {
    lux::VectorVolumeBase gridded(new lux::GriddedVolumev(g));
    gridded.BB.setBounds(g->center, g->length);
    return gridded;
};

lux::FloatVolumeBase AutoGriddedf(const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s, int vx, int vy, int vz, int p) {
    lux::FloatVolumeBase gridded( new lux::GriddedVolume(lux::FloatGridBase(new lux::DensityGrid(f, c, s, vx, vy, vz, p))));
    gridded.BB.setBounds(c, s);
    return gridded;
};
lux::DSMBase DSM(const lux::light& l, float m, const lux::FloatVolumeBase& f, const lux::Vector& c, const lux::Vector& s,  int vx, int vy, int vz, int p, std::vector<BoundingBox> BBs)   { return lux::DSMBase(new lux::DeepShadowMap(l, m, f, c, s, vx, vy, vz, p, BBs));};
