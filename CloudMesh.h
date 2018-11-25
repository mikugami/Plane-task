#pragma once

#include "common.h"
#include "LiteMath.h"

#include <vector>
#include <string>

using LiteMath::float4x4;
using LiteMath::float3;
using LiteMath::float4;
using LiteMath::rotate_Y_4x4;
using LiteMath::mul;

using std::vector;
using std::string;

class CloudMesh {
public:
    float4x4 model;
    uint32_t material_id;

    CloudMesh(const vector<float> &positions,
              const vector<uint32_t> &indices,
              size_t mat_id,
              string n);

    string GetName();

    void Draw();

    void DrawInstanced();

    void UpdateClouds(float deltaTime);

    ~CloudMesh();

protected:
    string name;
    GLuint vboVertices, vboIndices, vboOffset, vboColor, vao;
    size_t ind_num, instances_num;
};

CloudMesh* CreateCloudMesh();

static int InitClouds();

const int MAX_CLOUDCUBES = 7000;
const int MAX_CLOUDS = 120;

typedef struct Cloud {
    int cloudcube_num;
    float3 position[MAX_CLOUDCUBES];
} Cloud;

static Cloud clouds[MAX_CLOUDS];
static float4x4 cloudcube_offset[MAX_CLOUDCUBES * MAX_CLOUDS];
static float colors[MAX_CLOUDCUBES * MAX_CLOUDS];

