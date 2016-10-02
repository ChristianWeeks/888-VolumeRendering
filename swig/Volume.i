
%module rook
%{
#include "Volume.h"
%}

%include <std_shared_ptr.i>
%shared_ptr(Volume<float>)
%shared_ptr(Volume<lux::Vector>)
%shared_ptr(Volume<Matrix>)

%include "Volume.h"
%template(floatVolume) lux::Volume<float>;
%template(vectorVolume) lux::Volume<lux::Vector>;
%template(MultVolumeFloat) lux::MultVolume<float>;

/*%rename(multSVVolume) lux::MultScalVecVolume<lux::Vector>;
%rename(divideSVVolume) lux::DivideScalVecVolume<lux::Vector>;

%rename(sinSVolume) lux::SinVolume<float>;
%rename(cosSVolume) lux::CosVolume<float>;

%rename(dotSVolume) lux::DotProductVolume<float>;*/

