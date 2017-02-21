%module (jniclassname="CcnxName") test
%{
#include "ccnx_Name.h"
%}

%include "stdint.i"

%include "ccnx_Name.h"

/* Helper Functions */
%inline %{
void ccnxName_Release1(CCNxName *nameP) {
     ccnxName_Release(&nameP);
}
%}

