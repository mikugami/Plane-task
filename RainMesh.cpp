#include "RainMesh.h"

RainMesh* CreateRaindropMesh() {
    vector<float> pos {
        0,        0,   0, 1,
        0.015, -0.4,   0, 1,
       -0.015, -0.4,   0, 1
    };

    vector<float> norm {
        0, 0, 1, 1,
        0, 0, 1, 1,
        0, 0, 1, 1,
    };

    vector<float> texc{
        0, 1,
        0, 0,
        1, 0
    };

    vector<uint32_t> ind {
        0, 1, 2
    };

    return new RainMesh(pos, norm, texc, ind, -1, "RainTriangle");
}

void DrawRain() {
    static std::unique_ptr<RainMesh> mesh(CreateRaindropMesh());

    mesh->Draw();
}


static void reset_particle(int index)
{
    raindrops[index].velocity =  0.0;
    raindrops[index].gravity  = -9.8;
    raindrops[index].position = float3(
        -100.0 + (rand() % 200),
        40.0 + (rand() % 150),
        -100.0 + (rand() % 200)
    );

    raindrops[index].rotate = rand() % 360;

    // calculate offset matrix
    // const float matrix[16] = {
    //     1, 0, 0, 0,
    //     0, 1, 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1
    // };

    // raindrops_offset[index] = float4x4(matrix);

    raindrops_offset[index] = mul(rotate_Y_4x4(raindrops[index].rotate),
                                  transpose(translate4x4(raindrops[index].position)));
}

static void init_rain_particles()
{
    for (int i = 0; i < MAX_RAINDROPS; ++i) {
        reset_particle(i);
    }
}


void RainMesh::update_particles(float deltaTime)
{
    for (int i = 0; i < MAX_RAINDROPS; ++i) {
        float v_0 = raindrops[i].velocity;
        float a   = raindrops[i].gravity;

        float temp = a * deltaTime;
        float shift = deltaTime * (v_0 + temp / 2.0);
        raindrops[i].position.y += shift;
        // std::cout << "for " << i << "position: " << raindrops[i].position.y << std::endl;
        raindrops[i].velocity = v_0 + temp;


        raindrops[i].position.x -= 10.0 * deltaTime;
        if (raindrops[i].position.y <= -10 || raindrops[i].position.x <= -150.0) {
            reset_particle(i);
        } else {
            raindrops_offset[i] = mul(rotate_Y_4x4(raindrops[i].rotate),
                                      transpose(translate4x4(raindrops[i].position)));
        }
    }

    // just for tests
    // ============= RAINDROPS ================
    glBindBuffer(GL_ARRAY_BUFFER, vboOffset); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, MAX_RAINDROPS * 16 * sizeof(GL_FLOAT), raindrops_offset, GL_STREAM_DRAW); GL_CHECK_ERRORS;
    /*
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
    */
    // ========================================
}


RainMesh::RainMesh(const vector<float>    &positions,
           const vector<float>    &normals  ,
           const vector<float>    &texcoords,
           const vector<uint32_t> &indices  ,
           size_t mat_id,
           string n) {
    // RAINDROPS
    init_rain_particles(); GL_CHECK_ERRORS;


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
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

        //передаем в шейдерную программу атрибут нормалей
        glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GL_FLOAT), normals.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

        //передаем в шейдерную программу атрибут текстурных координат
        glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(GL_FLOAT), texcoords.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
        glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

        // ============= RAINDROPS ================
        glBindBuffer(GL_ARRAY_BUFFER, vboOffset);
        glBufferData(GL_ARRAY_BUFFER, sizeof(raindrops_offset), raindrops_offset, GL_STREAM_DRAW);

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
        // ========================================

        //передаем в шейдерную программу индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW); GL_CHECK_ERRORS;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0); GL_CHECK_ERRORS;

    glBindVertexArray(0); GL_CHECK_ERRORS;

    material_id = mat_id;
}

string RainMesh::GetName() {
    return name;
}

void RainMesh::Draw() {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElements(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}

void RainMesh::DrawInstanced(GLsizei prim_count) {
    glBindVertexArray(vao); GL_CHECK_ERRORS;
    glDrawElementsInstanced(GL_TRIANGLES, ind_num, GL_UNSIGNED_INT, nullptr, prim_count); GL_CHECK_ERRORS;
    glBindVertexArray(0); GL_CHECK_ERRORS;
}

RainMesh::~RainMesh() {
    glDeleteVertexArrays(1, &vao);
}