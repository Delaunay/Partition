#include <Graphics\Window.h>
#include <Graphics\DirectX.h>
#include <Graphics\MeshLoadUtil.h>
#include <Graphics\TextureLoadUtil.h>
#include <Graphics\ProjectionMatrix.h>

#include <Utilities\Logging.h>
#include <Utilities\frame_string.h>

#include <IO\FileUtil.h>
#include <IO\PathUtil.h>

#include <UI\UIUtil.h>
#include <UI\EditorUI.h>

#include <chrono>

#include <Application\MainLoop.inc>

struct alignas(16) GlobalConstantBufferData {
	float t = 0.0f;
	float screen_width;
	float screen_height;
};
GlobalConstantBufferData c;

struct InstanceConstantBufferData {
	pn::mat4f model;
	pn::mat4f view;
	pn::mat4f proj;
};
InstanceConstantBufferData ic;

struct alignas(16) DirectionalLightBufferData {
	pn::vec3f direction;
	float intensity;
};
DirectionalLightBufferData dl;

struct alignas(16) WaveBuffer {
	float A;
	float L;
	float w;
	float q;
	pn::vec2f d;
};
#define N_WAVES 1
WaveBuffer wb[N_WAVES];

pn::dx_buffer global_constant_buffer;
pn::dx_buffer instance_constant_buffer;
pn::dx_buffer directional_light_buffer;

pn::dx_buffer wave_buffer;

pn::dx_vertex_shader vertex_shader;
pn::input_layout_desc input_layout;
pn::dx_pixel_shader pixel_shader;

pn::vector<pn::mesh_buffer_t> mesh_buffer;

pn::vec3f pos(0, 0, 10);
pn::vec3f scale(1, 1, 1);
pn::vec3f rot(0.698, 0.465, 0);

pn::texture_t tex;
pn::dx_sampler_state sampler_state;

pn::linear_allocator frame_alloc(1024 * 1024);

void Init() {

	pn::SetWorkingDirectory("C:/Users/Ryan/Documents/Visual Studio 2017/Projects/Partition/");
	pn::SetResourceDirectoryName("Resources");

	// ---------- LOAD RESOURCES ----------------

	auto mesh		= pn::LoadMesh(pn::GetResourcePath("water.fbx"));
	mesh_buffer		= pn::CreateMeshBuffer(device, mesh);

	//tex				= pn::LoadTexture2D(pn::GetResourcePath("image.png"));
	//sampler_state	= pn::CreateSamplerState(device);


	// --------- CREATE SHADER DATA ---------------

	auto vs_byte_code	= pn::ReadFile(pn::GetResourcePath("water_vs.cso"));
	vertex_shader		= pn::CreateVertexShader(device, vs_byte_code);
	input_layout		= pn::CreateInputLayout(device, vs_byte_code);

	pixel_shader		= pn::CreatePixelShader(device, pn::GetResourcePath("water_ps.cso"));

	c.screen_width	= static_cast<float>(pn::app::window_desc.width);
	c.screen_height	= static_cast<float>(pn::app::window_desc.height);

	global_constant_buffer		= pn::CreateConstantBuffer(device, &c, 1);
	instance_constant_buffer	= pn::CreateConstantBuffer(device, &ic, 1);
	directional_light_buffer	= pn::CreateConstantBuffer(device, &dl, 1);
	wave_buffer					= pn::CreateConstantBuffer(device, &wb, 1);

	ic.model	= pn::SRTMatrix(scale, rot, pos);
	ic.view		= pn::mat4f::Identity;

	dl.direction = pn::vec3f(0.0f, 0.0f, 1.0f);
	dl.intensity = 1.0f;

	for (int i = 0; i < N_WAVES; ++i) {
		wb[i].A = 0.615f;
		wb[i].L = 5.615;
		wb[i].w = 0.615;
		wb[i].q = 0;
		wb[i].d = { 0.68f, 0.735f };
	}

	camera = pn::ProjectionMatrix{ pn::ProjectionType::PERSPECTIVE,
		static_cast<float>(pn::app::window_desc.width), static_cast<float>(pn::app::window_desc.height),
		0.01f, 1000.0f,
		70.0f, 0.1f
	};
	ic.proj = camera.GetMatrix();

	// --------- INIT CUSTOM ALLOCATORS -----------
	pn::frame_string::SetFrameAllocator(&frame_alloc);
}

void Update(const float dt) {}

void Render() {
	auto context = pn::GetContext(device);

	// Update global uniforms
	c.t					+= static_cast<float>(pn::app::dt);
	auto screen_desc	= pn::GetTextureDesc(pn::GetSwapChainBackBuffer(swap_chain));
	c.screen_width		= static_cast<float>(screen_desc.Width);
	c.screen_height		= static_cast<float>(screen_desc.Height);

	// Set render target backbuffer color
	float color[] = { 0.0f, 0.0f, 0.0f, 1.000f };
	context->ClearRenderTargetView(render_target_view.Get(), color);
	context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	// set vertex buffer
	auto& cmesh_buffer = mesh_buffer[0];
	pn::SetContextVertexBuffers(context, input_layout, cmesh_buffer);
	context->IASetInputLayout(input_layout.ptr.Get());
	context->IASetIndexBuffer(cmesh_buffer.indices.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(cmesh_buffer.topology);

	// set shader
	context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// update instance uniforms

	// update directional light
	ImGui::SliderFloat3("light dir", &dl.direction.x, -1.0f, 1.0f);
	ImGui::SliderFloat("light power", &dl.intensity, 0.0f, 10.0f);
	dl.direction = pn::Normalize(dl.direction);

	// update wave
	for (int i = 0; i < N_WAVES; ++i) {
		pn::string w_id = pn::string("w") + std::to_string(i);
		ImGui::SliderFloat( (w_id + " amp").c_str(), &wb[i].A, 0.0f, 10.0f);
		ImGui::SliderFloat((w_id + " L").c_str(), &wb[i].L, 0.0f, 10.0f);
		ImGui::SliderFloat((w_id + " w").c_str(), &wb[i].w, 0.0f, 10.0f);
		ImGui::SliderFloat((w_id + " Q").c_str(), &wb[i].q, 0.0f, 3.0f);
		ImGui::SliderFloat2((w_id + " d").c_str(), &(wb[i].d.x), -1.0f, 1.0f);
		wb[i].d = (wb[i].d == pn::vec2f::Zero) ? pn::vec2f::Zero : pn::Normalize(wb[i].d);
	}

	// update world and view
	ImGui::SliderFloat3("position", &pos.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("rotation", &rot.x, -pn::TWOPI, pn::TWOPI);
	ic.model = pn::SRTMatrix(scale, rot, pos);

	// update projection

	// send updates to uniform buffers
	context->UpdateSubresource(global_constant_buffer.Get(), 0, nullptr, &c, 0, 0);
	context->UpdateSubresource(instance_constant_buffer.Get(), 0, nullptr, &ic, 0, 0);
	context->UpdateSubresource(directional_light_buffer.Get(), 0, nullptr, &dl, 0, 0);
	context->UpdateSubresource(wave_buffer.Get(), 0, nullptr, &wb, 0, 0);

	// set constant buffers in shaders
	context->VSSetConstantBuffers(0, 1, global_constant_buffer.GetAddressOf());
	context->VSSetConstantBuffers(1, 1, instance_constant_buffer.GetAddressOf());
	context->VSSetConstantBuffers(3, 1, wave_buffer.GetAddressOf());

	context->PSSetConstantBuffers(0, 1, global_constant_buffer.GetAddressOf());
	context->PSSetConstantBuffers(1, 1, instance_constant_buffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, directional_light_buffer.GetAddressOf());

	// update shader textures
	//context->PSSetShaderResources(0, 1, tex.resource_view.GetAddressOf());
	//context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

	pn::DrawIndexed(context, mesh_buffer[0]);
}

void MainLoopBegin() {
	frame_alloc.Release();
}

void MainLoopEnd() {}