
%module voyager
%{
#include "renderlog.h"
%}

%include "std_vector.i"
%include "std_string.i"
namespace std
{
%template(StringArray)      vector<string>;
}
%include "renderlog.h"
