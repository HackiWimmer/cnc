#ifndef OPENGL_CNC_PATH_DATA_H
#define OPENGL_CNC_PATH_DATA_H

#include <vector>

struct GLCncPathVertices {
	long x;
	long y;
	long z;
};

typedef std::vector<GLCncPathVertices> GLCncPath;

#endif