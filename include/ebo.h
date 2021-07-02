#pragma once
#ifndef __EBO__
#define __EBO__

#include "buffer.h"

class Ebo : public Buffer {
public:
	Ebo(bool initial_bind = true) {
		if (initial_bind) Bind();
	}
	~Ebo() {
		glDeleteBuffers(1, &id);
	}

	void SetBufferData(i32 size, i32* data) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	void Bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
	void Unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

#endif