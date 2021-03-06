#include "mesh.h"

Mesh* CreateSimpleTriangleMesh() {
    vector<float> pos {
        -1,  1, -0.5, 1,
        -1, -1, -0.5, 1,
         1, -1, -0.5, 1
    };

    vector<float> norm {
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1
    };

    vector<float> texc{
        0, 1,
        0, 0,
        1, 0
    };

    vector<uint32_t> ind {
        0, 1, 2
    };

    return new Mesh(pos, norm, texc, ind, -1, "SimpleTriangle");
}

void DrawTriangle() {
    static std::unique_ptr<Mesh> mesh(CreateSimpleTriangleMesh());
    
    mesh->Draw();
}

Mesh::Mesh(const vector<float>    &positions,
           const vector<float>    &normals  ,
           const vector<float>    &texcoords,
           const vector<uint32_t> &indices  ,
           size_t mat_id,
           string n) {
    name = n;

    ind_num = indices.size();
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboVertices);
    glGenBuffers(1, &vboIndices);
    glGenBuffers(1, &vboNormals);
    glGenBuffers(1, &vboTexCoords);


    glBindVertexArray(vao); GL_CHECK_ERRORS;
    {

        //�������� � ��������� ��������� ������� ��������� ������
        glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GL_FLOAT), positions.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

        //�������� � ��������� ��������� ������� ��������
        glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GL_FLOAT), normals.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

        //�������� � ��������� ��������� ������� ���������� ���������
        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GL_FLOAT), texcoords.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

        //�������� � ��������� ��������� �������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0); GL_CHECK_ERRORS;

    glBindVertexArray(0); GL_CHECK_ERRORS;
    
    material_id = mat_id;
}

string Mesh::GetName() {
    return name;
}

void Mesh::Draw() {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElements(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}

void Mesh::DrawInstanced(GLsizei primcount) {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElementsInstanced(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr, primcount); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &vao);
}