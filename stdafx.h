#ifndef stdafxH
#define stdafxH

//opengl
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/gl.h>
#include <gl/glu.h>
//dos
#include <time.h>
#include <stdio.h>
#include <math.h>
//file stats
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//stl
#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <map>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;

//global log file variable - in stdafx.cpp
extern ofstream LogFile;

//sign template
template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

//typedefs
typedef void (*tVoidFunc) ();

//help function
unsigned int RandomChance(unsigned int n);
int RandomValue(int Min, int Max);
bool IsPowerOfTwo(unsigned long x);
bool CheckOpenglSupport();

//IMPORTANT!
#define SHR_LAYOUT_VERTEX 0
#define SHR_LAYOUT_COLOR 1

struct NVector2 {
	int x;
	int y;
	NVector2() {
		x = y = 0;
	}
	NVector2(int inX, int inY) {
		x = inX;
		y = inY;
	}
};

struct NLine2 {
	NVector2 a;
	NVector2 b;
	NLine2() {
		a = NVector2(0, 0);
		b = NVector2(0, 0);
	}
	NLine2(NVector2 inA, NVector2 inB) {
		a = inA;
		b = inB;
	}
};

struct NRectangle2 {
	NVector2 Position;
	NVector2 Size;
	NRectangle2() {
		Position = NVector2(0, 0);
		Size = NVector2(0, 0);
	}
	NRectangle2(NVector2 inPosition, NVector2 inSize) {
		Position = inPosition;
		Size = inSize;
	}
};

#endif
