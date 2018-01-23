#include <Graphics\Window.h>
#include <Graphics\DirectX.h>
#include <Graphics\MeshLoadUtil.h>
#include <Graphics\TextureLoadUtil.h>
#include <Graphics\ProjectionMatrix.h>

#include <Utilities\Logging.h>
#include <Utilities\frame_string.h>
#include <Utilities\Profile.h>

#include <IO\FileUtil.h>
#include <IO\PathUtil.h>

#include <UI\UIUtil.h>
#include <UI\EditorUI.h>
#include <UI\EditStruct.h>

#include <Component\transform_t.h>

#include <chrono>

#include <Application\ResourceDatabase.h>
#include <Application\MainLoop.inc>

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

template<>
void pn::gui::EditStruct(wave_t& wave) {
	DragFloat("amplitude##" , &wave.A, 0.0f, 10.0f);
	DragFloat("speed##"     , &wave.L, 0.0f, 10.0f);
	DragFloat("wavelength##", &wave.w, 0.0f, 10.0f);
	DragFloat("steepness##" , &wave.q, 0.0f, 3.0f);
	DragFloat2("direction##", &(wave.d.x), -1.0f, 1.0f);
	wave.d = (wave.d == pn::vec2f::Zero) ? pn::vec2f::Zero : pn::Normalize(wave.d);
}

// ----- program uniforms ------
#define N_WAVES 4

pn::cbuffer<directional_light_t>	directional_light;
pn::cbuffer_array<wave_t, N_WAVES>	wave;

// --- wave instance data ----
pn::transform_t			wave_transform;
pn::mesh_buffer_t		wave_mesh_buffer;
pn::shader_program_t	wave_program;

// ----- texture data ---------
pn::dx_resource_view	tex;
pn::dx_sampler_state	ss;

// ---- misc --------
pn::linear_allocator frame_alloc(1024 * 1024);

// ---- misc d3d11 state -----
pn::dx_blend_state ENABLE_ALPHA_BLENDING;

pn::string recomp() {
	auto vs_byte_code = pn::CompileVertexShader(pn::GetResourcePath("water.hlsl"));
	if (pn::Size(vs_byte_code) > 0) {
		wave_program.vertex_shader_data.shader		= pn::CreateVertexShader(vs_byte_code);
		wave_program.input_layout_data				= pn::CreateInputLayout(vs_byte_code);
		wave_program.vertex_shader_data.reflection	= pn::GetShaderReflector(vs_byte_code);
	}
	else {
		return "Compilation error, check log.";
	}

	auto ps_byte_code = pn::CompilePixelShader(pn::GetResourcePath("water.hlsl"));
	if (pn::Size(ps_byte_code) > 0) {
		wave_program.pixel_shader_data.shader		= pn::CreatePixelShader(ps_byte_code);
		wave_program.pixel_shader_data.reflection	= pn::GetShaderReflector(ps_byte_code);
	}
	else {
		return "Compilation error, check log.";
	}

	return "Successfully recompiled.";
}

void Init() {

	REGISTER_COMMAND(recomp, pn::string, void);

	pn::SetWorkingDirectory("C:/Users/Ryan/Documents/Visual Studio 2017/Projects/Partition/");
	//pn::SetWorkingDirectory("M:/projects/partition2");
	pn::SetResourceDirectoryName("resources");

	Log("Size of resource id: {} bytes", sizeof(pn::rdb::resource_id_t));

	// ---------- LOAD RESOURCES ----------------

	{
		auto cube_family = pn::LoadMesh(pn::GetResourcePath("cube_family.fbx"));
		//monkey_mesh_buffer = pn::CreateMeshBuffer(device, mesh);
	}

	pn::StartProfile("Loading all meshes");

	{
		pn::StartProfile("Loading wave mesh");
		auto mesh_handle = pn::LoadMesh(pn::GetResourcePath("water.fbx"));
		pn::EndProfile();

		wave_mesh_buffer = pn::rdb::GetMeshResource("Plane");
		
	}

	{
		/*pn::StartProfile("Loading monkey mesh");
		auto mesh_handle = pn::LoadMesh(pn::GetResourcePath("monkey.fbx"));
		pn::EndProfile();

		while (monkey_mesh_buffer.index_count == 0) {
			monkey_mesh_buffer = pn::rdb::GetMeshResource(mesh_handle++);
		}*/
	}

	pn::EndProfile();

	// --------- LOAD TEXTURES -------------

	tex				= pn::LoadTexture2D(pn::GetResourcePath("image.png"));
	ss	= pn::CreateSamplerState();

	// ------- SET BLENDING STATE ------------

	auto desc = pn::CreateAlphaBlendDesc();
	ENABLE_ALPHA_BLENDING = pn::CreateBlendState(&desc);
	pn::SetBlendState(ENABLE_ALPHA_BLENDING);

	// --------- CREATE SHADER DATA ---------------

	wave_program	= pn::CompileShaderProgram(pn::GetResourcePath("water.hlsl"));
	//basic_program	= pn::CompileShaderProgram(pn::GetResourcePath("basic.hlsl"));

	global_constants.data.screen_width	= static_cast<float>(pn::app::window_desc.width);
	global_constants.data.screen_height	= static_cast<float>(pn::app::window_desc.height);

	InitializeCBuffer(directional_light);
	InitializeCBuffer(wave);

	// init lights
	directional_light.data.direction = pn::vec3f(0.0f, 0.0f, 1.0f);
	directional_light.data.intensity = 1.0f;

	// init wave data
	for (int i = 0; i < N_WAVES; ++i) {
		wave.data[i].A = 0.615f;
		wave.data[i].L = 5.615f;
		wave.data[i].w = 0.615f;
		wave.data[i].q = 0.0f;
		wave.data[i].d = { 0.68f, 0.735f };
	}

	// init camera
	camera = pn::ProjectionMatrix{ pn::ProjectionType::PERSPECTIVE,
		static_cast<float>(pn::app::window_desc.width), static_cast<float>(pn::app::window_desc.height),
		0.01f, 1000.0f,
		70.0f, 0.1f
	};
	camera_constants.data.proj = camera.GetMatrix();
	camera_constants.data.view = pn::mat4f::Identity;

	// init wave object
	wave_transform.position	= { 0, 0, 15 };
	wave_transform.scale	= { 1, 1, 1 };
	wave_transform.rotation = pn::EulerToQuaternion( 0.698f, 3.069f, 0.f );

	// --------- INIT CUSTOM ALLOCATORS -----------
	pn::frame_string::SetFrameAllocator(&frame_alloc);
}

void Update(const double dt) {}

void Render() {

	// Set render target backbuffer color
	pn::ClearDepthStencilView(display_depth_stencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	static const pn::vec4f color = { 0.0f, 0.0f, 0.0f, 1.000f };
	pn::ClearRenderTargetView(display_render_target, color);
	pn::SetRenderTarget(display_render_target, display_depth_stencil);

	// update directional light
	ImGui::Begin("Lights");

	pn::gui::DragFloat3("light dir", &directional_light.data.direction.x, -1.0f, 1.0f);
	pn::gui::DragFloat("light power", &directional_light.data.intensity, 0.0f, 10.0f);
	directional_light.data.direction = directional_light.data.direction == pn::vec3f::Zero ? pn::vec3f::Zero : pn::Normalize(directional_light.data.direction);

	ImGui::End(); // Lights

// ------ BEGIN WATER

	pn::SetShaderProgram(wave_program);

	SetProgramConstant("global_constants" , global_constants);
	SetProgramConstant("camera_constants" , camera_constants);
	SetProgramConstant("model_constants"  , model_constants);
	SetProgramConstant("directional_light", directional_light);
	SetProgramConstant("wave"             , wave);

	// update uniform buffers that are shared across shaders
	UpdateBuffer(global_constants);
	UpdateBuffer(camera_constants);
	UpdateBuffer(directional_light);

	pn::SetShaderProgram(wave_program);

	auto& wave_mesh = wave_mesh_buffer;
	pn::SetVertexBuffers(wave_mesh);

	// update wave
	ImGui::Begin("Waves");
	// update model matrix
	pn::gui::EditStruct(wave_transform);
	model_constants.data.model	= LocalToWorldMatrix(wave_transform);
	model_constants.data.model_view_inverse_transpose = pn::Transpose(pn::Inverse(model_constants.data.model * camera_constants.data.view));
	model_constants.data.mvp	= model_constants.data.model * camera_constants.data.view * camera_constants.data.proj;

	for (int i = 0; i < N_WAVES; ++i) {
		ImGui::PushID(i);
		pn::gui::EditStruct(wave.data[i]);
		ImGui::PopID();
	}
	ImGui::End(); // Waves

	SetProgramResource(wave_program, "tex", tex);

	SetProgramSampler(wave_program, "ss", ss);

	// send updates to constant buffers
	UpdateBuffer(model_constants);
	UpdateBuffer(wave);

	pn::DrawIndexed(wave_mesh);

// ----- END WATER

}

void MainLoopBegin() {
	frame_alloc.Release();
}

void MainLoopEnd() {}

void Close() {}