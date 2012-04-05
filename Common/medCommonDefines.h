#ifndef __medCommonDefines_h
#define __medCommonDefines_h

//--------------------------------------------------------------------------
// For WIN32 dynamic linking
//--------------------------------------------------------------------------
#if defined(_WIN32) && defined(MED_SHARED)
# if defined(MED_EXPORTS)
#  define MED_COMMON_EXPORT __declspec( dllexport ) 
# else
#  define MED_COMMON_EXPORT __declspec( dllimport ) 
# endif
#else
# define MED_COMMON_EXPORT
#endif

#endif // __medCommonDefines_h