#pragma once

#include "ShaderProgram.h"
#include "Camera.h"
#include "mesh.h"
#include "cloud_mesh.h"

void DrawSimpleTriangle(ShaderProgram &debug_program,
                        Camera &camera,
                        uint32_t width,
                        uint32_t height);

void DrawClouds(ShaderProgram &program,
        Camera &camera,
        std::unique_ptr<CloudMesh> &mesh,
        uint32_t width,
        uint32_t height,
        float deltaTime);

void DrawMesh(ShaderProgram &program,
    Camera &camera,
    std::unique_ptr<Mesh> &mesh,
    uint32_t width,
    uint32_t height,
    float deltaTime);

void barelRoll();

static const int plane_num = 11;

static bool  BARREL_ROLLING = false;

static float planes_rotate_angles[plane_num] = {0.0, };
static float rotates_speed[plane_num] = {0.0, };
static float rotates_lates[plane_num] = {0.0, };
static bool  is_rotates_completed[plane_num] = {false, };