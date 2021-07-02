#pragma once
#ifndef __CUBE__
#define __CUBE__

#include "typedef.h"

struct Rgb {
	f32 r, g, b;
};

class Object {

protected:
	f32* vertices;
	i32* indices;
	i32  nv;
	i32  ni;

public:
	Object(i32 n, i32 m)
		: vertices(new f32[n]), indices(new i32[m]), nv(n), ni(m) {}

	f32* getVertices() {
		return vertices;
	}
	i32* getIndices() {
		return indices;
	}
	i32 getVSize() {
		return nv * FSIZE;
	}
	i32 getISize() {
		return ni * ISIZE;
	}
};

class Cube : public Object {
	f32  width;
	f32  height;
	f32  depth;

	// frente, arriba, derecha, abajo, izquierda, atras 
	Rgb a, b, c, d, e, f;

public:
	Cube(f32 width = 1.0f, f32 height = 1.0f, f32 depth = 1.0f):
		Object(24 * 11, 6 * 6), width(width), height(height), depth(depth), 
		a({ 1.0f, 0.0f, 0.0f }), b({ 1.0f, 0.0f, 0.0f }),
		c({ 1.0f, 0.0f, 0.0f }), d({ 0.0f, 0.0f, 1.0f }),
		e({ 1.0f, 0.0f, 0.0f }), f({ 0.0f, 1.0f, 0.0f }) {
		
		f32 wm = width / 2.0f;
		f32 hm = height / 2.0f;
		f32 dm = depth / 2.0f;
		f32 temp[] = {
			// posiciones    normales              colores          texturas
			-wm,  hm,  dm,   0.0f,  0.0f,  1.0f,   a.r, a.g, a.b,   0.0f, 1.0f,  // 0
			 wm,  hm,  dm,   0.0f,  0.0f,  1.0f,   a.r, a.g, a.b,   1.0f, 1.0f,  // 1
			-wm, -hm,  dm,   0.0f,  0.0f,  1.0f,   a.r, a.g, a.b,   0.0f, 0.0f,  // 2
			 wm, -hm,  dm,   0.0f,  0.0f,  1.0f,   a.r, a.g, a.b,   1.0f, 0.0f,  // 3

			-wm,  hm,  dm,   0.0f,  1.0f,  0.0f,   b.r, b.g, b.b,   0.0f, 1.0f,  // 4
			 wm,  hm,  dm,   0.0f,  1.0f,  0.0f,   b.r, b.g, b.b,   1.0f, 1.0f,  // 5
			-wm,  hm, -dm,   0.0f,  1.0f,  0.0f,   b.r, b.g, b.b,   0.0f, 0.0f,  // 6
			 wm,  hm, -dm,   0.0f,  1.0f,  0.0f,   b.r, b.g, b.b,   1.0f, 0.0f,  // 7

			 wm,  hm,  dm,   1.0f,  0.0f,  0.0f,   c.r, c.g, c.b,   1.0f, 1.0f,  // 8
			 wm,  hm, -dm,   1.0f,  0.0f,  0.0f,   c.r, c.g, c.b,   1.0f, 0.0f,  // 9
			 wm, -hm,  dm,   1.0f,  0.0f,  0.0f,   c.r, c.g, c.b,   0.0f, 1.0f,  // 10
			 wm, -hm, -dm,   1.0f,  0.0f,  0.0f,   c.r, c.g, c.b,   0.0f, 0.0f,  // 11

			-wm, -hm,  dm,   0.0f, -1.0f,  0.0f,   d.r, d.g, d.b,   0.0f, 1.0f,  // 12
			 wm, -hm,  dm,   0.0f, -1.0f,  0.0f,   d.r, d.g, d.b,   1.0f, 1.0f,  // 13
			-wm, -hm, -dm,   0.0f, -1.0f,  0.0f,   d.r, d.g, d.b,   0.0f, 0.0f,  // 14
			 wm, -hm, -dm,   0.0f, -1.0f,  0.0f,   d.r, d.g, d.b,   1.0f, 0.0f,  // 15

			-wm,  hm,  dm,  -1.0f,  0.0f,  0.0f,   e.r, e.g, e.b,   1.0f, 1.0f,  // 16
			-wm,  hm, -dm,  -1.0f,  0.0f,  0.0f,   e.r, e.g, e.b,   1.0f, 0.0f,  // 17
			-wm, -hm,  dm,  -1.0f,  0.0f,  0.0f,   e.r, e.g, e.b,   0.0f, 1.0f,  // 18
			-wm, -hm, -dm,  -1.0f,  0.0f,  0.0f,   e.r, e.g, e.b,   0.0f, 0.0f,  // 19

			-wm,  hm, -dm,   0.0f,  0.0f, -1.0f,   f.r, f.g, f.b,   0.0f, 1.0f,  // 20
			 wm,  hm, -dm,   0.0f,  0.0f, -1.0f,   f.r, f.g, f.b,   1.0f, 1.0f,  // 21
			-wm, -hm, -dm,   0.0f,  0.0f, -1.0f,   f.r, f.g, f.b,   0.0f, 0.0f,  // 22
			 wm, -hm, -dm,   0.0f,  0.0f, -1.0f,   f.r, f.g, f.b,   1.0f, 0.0f   // 23
		};
		for (u32 i = 0; i < nv; ++i) {
			vertices[i] = temp[i];
		}
		for (u32 i = 0; i < 6; ++i) {
			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 1;
			indices[i * 6 + 4] = i * 4 + 2;
			indices[i * 6 + 5] = i * 4 + 3;
		}
	}
};
#endif