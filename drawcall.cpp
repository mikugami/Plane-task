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

void DrawMesh(ShaderProgram &program,
        Camera &camera,
        std::unique_ptr<Mesh> &mesh,
        uint32_t width,
        uint32_t height,
        float deltaTime)
{
    if (mesh->GetName() == "Aircraft_propeller") {
        mesh->model = mul(rotate_X_4x4(deltaTime), mesh->model);
    }
    //offset = mul(rotate_Y_4x4(deltaTime * 10.0), mesh->model);

    const int plane_num = 11;

    //Расчёт смещения самолётов для Instancing
    float4x4 models[plane_num];
    int j = 0;
    float offset = 7.0f;
    for(int i = -5; i <= 5; i += 1) {
        float3 translation;
        translation.x = -abs((float)i * offset / 3.0f);
        translation.z = (float)i * offset;
        translation.y = abs((float)i * offset / 3.0f);
        models[j++] = mul(mesh->model, transpose(translate4x4(translation)));
    }

    float4x4 view = camera.GetViewMatrix();
    float4x4 projection = projectionMatrixTransposed(camera.zoom, float(width) / float(height), 0.1f, 1000);

    program.StartUseShader();
    for(unsigned int i = 0; i < plane_num; i++) {
        program.SetUniform(("models[" + std::to_string(i) + "]").c_str(), models[i]);
    }
    program.SetUniform("view", view);
    program.SetUniform("projection", projection);

    program.SetUniform("color_texture", 0);
    program.SetUniform("color_light", float3(1.0f, 1.0f, 1.0f));
    program.SetUniform("light_pos", float3(50.0f, 50.0f, 0.0f));
    program.SetUniform("view_pos", camera.pos);

    mesh->DrawInstanced(plane_num);
    
    program.StopUseShader();
}