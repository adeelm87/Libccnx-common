/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.8
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */


#ifndef SWIGJAVA
#define SWIGJAVA
#endif


/* -----------------------------------------------------------------------------
 *  This section contains generic SWIG labels for method/variable
 *  declarations/attributes, and other compiler dependent labels.
 * ----------------------------------------------------------------------------- */

/* template workaround for compilers that cannot correctly implement the C++ standard */
#ifndef SWIGTEMPLATEDISAMBIGUATOR
# if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x560)
#  define SWIGTEMPLATEDISAMBIGUATOR template
# elif defined(__HP_aCC)
/* Needed even with `aCC -AA' when `aCC -V' reports HP ANSI C++ B3910B A.03.55 */
/* If we find a maximum version that requires this, the test would be __HP_aCC <= 35500 for A.03.55 */
#  define SWIGTEMPLATEDISAMBIGUATOR template
# else
#  define SWIGTEMPLATEDISAMBIGUATOR
# endif
#endif

/* inline attribute */
#ifndef SWIGINLINE
# if defined(__cplusplus) || (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#   define SWIGINLINE inline
# else
#   define SWIGINLINE
# endif
#endif

/* attribute recognised by some compilers to avoid 'unused' warnings */
#ifndef SWIGUNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define SWIGUNUSED __attribute__ ((__unused__))
#   else
#     define SWIGUNUSED
#   endif
# elif defined(__ICC)
#   define SWIGUNUSED __attribute__ ((__unused__))
# else
#   define SWIGUNUSED
# endif
#endif

#ifndef SWIG_MSC_UNSUPPRESS_4505
# if defined(_MSC_VER)
#   pragma warning(disable : 4505) /* unreferenced local function has been removed */
# endif
#endif

#ifndef SWIGUNUSEDPARM
# ifdef __cplusplus
#   define SWIGUNUSEDPARM(p)
# else
#   define SWIGUNUSEDPARM(p) p SWIGUNUSED
# endif
#endif

/* internal SWIG method */
#ifndef SWIGINTERN
# define SWIGINTERN static SWIGUNUSED
#endif

/* internal inline SWIG method */
#ifndef SWIGINTERNINLINE
# define SWIGINTERNINLINE SWIGINTERN SWIGINLINE
#endif

/* exporting methods */
#if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#  ifndef GCC_HASCLASSVISIBILITY
#    define GCC_HASCLASSVISIBILITY
#  endif
#endif

#ifndef SWIGEXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define SWIGEXPORT
#   else
#     define SWIGEXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define SWIGEXPORT __attribute__ ((visibility("default")))
#   else
#     define SWIGEXPORT
#   endif
# endif
#endif

/* calling conventions for Windows */
#ifndef SWIGSTDCALL
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   define SWIGSTDCALL __stdcall
# else
#   define SWIGSTDCALL
# endif
#endif

/* Deal with Microsoft's attempt at deprecating C standard runtime functions */
#if !defined(SWIG_NO_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE
#endif

/* Deal with Microsoft's attempt at deprecating methods in the standard C++ library */
#if !defined(SWIG_NO_SCL_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_SCL_SECURE_NO_DEPRECATE)
# define _SCL_SECURE_NO_DEPRECATE
#endif

/* Deal with Apple's deprecated 'AssertMacros.h' from Carbon-framework */
#if defined(__APPLE__) && !defined(__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES)
# define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#endif

/* Intel's compiler complains if a variable which was never initialised is
 * cast to void, which is a common idiom which we use to indicate that we
 * are aware a variable isn't used.  So we just silence that warning.
 * See: https://github.com/swig/swig/issues/192 for more discussion.
 */
#ifdef __INTEL_COMPILER
# pragma warning disable 592
#endif


/* Fix for jlong on some versions of gcc on Windows */
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
  typedef long long __int64;
#endif

/* Fix for jlong on 64-bit x86 Solaris */
#if defined(__x86_64)
# ifdef _LP64
#   undef _LP64
# endif
#endif

#include <jni.h>
#include <stdlib.h>
#include <string.h>


/* Support for throwing Java exceptions */
typedef enum {
  SWIG_JavaOutOfMemoryError = 1, 
  SWIG_JavaIOException, 
  SWIG_JavaRuntimeException, 
  SWIG_JavaIndexOutOfBoundsException,
  SWIG_JavaArithmeticException,
  SWIG_JavaIllegalArgumentException,
  SWIG_JavaNullPointerException,
  SWIG_JavaDirectorPureVirtual,
  SWIG_JavaUnknownError
} SWIG_JavaExceptionCodes;

typedef struct {
  SWIG_JavaExceptionCodes code;
  const char *java_exception;
} SWIG_JavaExceptions_t;


static void SWIGUNUSED SWIG_JavaThrowException(JNIEnv *jenv, SWIG_JavaExceptionCodes code, const char *msg) {
  jclass excep;
  static const SWIG_JavaExceptions_t java_exceptions[] = {
    { SWIG_JavaOutOfMemoryError, "java/lang/OutOfMemoryError" },
    { SWIG_JavaIOException, "java/io/IOException" },
    { SWIG_JavaRuntimeException, "java/lang/RuntimeException" },
    { SWIG_JavaIndexOutOfBoundsException, "java/lang/IndexOutOfBoundsException" },
    { SWIG_JavaArithmeticException, "java/lang/ArithmeticException" },
    { SWIG_JavaIllegalArgumentException, "java/lang/IllegalArgumentException" },
    { SWIG_JavaNullPointerException, "java/lang/NullPointerException" },
    { SWIG_JavaDirectorPureVirtual, "java/lang/RuntimeException" },
    { SWIG_JavaUnknownError,  "java/lang/UnknownError" },
    { (SWIG_JavaExceptionCodes)0,  "java/lang/UnknownError" }
  };
  const SWIG_JavaExceptions_t *except_ptr = java_exceptions;

  while (except_ptr->code != code && except_ptr->code)
    except_ptr++;

  (*jenv)->ExceptionClear(jenv);
  excep = (*jenv)->FindClass(jenv, except_ptr->java_exception);
  if (excep)
    (*jenv)->ThrowNew(jenv, excep, msg);
}


/* Contract support */

#define SWIG_contract_assert(nullreturn, expr, msg) if (!(expr)) {SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, msg); return nullreturn; } else


#include "ccnx_Name.h"


#include <stdint.h>		// Use the C99 official header


void ccnxName_Release1(CCNxName *nameP) {
     ccnxName_Release(&nameP);
}


#ifdef __cplusplus
extern "C" {
#endif

SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1CreateFromCString(JNIEnv *jenv, jclass jcls, jstring jarg1) {
  jlong jresult = 0 ;
  char *arg1 = (char *) 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = 0;
  if (jarg1) {
    arg1 = (char *)(*jenv)->GetStringUTFChars(jenv, jarg1, 0);
    if (!arg1) return 0;
  }
  result = (CCNxName *)ccnxName_CreateFromCString((char const *)arg1);
  *(CCNxName **)&jresult = result; 
  if (arg1) (*jenv)->ReleaseStringUTFChars(jenv, jarg1, (const char *)arg1);
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1FromURI(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jlong jresult = 0 ;
  PARCURI *arg1 = (PARCURI *) 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(PARCURI **)&jarg1; 
  result = (CCNxName *)ccnxName_FromURI((PARCURI const *)arg1);
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1CreateFormatString(JNIEnv *jenv, jclass jcls, jstring jarg1) {
  jlong jresult = 0 ;
  char *arg1 = (char *) 0 ;
  void *arg2 = 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = 0;
  if (jarg1) {
    arg1 = (char *)(*jenv)->GetStringUTFChars(jenv, jarg1, 0);
    if (!arg1) return 0;
  }
  result = (CCNxName *)ccnxName_CreateFormatString((char const *)arg1,arg2);
  *(CCNxName **)&jresult = result; 
  if (arg1) (*jenv)->ReleaseStringUTFChars(jenv, jarg1, (const char *)arg1);
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1CreateFromBuffer(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jlong jresult = 0 ;
  PARCBuffer *arg1 = (PARCBuffer *) 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(PARCBuffer **)&jarg1; 
  result = (CCNxName *)ccnxName_CreateFromBuffer((PARCBuffer const *)arg1);
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1Acquire(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  result = (CCNxName *)ccnxName_Acquire((struct ccnx_name const *)arg1);
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT void JNICALL Java_ccnx_common_CcnxName_ccnxName_1Release(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  CCNxName **arg1 = (CCNxName **) 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName ***)&jarg1; 
  ccnxName_Release(arg1);
}


SWIGEXPORT void JNICALL Java_ccnx_common_CcnxName_ccnxName_1AssertValid(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  CCNxName *arg1 = (CCNxName *) 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  ccnxName_AssertValid((struct ccnx_name const *)arg1);
}


SWIGEXPORT jboolean JNICALL Java_ccnx_common_CcnxName_ccnxName_1IsValid(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jboolean jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  bool result;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  result = (bool)ccnxName_IsValid((struct ccnx_name const *)arg1);
  jresult = (jboolean)result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1BuildString(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  PARCBufferComposer *arg2 = (PARCBufferComposer *) 0 ;
  PARCBufferComposer *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = *(PARCBufferComposer **)&jarg2; 
  result = (PARCBufferComposer *)ccnxName_BuildString((struct ccnx_name const *)arg1,arg2);
  *(PARCBufferComposer **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jstring JNICALL Java_ccnx_common_CcnxName_ccnxName_1ToString(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jstring jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  char *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  result = (char *)ccnxName_ToString((struct ccnx_name const *)arg1);
  if (result) jresult = (*jenv)->NewStringUTF(jenv, (const char *)result);
  return jresult;
}


SWIGEXPORT void JNICALL Java_ccnx_common_CcnxName_ccnxName_1Display(JNIEnv *jenv, jclass jcls, jlong jarg1, jint jarg2) {
  CCNxName *arg1 = (CCNxName *) 0 ;
  int arg2 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = (int)jarg2; 
  ccnxName_Display((struct ccnx_name const *)arg1,arg2);
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1Append(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  CCNxNameSegment *arg2 = (CCNxNameSegment *) 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = *(CCNxNameSegment **)&jarg2; 
  result = (CCNxName *)ccnxName_Append(arg1,(CCNxNameSegment const *)arg2);
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jboolean JNICALL Java_ccnx_common_CcnxName_ccnxName_1StartsWith(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jboolean jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  CCNxName *arg2 = (CCNxName *) 0 ;
  bool result;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = *(CCNxName **)&jarg2; 
  result = (bool)ccnxName_StartsWith((struct ccnx_name const *)arg1,(struct ccnx_name const *)arg2);
  jresult = (jboolean)result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1Copy(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  result = (CCNxName *)ccnxName_Copy((struct ccnx_name const *)arg1);
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jboolean JNICALL Java_ccnx_common_CcnxName_ccnxName_1Equals(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jboolean jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  CCNxName *arg2 = (CCNxName *) 0 ;
  bool result;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = *(CCNxName **)&jarg2; 
  result = (bool)ccnxName_Equals((struct ccnx_name const *)arg1,(struct ccnx_name const *)arg2);
  jresult = (jboolean)result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1Create(JNIEnv *jenv, jclass jcls) {
  jlong jresult = 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  result = (CCNxName *)ccnxName_Create();
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jint JNICALL Java_ccnx_common_CcnxName_ccnxName_1Compare(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jint jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  CCNxName *arg2 = (CCNxName *) 0 ;
  int result;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = *(CCNxName **)&jarg2; 
  result = (int)ccnxName_Compare((struct ccnx_name const *)arg1,(struct ccnx_name const *)arg2);
  jresult = (jint)result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1GetSegment(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  size_t arg2 ;
  CCNxNameSegment *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = (size_t)jarg2; 
  result = (CCNxNameSegment *)ccnxName_GetSegment((struct ccnx_name const *)arg1,arg2);
  *(CCNxNameSegment **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1GetSegmentCount(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  size_t result;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  result = ccnxName_GetSegmentCount((struct ccnx_name const *)arg1);
  jresult = (jlong)result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1HashCode(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  PARCHashCode result;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  result = ccnxName_HashCode((struct ccnx_name const *)arg1);
  {
    PARCHashCode * resultptr = (PARCHashCode *) malloc(sizeof(PARCHashCode));
    memmove(resultptr, &result, sizeof(PARCHashCode));
    *(PARCHashCode **)&jresult = resultptr;
  }
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1LeftMostHashCode(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  size_t arg2 ;
  PARCHashCode result;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = (size_t)jarg2; 
  result = ccnxName_LeftMostHashCode((struct ccnx_name const *)arg1,arg2);
  {
    PARCHashCode * resultptr = (PARCHashCode *) malloc(sizeof(PARCHashCode));
    memmove(resultptr, &result, sizeof(PARCHashCode));
    *(PARCHashCode **)&jresult = resultptr;
  }
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1Trim(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  size_t arg2 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = (size_t)jarg2; 
  result = (CCNxName *)ccnxName_Trim(arg1,arg2);
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1ComposeNAME(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  char *arg2 = (char *) 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = 0;
  if (jarg2) {
    arg2 = (char *)(*jenv)->GetStringUTFChars(jenv, jarg2, 0);
    if (!arg2) return 0;
  }
  result = (CCNxName *)ccnxName_ComposeNAME((struct ccnx_name const *)arg1,(char const *)arg2);
  *(CCNxName **)&jresult = result; 
  if (arg2) (*jenv)->ReleaseStringUTFChars(jenv, jarg2, (const char *)arg2);
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1ComposeFormatString(JNIEnv *jenv, jclass jcls, jlong jarg1, jstring jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  char *arg2 = (char *) 0 ;
  void *arg3 = 0 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = 0;
  if (jarg2) {
    arg2 = (char *)(*jenv)->GetStringUTFChars(jenv, jarg2, 0);
    if (!arg2) return 0;
  }
  result = (CCNxName *)ccnxName_ComposeFormatString((struct ccnx_name const *)arg1,(char const *)arg2,arg3);
  *(CCNxName **)&jresult = result; 
  if (arg2) (*jenv)->ReleaseStringUTFChars(jenv, jarg2, (const char *)arg2);
  return jresult;
}


SWIGEXPORT jlong JNICALL Java_ccnx_common_CcnxName_ccnxName_1CreatePrefix(JNIEnv *jenv, jclass jcls, jlong jarg1, jlong jarg2) {
  jlong jresult = 0 ;
  CCNxName *arg1 = (CCNxName *) 0 ;
  size_t arg2 ;
  CCNxName *result = 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  arg2 = (size_t)jarg2; 
  result = (CCNxName *)ccnxName_CreatePrefix((struct ccnx_name const *)arg1,arg2);
  *(CCNxName **)&jresult = result; 
  return jresult;
}


SWIGEXPORT void JNICALL Java_ccnx_common_CcnxName_ccnxName_1Release1(JNIEnv *jenv, jclass jcls, jlong jarg1) {
  CCNxName *arg1 = (CCNxName *) 0 ;
  
  (void)jenv;
  (void)jcls;
  arg1 = *(CCNxName **)&jarg1; 
  ccnxName_Release1(arg1);
}


#ifdef __cplusplus
}
#endif
