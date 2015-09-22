#ifndef _COMMON_H_
#define _COMMON_H_

#define _USE_MATH_DEFINES

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <math.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define M_PIf 3.141592653589f
#define DEG_TO_RAD(a) ((a) * M_PIf / 180.0f)
#define RAD_TO_DEG(a) ((a) / M_PIf * 180.0f)

#endif // _COMMON_H_