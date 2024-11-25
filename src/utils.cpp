#include "utils.h"

unsigned int generatePositionTextureVAO(const float* vertices, unsigned int vertexSize) {
    unsigned int VAO, VBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

    // Vertex attribute pointers (assume positions and texture coordinates)
    // Positions (3 floats per vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates (2 floats per vertex)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO (optional)
    glBindVertexArray(0);

    return VAO;
}

unsigned int generatePositionTextureVAOWithEBO(const float* vertices, unsigned int vertexSize, const unsigned int* indices, unsigned int indexSize) {
    unsigned int VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);

    // Vertex attribute pointers (assume positions and texture coordinates)
    // Positions (3 floats per vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates (2 floats per vertex)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO (optional)
    glBindVertexArray(0);

    return VAO;
}

VAO *getVAOFromAttribData(const std::vector<float> &attrib_data, const std::vector<unsigned int> &attrib_sizes)
{
    VAO *vao = new VAO();
    //unsigned int VBO;
    glGenVertexArrays(1, &(vao->ID));
    glGenBuffers(1, &(vao->associatedVBOID));
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao->ID);
    glBindBuffer(GL_ARRAY_BUFFER, vao->associatedVBOID);
    glBufferData(GL_ARRAY_BUFFER, attrib_data.size(), &attrib_data[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int offset = 0;
    unsigned int index = 0;
    unsigned int size_sum = 0;
    for(auto const& value: attrib_sizes){
        size_sum += value;
    }
    for(auto const& value: attrib_sizes){
        glVertexAttribPointer(index, value, GL_FLOAT, GL_FALSE, size_sum * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(index);
        index++;
        offset += value;
    }
    return vao;
}

void getPositionVAO(const float* vertices, unsigned int size, unsigned int& VAO, unsigned int& VBO)
{
    //unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}