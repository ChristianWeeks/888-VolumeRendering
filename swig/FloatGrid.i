%module voyager
%{
#include "FloatGrid.h"
%}

%shared_ptr(FloatGridPtr);
%shared_ptr(DSMPtr);

%include "FloatGrid.h"

