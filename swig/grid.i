%module voyager
%{
#include "grid.h"
%}

%include "grid.h"

%shared_ptr(FloatGridPtr);
%shared_ptr(DSMPtr);
