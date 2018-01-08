#ifndef _FLV_COMMON_H
#define _FLV_COMMON_H
#include <stdint.h>
#ifdef LIBFLV_EXPORTS
#define LIBFLV_API __declspec(dllexport)
#else
#define LIBFLV_API __declspec(dllimport)
#endif

#endif //_FLV_COMMON_H
