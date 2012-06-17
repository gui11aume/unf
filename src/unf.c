#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#include "uniformity.h"

// Declare and register R/C interface.
SEXP
uniformity_R_call(
  SEXP mat
);

R_CallMethodDef callMethods[] = { 
    {"uniformity_R_call", (DL_FUNC) &uniformity_R_call, 1}, 
    {NULL, NULL, 0}
};

void R_init_uniformity(DllInfo *info) {
   R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

// Wrapper for unf_test.
SEXP
uniformity_R_call(
  SEXP mat
){

   double *p = REAL(mat);
   int *dim = INTEGER(getAttrib(mat, R_DimSymbol));
   SEXP return_val_sexp;
   PROTECT(return_val_sexp = allocVector(REALSXP, 1));
   double *return_val = REAL(return_val_sexp);
   
   return_val[0] = unf_test(NULL, p, dim[1], dim[0]);
   UNPROTECT(1);
   
   return return_val_sexp;

}
