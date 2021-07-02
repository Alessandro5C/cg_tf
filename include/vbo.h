#pragma once
#ifndef __VBO__
#define __VBO__

#include "buffer.h"


class Vbo : public Buffer {
public:
	Vbo(bool initial_bind = true) {
		if (initial_bind) Bind();
	}
	~Vbo() {
		glDeleteBuffers(1, &id);
	}

	void SetBufferData(i32 size, f32* data) {
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	void Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}
	void Unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


};

#endif