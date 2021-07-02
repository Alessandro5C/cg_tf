#pragma once
#ifndef __VAO__
#define __VAO__

#include "typedef.h"

class Vao{
private:
	u32 id;
public:
	Vao(bool initial_bind = true) {
		glGenVertexArrays(1, &id);
		if(initial_bind) Bind();
	}
	~Vao() {
		glDeleteVertexArrays(1, &id);
	}
	
	void SetVertexAttributes(i32 location, i32 nset, ui32 stride, i32 starting_point) {
		glVertexAttribPointer(location, nset, GL_FLOAT, GL_FALSE, stride, (void*)(starting_point*sizeof(float)));
		glEnableVertexAttribArray(location);
	}

	void Unbind() {
		glBindVertexArray(0);
	}
	void Bind() {
		glBindVertexArray(id);
	}
};

#endif