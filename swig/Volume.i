
%module voyager
%{
#include "../grid.h"
%include "../volume_implicits.h"
%}

/*%include <std_shared_ptr.i>
%shared_ptr(lux::Volume<float>)
%shared_ptr(lux::Volume<lux::Vector>)
%shared_ptr(lux::Volume<Matrix>)*/

%include "../grid.h"
%include "../volume_implicits.h"
%template(Volumef) lux::Volume<float>;
%template(Volumev) lux::Volume<lux::Vector>;
%template(ConstantVolumef) lux::ConstantVolume<float>;
%template(ConstantVolumev) lux::ConstantVolume<lux::Vector>;
%template(SphereVolumef) lux::SphereVolume<float>;
%template(SphereVolumev) lux::SphereVolume<lux::Vector>;


/*%rename(multSVVolume) lux::MultScalVecVolume<lux::Vector>;
%rename(divideSVVolume) lux::DivideScalVecVolume<lux::Vector>;

%rename(sinSVolume) lux::SinVolume<float>;
%rename(cosSVolume) lux::CosVolume<float>;

%rename(dotSVolume) lux::DotProductVolume<float>;*/

