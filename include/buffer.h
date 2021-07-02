#pragma once
#ifndef __BUFFER__
#define __BUFFER__

#include "typedef.h"

class Buffer {
protected:
	u32 id;
protected:
	Buffer() {
		glGenBuffers(1, &id);
	}
	~Buffer() {
	}

	virtual void Bind() = 0;
	virtual void Unbind() = 0; 
};
#endif