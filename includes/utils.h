#ifndef __UTILS_H__
#define __UTILS_H__

#include "common.h"

unsigned int generatePositionVAO(const float* vertices, unsigned int vertexSize);
unsigned int generatePositionVAO(const std::vector<float>& vertices);
unsigned int generatePositionTextureVAO(const float* vertices, unsigned int vertexSize);
unsigned int generatePositionTextureVAO(const std::vector<float>& vertices);
unsigned int generatePositionTextureVAOWithEBO(const float* vertices, unsigned int vertexSize, const unsigned int* indices, unsigned int indexSize);
unsigned int generatePositionTextureVAOWithEBO(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);



#endif // __UTILS_H__