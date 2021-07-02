#pragma once
#ifndef __TYPEDEF__
#define __TYPEDEF__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <stb_image.h>
#include <path.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

typedef float  f32;
typedef double f64;

typedef char      i8;
typedef short int i16;
typedef int       i32;
typedef long long i64;

typedef unsigned char      uchar;
typedef unsigned short int ui16;
typedef unsigned int       ui32;
typedef unsigned long long ui64;

using namespace std;
using namespace glm;

#define FSIZE sizeof(f32)
#define ISIZE sizeof(i32)

#define _USE_MATH_DEFINES

#endif 