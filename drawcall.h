#pragma once

#include "ShaderProgram.h"
#include "Camera.h"
#include "mesh.h"
#include "CloudMesh.h"
#include "RainMesh.h"

void DrawSimpleTriangle(ShaderProgram &debug_program,
                        Camera &camera,
                        uint32_t width,
                        uint32_t height);

void DrawClouds(ShaderProgram &program,
        Camera &camera,
        CloudMesh *mesh,
        uint32_t width,
        uint32_t height,
        float deltaTime);

void DrawRain(ShaderProgram &program,
    Camera &camera,
    RainMesh *mesh,
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

static bool  barell_rolling = false;
static bool  rolling_inited = false;

static float planes_rotate_angles[plane_num];
static float planes_rotate_speed[plane_num];
static bool  is_rotating[plane_num];