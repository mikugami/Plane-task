#include "RainMesh.h"

RainMesh* CreateRaindropMesh() 
{
    vector<float> pos {
        0,    0,    0,
        0.01, -0.3, 0, 
       -0.01, -0.3, 0, 
    };

    vector<uint32_t> ind {
        0, 1, 2
    };

    return new RainMesh(pos, ind, -1, "RainTriangle");
}

void InitRaindrop(int i)
{
    raindrops_position[i].x = -150.0 + (rand() % 300);
    raindrops_position[i].y = 30.0 + (rand() % 100);
    raindrops_position[i].z = -150.0 + (rand() % 300);

    raindrops_offset[i] = transpose(translate4x4(raindrops_position[i]));
}

void RainMesh::UpdateRain(float deltaTime)
{
    for (int i = 0; i < MAX_RAINDROPS; ++i) {
        raindrops_position[i].y -= 100.0 * deltaTime;
        raindrops_position[i].x -= 10.0 * deltaTime;
        if (raindrops_position[i].y <= -10 || raindrops_position[i].x <= -130.0) {
            InitRaindrop(i);
        } else {
            raindrops_offset[i] = transpose(translate4x4(raindrops_position[i]));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboOffset); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, MAX_RAINDROPS * 16 * sizeof(GL_FLOAT), raindrops_offset, GL_STREAM_DRAW); GL_CHECK_ERRORS;
}


RainMesh::RainMesh(const vector<float> &positions,
            const vector<uint32_t> &indices,
            size_t mat_id,
            string n) 
{
    for (int i = 0; i < MAX_RAINDROPS; ++i) {
        InitRaindrop(i);
    }

    name = n;

    ind_num = indices.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboVertices);
    glGenBuffers(1, &vboIndices);
    glGenBuffers(1, &vboNormals);
    glGenBuffers(1, &vboTexCoords);
    glGenBuffers(1, &vboOffset);


    glBindVertexArray(vao); GL_CHECK_ERRORS;
    {
        //передаем в шейдерную программу атрибут координат вершин
        glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GL_FLOAT), positions.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

        glBindBuffer(GL_ARRAY_BUFFER, vboOffset);
        glBufferData(GL_ARRAY_BUFFER, MAX_RAINDROPS * 16 * sizeof(GL_FLOAT), raindrops_offset, GL_STREAM_DRAW);
        
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(0)); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(3); GL_CHECK_ERRORS;

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(sizeof(float4)));
        glEnableVertexAttribArray(4);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(2 * sizeof(float4)));
        glEnableVertexAttribArray(5);

        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float4), (void*)(3 * sizeof(float4)));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        //передаем в шейдерную программу индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0); GL_CHECK_ERRORS;

    glBindVertexArray(0); GL_CHECK_ERRORS;

    material_id = mat_id;
}

string RainMesh::GetName() 
{
    return name;
}

void RainMesh::Draw() 
{
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElements(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}

void RainMesh::DrawInstanced(GLsizei prim_count) 
{
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElementsInstanced(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr, prim_count); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}

RainMesh::~RainMesh() 
{
    glDeleteVertexArrays(1, &vao);
}