#include <Graphics/DirectX.h>
#include <Graphics/MeshLoadUtil.h>
#include <Graphics/ProjectionMatrix.h>
#include <Graphics/TextureLoadUtil.h>
#include <Graphics/Window.h>

#include <Utilities/Logging.h>
#include <Utilities/Profile.h>
#include <Utilities/frame_string.h>

#include <IO/FileUtil.h>
#include <IO/PathUtil.h>

#include <UI/EditStruct.h>
#include <UI/EditorUI.h>
#include <UI/UIUtil.h>

#include <Component/transform_t.h>

#include <chrono>

#include <Application/MainLoop.inc>
#include <Application/ResourceDatabase.h>

// -- Uniform buffer data definitions ----

struct alignas(16) directional_light_t {
    pn::vec3f direction;
    float intensity;
};

struct alignas(16) wave_t {
    float A;
    float L;
    float w;
    float q;
    pn::vec2f d;
};

template <> void pn::gui::EditStruct(wave_t &wave) {
    DragFloat("amplitude##", &wave.A, 0.0f, 10.0f);
    DragFloat("speed##", &wave.L, 0.0f, 10.0f);
    DragFloat("wavelength##", &wave.w, 0.0f, 10.0f);
    DragFloat("steepness##", &wave.q, 0.0f, 3.0f);
    DragFloat2("direction##", &(wave.d.x), -1.0f, 1.0f);
    wave.d = (wave.d == pn::vec2f::Zero) ? pn::vec2f::Zero : pn::Normalize(wave.d);
}

// ----- program uniforms ------
#define N_WAVES 4

pn::cbuffer<directional_light_t> directional_light;
pn::cbuffer_array<wave_t, N_WAVES> wave;

// --- wave instance data ----
pn::transform_t wave_transform;
pn::mesh_buffer_t wave_mesh_buffer;
pn::shader_program_t wave_program;

// ----- texture data ---------
pn::texture_t tex;
pn::dx_sampler_state sampler_state;

// ---- misc d3d11 state -----
pn::dx_blend_state blend_state;

// --- image effect data ---
pn::dx_render_target_view offscreen_render_target;

void Init() {
    pn::SetWorkingDirectory("C:/Users/Ryan/Documents/Visual Studio 2017/Projects/Partition/");
    pn::SetResourceDirectoryName("Resources");

    Log("Size of resource id: {} bytes", sizeof(pn::rdb::resource_id_t));

    // ---------- LOAD RESOURCES ----------------

    pn::StartProfile("Loading all meshes");

    {
        pn::StartProfile("Loading wave mesh");
        auto mesh_handle = pn::LoadMesh(pn::GetResourcePath("water.fbx"));
        pn::EndProfile();

        wave_mesh_buffer = pn::rdb::GetMeshResource("Plane");
    }

    pn::EndProfile();

    // --------- LOAD TEXTURES -------------

    tex           = pn::LoadTexture2D(pn::GetResourcePath("image.png"));
    sampler_state = pn::CreateSamplerState(device);

    // ------- SET BLENDING STATE ------------

    blend_state = pn::CreateBlendState(device);
    pn::SetBlendState(device, blend_state);

    // --------- CREATE SHADER DATA ---------------

    wave_program = pn::CompileShaderProgram(device, pn::GetResourcePath("water.hlsl"));

    global_constants.data.screen_width  = static_cast<float>(pn::app::window_desc.width);
    global_constants.data.screen_height = static_cast<float>(pn::app::window_desc.height);

    InitializeCBuffer(device, directional_light);
    InitializeCBuffer(device, wave);

    // init lights
    directional_light.data.direction = pn::vec3f(0.0f, 0.0f, 1.0f);
    directional_light.data.intensity = 1.0f;

    // init wave data
    for(int i = 0; i < N_WAVES; ++i) {
        wave.data[i].A = 0.615f;
        wave.data[i].L = 5.615f;
        wave.data[i].w = 0.615f;
        wave.data[i].q = 0.0f;
        wave.data[i].d = {0.68f, 0.735f};
    }

    // init camera
    camera                     = pn::ProjectionMatrix{pn::ProjectionType::PERSPECTIVE,
                                  static_cast<float>(pn::app::window_desc.width),
                                  static_cast<float>(pn::app::window_desc.height),
                                  0.01f,
                                  1000.0f,
                                  70.0f,
                                  0.1f};
    camera_constants.data.proj = camera.GetMatrix();
    camera_constants.data.view = pn::mat4f::Identity;

    // init wave object
    wave_transform.position = {0, 0, 15};
    wave_transform.scale    = {1, 1, 1};
    wave_transform.rotation = pn::EulerToQuaternion(0.698f, 3.069f, 0.f);
}

void Update(const float dt) {}

void Render() {
    auto context = pn::GetContext(device);

    // Update global uniforms
    global_constants.data.t += static_cast<float>(pn::app::dt);
    auto screen_desc                   = pn::GetTextureDesc(pn::GetSwapChainBackBuffer(swap_chain));
    global_constants.data.screen_width = static_cast<float>(screen_desc.Width);
    global_constants.data.screen_height = static_cast<float>(screen_desc.Height);

    // Set render target backbuffer color
    float color[] = {0.0f, 0.0f, 0.0f, 1.000f};
    context->ClearRenderTargetView(display_render_target.Get(), color);
    context->ClearDepthStencilView(display_depth_stencil.Get(),
                                   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, display_render_target.GetAddressOf(),
                                display_depth_stencil.Get());

    // update directional light
    ImGui::Begin("Lights");

    pn::gui::DragFloat3("light dir", &directional_light.data.direction.x, -1.0f, 1.0f);
    pn::gui::DragFloat("light power", &directional_light.data.intensity, 0.0f, 10.0f);
    directional_light.data.direction = directional_light.data.direction == pn::vec3f::Zero
                                           ? pn::vec3f::Zero
                                           : pn::Normalize(directional_light.data.direction);

    ImGui::End(); // Lights

    SetProgramConstantBuffer(context, global_constants, wave_program);
    SetProgramConstantBuffer(context, camera_constants, wave_program);
    SetProgramConstantBuffer(context, model_constants, wave_program);
    SetProgramConstantBuffer(context, directional_light, wave_program);

    // update uniform buffers that are shared across shaders
    UpdateBuffer(context, global_constants);
    UpdateBuffer(context, camera_constants);
    UpdateBuffer(context, directional_light);

    // ------ BEGIN WATER

    SetProgramConstantBuffer(context, wave, wave_program);

    pn::SetShaderProgram(context, wave_program);

    auto &wave_mesh = wave_mesh_buffer;
    pn::SetVertexBuffers(context, wave_program.input_layout_data, wave_mesh);

    // update wave
    ImGui::Begin("Waves");
    // update model matrix
    pn::gui::EditStruct(wave_transform);
    model_constants.data.model = LocalToWorldMatrix(wave_transform);
    model_constants.data.model_view_inverse_transpose =
        pn::Transpose(pn::Inverse(model_constants.data.model * camera_constants.data.view));
    model_constants.data.mvp =
        model_constants.data.model * camera_constants.data.view * camera_constants.data.proj;

    for(int i = 0; i < N_WAVES; ++i) {
        ImGui::PushID(i);
        pn::gui::EditStruct(wave.data[i]);
        ImGui::PopID();
    }
    ImGui::End(); // Waves

    SetProgramShaderResources(context, tex, wave_program);
    SetProgramSamplers(context, sampler_state, wave_program);

    // send updates to constant buffers
    UpdateBuffer(context, model_constants);
    UpdateBuffer(context, wave);

    pn::DrawIndexed(context, wave_mesh);

    // ----- END WATER
}

void MainLoopBegin() {}

void MainLoopEnd() {}