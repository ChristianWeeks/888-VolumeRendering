%module voyager
%{
#include "SceneManager.h"
%}

%include "std_vector.i"
namespace std
{
%template(FloatArray)       vector<float>;
%template(ColorArray)       vector<lux::Color>;
%template(FloatVolumeArray) vector<lux::FloatVolumeBase>;
%template(ColorVolumeArray) vector<lux::ColorVolumeBase>;
}

%include "SceneManager.h"

