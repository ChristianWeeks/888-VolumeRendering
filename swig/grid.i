%module voyager
%{
#include "grid.h"
%}

%shared_ptr(FloatGridPtr);
%shared_ptr(DSMPtr);

%include "grid.h"

