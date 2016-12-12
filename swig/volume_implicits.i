
%module voyager
%{
#include "volume_implicits.h"
%}

/*%include <std_shared_ptr.i>
%shared_ptr(FloatVolumePtr);
%shared_ptr(VectorVolumePtr);
%shared_ptr(ColorVolumePtr);*/

%include "volume_implicits.h"
/*%template(FloatVolume) lux::Volume<float>;
%template(VectorVolume) lux::Volume<lux::Vector>;
%template(ColorVolume) lux::Volume<lux::Color>;*/

/*%template(FloatVolumePtr) std::shared_ptr<lux::Volume<float> > ;
%template(VectorVolumePtr) std::shared_ptr<lux::Volume<lux::Vector> >;
%template(ColorVolumePtr) std::shared_ptr<lux::Volume<lux::Color> >;*/

