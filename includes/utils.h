#ifndef __UTILS_H__
#define __UTILS_H__

#include "common.h"

unsigned int generatePositionVAO(const float* vertices, unsigned int size);
unsigned int generatePositionTextureVAO(const float* vertices, unsigned int vertexSize);
unsigned int generatePositionTextureVAOWithEBO(const float* vertices, unsigned int vertexSize, const unsigned int* indices, unsigned int indexSize);

class VAO{
public:
    unsigned int ID;
    unsigned int associatedVBOID;
    //std::vector<unsigned int> associatedVBOs;
};

// Math utils
float clamp(float x, float x_min, float x_max);
#endif // __UTILS_H__