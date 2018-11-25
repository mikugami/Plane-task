#pragma once

#include "common.h"
#include "LiteMath.h"

#include <vector>
#include <string>

using namespace LiteMath;
using namespace std;

class RainMesh {
public:
    float4x4 model;
    uint32_t material_id;

    RainMesh(const vector<float> &positions,
             const vector<uint32_t> &indices,
             size_t mat_id,
             string n);

    string GetName();

    void Draw();

    void DrawInstanced(GLsizei prim_count);

    void UpdateRain(float deltaTime);

    ~RainMesh();

protected:
    string name;
    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords, vboOffset, vao;
    size_t ind_num;
};

RainMesh* CreateRaindropMesh();

void InitRaindrop(int i);

const int MAX_RAINDROPS = 8000;

static float3 raindrops_position[MAX_RAINDROPS];
static float4x4 raindrops_offset[MAX_RAINDROPS];