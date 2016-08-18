/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Monadic when arguments have one atom

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// Support for Speedy Singletons
#define SSINGF1(f) A f(J jtf, A w){ J jt=(J)((I)jtf&-4); // header for function definition
#define SSINGF1OP(f) A f(J jtf, A w, I op){ J jt=(J)((I)jtf&-4);   // header for function definition

// An argument can be inplaced if it is enabled in the block AND in the call
#define WINPLACE ((I)jtf&1 && ACIPISOK(w))

#define SSRDB(w) (*(B *)CAV(w))
#define SSRDI(w) (*(I *)CAV(w))
#define SSRDD(w) (*(D *)CAV(w))
#define SSSTORE(v,z,t,type) {*((type *)CAV(z)) = (v); if((t)!=FL)AT(z)=(t);}

#define SSNUMPREFIX A z; I wt=AT(w);  \
/* Establish the output area.  If this operation is in-placeable, reuse an in-placeable operand */ \
/* If not, allocate a single FL block with the required rank/shape.  We will */ \
/* change the type of this block when we get the result type */ \
/* Try the zombiesym first, because if we use it the assignment is faster */ \
{I wr = AR(w);    /* get rank */ \
 if(jt->zombieval && AN(jt->zombieval)==1 && AR(jt->zombieval)>=wr){AR(jt->zombieval)=wr; AT(z=jt->zombieval)=FL;}  \
 else if (WINPLACE){ AT(z=w) = FL; } \
 else {GATV(z, FL, 1, wr, AS(w)); if((I)jtf&3)ACIPYES(z);} \
} /* We have the output block */


// speedy singleton routines: each argument has one atom.  The shapes may be
// any length, but we know they contain all 1s, so we don't care about jt->rank except to clear it
SSINGF1(jtssfloor) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f;
  default: R 0;
  case B01: SSSTORE(SSRDB(w),z,B01,B) R z;
  case INT: SSSTORE(SSRDI(w),z,INT,I) R z;
  case FL:
   f = tfloor(SSRDD(w));
   if(f == (D)(I)f) SSSTORE((I)f,z,INT,I) else SSSTORE(f,z,FL,D)
   R z;
 }
}

SSINGF1(jtssceil) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f;
  default: R 0;
  case B01: SSSTORE(SSRDB(w),z,B01,B) R z;
  case INT: SSSTORE(SSRDI(w),z,INT,I) R z;
  case FL:
   f = tceil(SSRDD(w));
   if(f == (D)(I)f) SSSTORE((I)f,z,INT,I) else SSSTORE(f,z,FL,D)
   R z;
 }
}

SSINGF1(jtsssignum) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f;
  default: R 0;
  case B01: SSSTORE(SSRDB(w),z,B01,B) R z;
  case INT: SSSTORE(SSRDI(w)>0?1:SSRDI(w)<0?-1:0,z,INT,I) R z;
  case FL:
   f = SSRDD(w);
   SSSTORE(f>=jt->ct?1:f<=-jt->ct?-1:0,z,INT,I)
   R z;
 }
}

SSINGF1(jtsssqrt) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f; I i;
  default: R 0;
  case B01: SSSTORE(SSRDB(w),z,INT,I) R z;  // normal code returns B01, but INT seems better
  case INT:
    if((i = SSRDI(w))>=0){SSSTORE(sqrt((D)i),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
  case FL:
    if((f = SSRDD(w))>=0){SSSTORE(sqrt(f),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 }
}

SSINGF1(jtssexp) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f; I i;
  default: R 0;
  case B01: SSSTORE(SSRDB(w)?2.71828182845904523536:1.0,z,FL,D) R z;
  case INT:
    i = SSRDI(w);
    SSSTORE(i<EMIN?0.0:EMAX<i?inf:exp((D)i),z,FL,D) R z;
  case FL:
    f = SSRDD(w);
    SSSTORE(f<EMIN?0.0:EMAX<f?inf:exp(f),z,FL,D) R z;
 }
}

SSINGF1(jtsslog) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f; I i;
  default: R 0;
  case B01: SSSTORE(SSRDB(w)?0.0:infm,z,FL,D) R z;
  case INT:
    if((i = SSRDI(w))>=0){SSSTORE(log((D)i),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
  case FL:
    if((f = SSRDD(w))>=0){SSSTORE(log(f),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 }
}

SSINGF1(jtssmag) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f; I i;
  default: R 0;
  case B01: SSSTORE(SSRDB(w),z,INT,I) R z;   // return INT rather than normal B01
  case INT:
    if((i = SSRDI(w))>IMIN){SSSTORE(i>=0?i:-i,z,INT,I)}else SSSTORE(-(D)IMIN,z,FL,D) R z;
  case FL:
    f = SSRDD(w); SSSTORE(ABS(f),z,FL,D) R z;
 }
}

SSINGF1(jtssfact) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {D f;
  default: R 0;
  case B01: SSSTORE(1,z,INT,I) R z;   // return INT rather than normal B01
  case INT:
    SSSTORE(dgamma(1.0+(D)SSRDI(w)),z,FL,D) RE(0) R z;
  case FL:
    f = SSRDD(w); SSSTORE(_isnan(f)?f:dgamma(1.0+f),z,FL,D) RE(0) R z;
 }
}

SSINGF1(jtsspix) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wt) {
  default: R 0;
  case B01: SSSTORE(SSRDB(w)?PI:0.0,z,FL,D) R z;
  case INT:
    SSSTORE(PI*(D)SSRDI(w),z,FL,D) R z;
  case FL:
    SSSTORE(PI*SSRDD(w),z,FL,D) R z;
 }
}
