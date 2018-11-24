#include "drawcall.h"

void DrawSimpleTriangle(ShaderProgram &debug_program,
        Camera &camera,
        uint32_t width,
        uint32_t height)
{
    float4x4 view = camera.GetViewMatrix();
    float4x4 projection = projectionMatrixTransposed(90.0f, float(width) / float(height), 0.1f, 1000);

    debug_program.StartUseShader();
    debug_program.SetUniform("view", view);
    debug_program.SetUniform("projection", projection);
    DrawTriangle();
    debug_program.StopUseShader();
}

void DrawClouds(ShaderProgram &program,
        Camera &camera,
        std::unique_ptr<CloudMesh> &mesh,
        uint32_t width,
        uint32_t height,
        float deltaTime)
{
    float4x4 view = camera.GetViewMatrix();
    float4x4 projection = projectionMatrixTransposed(camera.zoom, float(width) / float(height), 0.1f, 1000.0f);
    
    mesh->UpdateClouds(deltaTime);

    program.StartUseShader();

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program.SetUniform("view", view);
    program.SetUniform("projection", projection);

    mesh->DrawInstanced();

    glDisable(GL_BLEND);
    
    program.StopUseShader();
}

void barelRoll()
{
    if (BARREL_ROLLING == false) {
        BARREL_ROLLING = true;

        for (int i = 0; i < plane_num; ++i) {
            rotates_lates[i] = (0.1 + (float(rand()) / RAND_MAX) * 0.7);
            rotates_speed[i] = (1.4 + (float(rand()) / RAND_MAX) * 0.2);

            planes_rotate_angles[i] = -rotates_lates[i];
            is_rotates_completed[i] = false;
        }
    }
}

void DrawMesh(ShaderProgram &program,
        Camera &camera,
        std::unique_ptr<Mesh> &mesh,
        uint32_t width,
        uint32_t height,
        float deltaTime)
{
    if (mesh->GetName() == "Aircraft_propeller") {
        mesh->model = mul(rotate_X_4x4(deltaTime * 20.0), mesh->model);
    }

    float4x4 models[plane_num];

    float offset = 7.0;
    for(int i = -plane_num / 2, j = 0; i <= plane_num / 2; i++, j++) {
        // Расчёт смещения самолётов для Instancing
        float3 translation = float3(-abs((float)i * offset / 3.0), 
                                    abs((float)i * offset / 3.0),
                                    (float)i * offset);

        models[j] = transpose(translate4x4(translation));

        // Расчёт поворота бочки
        if (!is_rotates_completed[j]) {
            planes_rotate_angles[j] += deltaTime * rotates_speed[j];
            if (planes_rotate_angles[j] >= 2 * 3.1415) {

                planes_rotate_angles[j] = 0.0;
                is_rotates_completed[j] = true;

                bool all_ends = true;

                for (int k = 0; k < plane_num; k++) {
                    if (!is_rotates_completed[k])
                        all_ends = false;
                }

                if (all_ends) {
                    BARREL_ROLLING = false;
                }
            }

            if (planes_rotate_angles[j] > 0.0) {
                models[j] = mul(rotate_X_4x4(planes_rotate_angles[j]), models[j]);
            }
        }

        models[j] = mul(mesh->model, models[j]);
    }
    
    float4x4 view = camera.GetViewMatrix();
    float4x4 projection = projectionMatrixTransposed(camera.zoom, float(width) / float(height), 0.1f, 1000);

    program.StartUseShader();
    
    for(unsigned int i = 0; i < plane_num; i++) {
        program.SetUniform(("models[" + std::to_string(i) + "]").c_str(), models[i]);
    }
    program.SetUniform("view", view);
    program.SetUniform("projection", projection);

    program.SetUniform("plane_texture", 2);
    program.SetUniform("skybox_texture", 5);
    program.SetUniform("color_light", float3(1.0f, 1.0f, 1.0f));
    program.SetUniform("light_pos", float3(50.0f, 50.0f, 0.0f));
    program.SetUniform("view_pos", camera.pos);

    mesh->DrawInstanced(plane_num);
    
    program.StopUseShader();
}