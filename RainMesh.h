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


class RainMesh {
public:
    float4x4 model;
    uint32_t material_id;

    RainMesh(const vector<float>    &positions,
             const vector<float>    &normals  ,
             const vector<float>    &texcoords,
             const vector<uint32_t> &indices  ,
             size_t mat_id,
             string n);

    string GetName();

    void Draw();

    void DrawInstanced(GLsizei prim_count);

    void update_particles(float deltaTime);

    ~RainMesh();

protected:
    string name;
    GLuint vboVertices, vboIndices, vboNormals, vboTexCoords, vboOffset, vao;
    size_t ind_num;
};



// ========== RAIN PART ===============
void DrawRain();
RainMesh* CreateRaindropMesh();


// rain particles
static void reset_particle(int index);
static void init_rain_particles();


typedef struct Particle {
    // Position/direction
    float3 position;
    float  rotate;

    // Velocity/Direction, only goes down in y dir
    float velocity;
    float gravity;
} Particle;


const uint64_t MAX_RAINDROPS = 10000;
static Particle raindrops[MAX_RAINDROPS];
static float4x4 raindrops_offset[MAX_RAINDROPS];