#ifndef __UTILS_H__
#define __UTILS_H__

#include "common.h"

unsigned int generatePositionTextureVAO(const float* vertices, unsigned int vertexSize);
unsigned int generatePositionTextureVAOWithEBO(const float* vertices, unsigned int vertexSize, const unsigned int* indices, unsigned int indexSize);

class VAO{
public:
    unsigned int ID;
    unsigned int associatedVBOID;
    //std::vector<unsigned int> associatedVBOs;
};

VAO *getVAOFromAttribData(const std::vector<float> &attrib_data, const std::vector<unsigned int> &attrib_sizes);
void getPositionVAO(const float* vertices, unsigned int size, unsigned int& VAO, unsigned int& VBO);


#endif // __UTILS_H__