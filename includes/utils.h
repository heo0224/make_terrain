#ifndef __UTILS_H__
#define __UTILS_H__

#include "common.h"

unsigned int generatePositionVAO(const float* vertices, unsigned int vertexSize);
unsigned int generatePositionTextureVAO(const float* vertices, unsigned int vertexSize);
unsigned int generatePositionTextureVAOWithEBO(const float* vertices, unsigned int vertexSize, const unsigned int* indices, unsigned int indexSize);



#endif // __UTILS_H__