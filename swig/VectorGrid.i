%module voyager
%{
#include "VectorGrid.h"
%}

%shared_ptr(VectorGridPtr);
%shared_ptr(DSMPtr);

%include "VectorGrid.h"

