
%module voyager
%{
#include "volume_implicits.h"
%}

%include <std_shared_ptr.i>
%shared_ptr(FloatVolumePtr);
%shared_ptr(VectorVolumePtr);
%shared_ptr(ColorVolumePtr);

%include "volume_implicits.h"
