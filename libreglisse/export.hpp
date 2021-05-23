#pragma once

// Normally we don't export class templates (but do complete specializations),
// inline functions, and classes with only inline member functions. Exporting
// classes that inherit from non-exported/imported bases (e.g., std::string)
// will end up badly. The only known workarounds are to not inherit or to not
// export. Also, MinGW GCC doesn't like seeing non-exported functions being
// used before their inline definition. The workaround is to reorder code. In
// the end it's all trial and error.

#if defined(LIBREGLISSE_STATIC)         // Using static.
#  define LIBREGLISSE_API
#elif defined(LIBREGLISSE_STATIC_BUILD) // Building static.
#  define LIBREGLISSE_API
#elif defined(LIBREGLISSE_SHARED)       // Using shared.
#  ifdef _WIN32
#    define LIBREGLISSE_API __declspec(dllimport)
#  else
#    define LIBREGLISSE_API
#  endif
#elif defined(LIBREGLISSE_SHARED_BUILD) // Building shared.
#  ifdef _WIN32
#    define LIBREGLISSE_API __declspec(dllexport)
#  else
#    define LIBREGLISSE_API
#  endif
#else
// If none of the above macros are defined, then we assume we are being used
// by some third-party build system that cannot/doesn't signal the library
// type. Note that this fallback works for both static and shared libraries
// provided the library only exports functions (in other words, no global
// exported data) and for the shared case the result will be sub-optimal
// compared to having dllimport. If, however, your library does export data,
// then you will probably want to replace the fallback with the (commented
// out) error since it won't work for the shared case.
//
#  define LIBREGLISSE_API         // Using static or shared.
//#  error define LIBREGLISSE_STATIC or LIBREGLISSE_SHARED preprocessor macro to signal hello library type being linked
#endif

