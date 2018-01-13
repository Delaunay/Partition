#pragma once

#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include <wrl.h> // ComPtr

#include <Application/ResourceDatabaseTypes.h>

#include <Graphics/Window.h>
#include <Graphics/ProjectionMatrix.h>

#include <Utilities/Logging.h>
#include <Utilities/Math.h>
#include <Utilities/UtilityTypes.h>

namespace pn {

// ------------ TYPEDEFS -------------

template<typename T>
using dx_ptr				= Microsoft::WRL::ComPtr<T>;

using vertex_input_desc		= pn::vector<struct input_element_desc>;

using dx_device				= dx_ptr<ID3D11Device>;
using dx_context			= dx_ptr<ID3D11DeviceContext>;
using dx_swap_chain			= dx_ptr<IDXGISwapChain>;
using dx_buffer				= dx_ptr<ID3D11Buffer>;

using dx_render_target_view = dx_ptr<ID3D11RenderTargetView>;
using dx_depth_stencil_view = dx_ptr<ID3D11DepthStencilView>;

using dx_input_layout		= dx_ptr<ID3D11InputLayout>;
using dx_shader_reflection	= dx_ptr<ID3D11ShaderReflection>;
using dx_vertex_shader		= dx_ptr<ID3D11VertexShader>;
using dx_pixel_shader		= dx_ptr<ID3D11PixelShader>;

using dx_sampler_state		= dx_ptr<ID3D11SamplerState>;
using dx_texture2d			= dx_ptr<ID3D11Texture2D>;
using dx_resource			= dx_ptr<ID3D11Resource>;
using dx_resource_view		= dx_ptr<ID3D11ShaderResourceView>;

using dx_blend_state		 = dx_ptr<ID3D11BlendState>;
using dx_depth_stencil_state = dx_ptr<ID3D11DepthStencilState>;

// -------- CONSTANTS --------------

extern const unsigned int DEFAULT_SHADER_COMPILATION_FLAGS;

extern const D3D11_BLEND_DESC         DEFAULT_BLEND_DESC;
extern const D3D11_DEPTH_STENCIL_DESC DEFAULT_DEPTH_STENCIL_DESC;

// ------------- CLASS DEFINITIONS ---------------

struct input_element_desc {
	LPCSTR                     SemanticName;
	UINT                       SemanticIndex;
	DXGI_FORMAT                Format;
	UINT                       InputSlot;
	UINT                       AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT                       InstanceDataStepRate;
	static const int NAME_SIZE = 16;
	
	input_element_desc(const input_element_desc& d3d_desc) {
		SemanticName = new char[NAME_SIZE];
		strcpy(const_cast<char*>(SemanticName), d3d_desc.SemanticName);

		SemanticIndex		 = d3d_desc.SemanticIndex;
		Format				 = d3d_desc.Format;
		InputSlot			 = d3d_desc.InputSlot;
		AlignedByteOffset	 = d3d_desc.AlignedByteOffset;
		InputSlotClass		 = d3d_desc.InputSlotClass;
		InstanceDataStepRate = d3d_desc.InstanceDataStepRate;
	}

	input_element_desc(const D3D11_SIGNATURE_PARAMETER_DESC& d3d_parameter_desc, int input_slot) {
		this->SemanticName = new char[NAME_SIZE];
		strcpy(const_cast<LPSTR>(this->SemanticName), d3d_parameter_desc.SemanticName);
		this->SemanticIndex = d3d_parameter_desc.SemanticIndex;
		this->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		this->InputSlot = input_slot;
		this->AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		this->InstanceDataStepRate = 0;

		if (d3d_parameter_desc.Mask == 1) {
			if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) this->Format = DXGI_FORMAT_R32_UINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) this->Format = DXGI_FORMAT_R32_SINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) this->Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (d3d_parameter_desc.Mask <= 3) {
			if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) this->Format = DXGI_FORMAT_R32G32_UINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) this->Format = DXGI_FORMAT_R32G32_SINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) this->Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (d3d_parameter_desc.Mask <= 7) {
			if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) this->Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) this->Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) this->Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (d3d_parameter_desc.Mask <= 15) {
			if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) this->Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) this->Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (d3d_parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) this->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	~input_element_desc() {
		delete[] SemanticName;
	}

	input_element_desc& operator=(const input_element_desc& d3d_desc) {
		SemanticName = new char[NAME_SIZE];
		strcpy(const_cast<char*>(SemanticName), d3d_desc.SemanticName);

		SemanticIndex = d3d_desc.SemanticIndex;
		Format = d3d_desc.Format;
		InputSlot = d3d_desc.InputSlot;
		AlignedByteOffset = d3d_desc.AlignedByteOffset;
		InputSlotClass = d3d_desc.InputSlotClass;
		InstanceDataStepRate = d3d_desc.InstanceDataStepRate;

		return *this;
	}
};

struct input_layout_data_t {
	input_layout_data_t() : ptr(nullptr), desc() {}
	input_layout_data_t(dx_input_layout ptr, vertex_input_desc desc) :
		ptr{ ptr }, desc{ desc } {}

	dx_input_layout ptr;
	vertex_input_desc desc;
};

struct mesh_t {
	pn::vector<pn::vec3f>		vertices;
	pn::vector<pn::vec4f>		colors;
	pn::vector<pn::vec3f>		normals;
	pn::vector<pn::vec3f>		tangents;
	pn::vector<pn::vec3f>		bitangents;
	pn::vector<pn::vec2f>		uvs;
	pn::vector<pn::vec2f>		uv2s;

	pn::vector<unsigned int>	indices;
	D3D_PRIMITIVE_TOPOLOGY		topology;

	pn::string					name;

	mesh_t()						= default;
	mesh_t(mesh_t&&)				= default;

	mesh_t(const mesh_t&)			= delete;
	mesh_t operator=(const mesh_t&) = delete;

};

struct mesh_buffer_t {
	dx_buffer				vertices;
	dx_buffer				colors;
	dx_buffer				normals;
	dx_buffer				tangents;
	dx_buffer				bitangents;
	dx_buffer				uvs;
	dx_buffer				uv2s;

	dx_buffer				indices;
	unsigned int			index_count;
	D3D_PRIMITIVE_TOPOLOGY	topology;

	pn::rdb::resource_id_t	id;
	pn::string				name;
};

struct texture_t {
	dx_resource			resource;
	dx_resource_view	resource_view;

	texture_t() : resource(nullptr), resource_view(nullptr) {}
};

template<typename DxShaderPtrT>
struct shader_data_t {
	DxShaderPtrT			shader;
	dx_shader_reflection	reflection;
};

struct shader_program_t {
	input_layout_data_t				input_layout_data;
	shader_data_t<dx_vertex_shader> vertex_shader_data;
	shader_data_t<dx_pixel_shader>	pixel_shader_data;
};

// ------------------- FUNCTIONS ----------------------

// ------------ CREATION FUNCTIONS -------------

dx_device				CreateDevice();
dx_swap_chain			CreateSwapChain(dx_device device, DXGI_SWAP_CHAIN_DESC swap_chain_desc);
dx_swap_chain			CreateMainWindowSwapChain(dx_device device, const window_handle hwnd, const application_window_desc awd);

dx_texture2d			CreateTexture2D(dx_device device, CD3D11_TEXTURE2D_DESC texture_desc, const D3D11_SUBRESOURCE_DATA* initial_data);
dx_sampler_state		CreateSamplerState(dx_device device, CD3D11_SAMPLER_DESC sampler_desc);
dx_sampler_state		CreateSamplerState(dx_device device);

dx_render_target_view	CreateRenderTargetView(dx_device device, dx_texture2d texture);
dx_depth_stencil_view	CreateDepthStencilView(dx_device device, dx_texture2d& depth_stencil_texture);

vector<mesh_buffer_t>	CreateMeshBuffer(dx_device device, const pn::vector<mesh_t>& mesh);
mesh_buffer_t			CreateMeshBuffer(dx_device device, const mesh_t& mesh);

// -------------- SHADER CREATION -------------

pn::bytes				CompileShader(
	const pn::string& filename,
	const D3D_SHADER_MACRO* defines,
	const pn::string& entry_point,
	const pn::string& shader_type,
	unsigned int flags);

shader_program_t		CompileShaderProgram(dx_device device, const pn::string& filename, const D3D_SHADER_MACRO* defines = nullptr, unsigned int flags = DEFAULT_SHADER_COMPILATION_FLAGS);

template<typename ID3D11ShaderType, typename DeviceShaderFunc>
auto					CreateShader(dx_device device, const pn::bytes& bytes, DeviceShaderFunc CreateShader) {

	dx_ptr<ID3D11ShaderType> shader;
	if (bytes.empty()) {
		LogError("Couldn't create shader {} from byte code: {}", TYPENAME(ID3D11ShaderType), "No data");
		return shader;
	}

	auto hr = CreateShader(
		bytes.data(),
		bytes.size(),
		nullptr,
		&shader
	);

	if (FAILED(hr)) {
		LogError("Couldn't create shader {} from byte code: {}", TYPENAME(ID3D11ShaderType), ErrMsg(hr));
	}

	return shader;
}

pn::bytes				CompileVertexShader(const pn::string& filename, const D3D_SHADER_MACRO* defines = nullptr, unsigned int flags = DEFAULT_SHADER_COMPILATION_FLAGS);

dx_vertex_shader		CreateVertexShader(dx_device device, const pn::bytes& bytes);
dx_vertex_shader		CreateVertexShader(dx_device device, const pn::string& filename);

pn::bytes				CompilePixelShader(const pn::string& filename, const D3D_SHADER_MACRO* defines = nullptr, unsigned int flags = DEFAULT_SHADER_COMPILATION_FLAGS);

dx_pixel_shader			CreatePixelShader(dx_device device, const pn::bytes& ps_data);
dx_pixel_shader			CreatePixelShader(dx_device device, const pn::string& filename);

input_layout_data_t		CreateInputLayout(dx_device device, const pn::bytes& bytes, const vertex_input_desc& desc);
input_layout_data_t		CreateInputLayout(dx_device device, const pn::bytes& bytes);

std::pair<dx_vertex_shader, input_layout_data_t> 
						CreateVertexShaderAndInputLayout(dx_device device, const pn::string& filename, const vertex_input_desc& desc);

std::pair<dx_vertex_shader, input_layout_data_t> 
						CreateVertexShaderAndInputLayout(dx_device device, const pn::bytes& vs_byte_code, const vertex_input_desc& desc);

std::pair<dx_vertex_shader, input_layout_data_t> 
						CreateVertexShaderAndInputLayout(dx_device device, const pn::string& filename);

// -------------- BUFFER CREATION -------------------

template<typename BufferDataType>
auto					CreateBufferDataDesc(BufferDataType* data) {
	D3D11_SUBRESOURCE_DATA data_desc;
	ZeroMemory(&data_desc, sizeof(decltype(data_desc)));
	data_desc.pSysMem = data;
	data_desc.SysMemPitch = 0;
	data_desc.SysMemSlicePitch = 0;
	return data_desc;
}

template<typename BufferDataType>
auto					CreateBuffer(dx_device device, BufferDataType* v_data, const size_t n, const D3D11_BIND_FLAG bind_target) {
	// Create buffer description
	CD3D11_BUFFER_DESC buffer_desc(static_cast<unsigned int>(n) * sizeof(BufferDataType), bind_target);

	// Create buffer data
	auto data_desc = CreateBufferDataDesc(v_data);

	dx_buffer buffer;
	auto hr = device->CreateBuffer(&buffer_desc, &data_desc, &buffer);
	if (FAILED(hr)) {
		LogError("Couldn't create buffer {}: {}", bind_target, pn::ErrMsg(hr));
	}

	return buffer;
}

template<typename VertexDataType>
auto					CreateVertexBuffer(dx_device device, const VertexDataType* v_data, const size_t n) {
	return CreateBuffer(device, v_data, n, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER);
}

template<typename VertexDataType>
auto					CreateVertexBuffer(dx_device device, const pn::vector<VertexDataType>& v_data) {
	return CreateVertexBuffer(device, v_data.data(), v_data.size());
}

template<typename ConstantDataType>
auto					CreateConstantBuffer(dx_device device, const ConstantDataType* c_data, const size_t n) {
	static_assert((sizeof(ConstantDataType) % 16) == 0, "Constant Buffer size must be 16-byte aligned");
	return CreateBuffer(device, c_data, n, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER);
}

template<typename ConstantDataType>
auto CreateConstantBuffer(dx_device device, const pn::vector<ConstantDataType>& c_data) {
	return CreateConstantBuffer(device, c_data.data(), c_data.size());
}

template<typename IndexDataType>
auto					CreateIndexBuffer(dx_device device, const IndexDataType* i_data, const size_t n) {
	return CreateBuffer(device, i_data, n, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER);
}

template<typename IndexDataType>
auto					CreateIndexBuffer(dx_device device, const pn::vector<IndexDataType>& i_data) {
	return CreateIndexBuffer(device, i_data.data(), i_data.size());
}

// ------------ UTILITY FUNCTIONS -------------

dx_texture2d			GetSwapChainBackBuffer(dx_swap_chain swap_chain);
dx_context				GetContext(dx_device device);
CD3D11_TEXTURE2D_DESC	GetTextureDesc(dx_texture2d texture);

// ---------- SHADER REFLECTION ----------------

dx_shader_reflection			GetShaderReflector(const pn::bytes& shader_byte_code);

vertex_input_desc				GetVertexInputDescFromShader(const pn::bytes& vs_byte_code);
vertex_input_desc				GetVertexInputDescFromShader(dx_shader_reflection reflector);

D3D11_SHADER_INPUT_BIND_DESC	GetResourceBindingDesc(dx_shader_reflection reflector, const pn::string& name);

unsigned int					GetShaderResourceStartSlot(dx_shader_reflection reflector, const pn::string& name);

// ----------- VIEWPORT -----------------------

void SetViewport(dx_context context, const int width, const int height, const int top_left_x = 0, const int top_left_y = 0);
void SetRenderTargetViewAndDepthStencilFromSwapChain(dx_device device, 
																		dx_swap_chain swap_chain, 
																		dx_render_target_view& render_target_view, 
																		dx_depth_stencil_view& depth_stencil_view);
void ResizeRenderTargetViewportCamera(dx_device device, 
														 unsigned int width, unsigned int height, 
														 dx_swap_chain& swap_chain, 
														 dx_render_target_view& render_target_view, 
														 dx_depth_stencil_view& depth_stencil_view, 
														 ProjectionMatrix& camera);

// --------- SHADER STATE ----------------

void SetShaderProgram(dx_context context, shader_program_t& shader_program);

void SetVertexShader(dx_context context, dx_vertex_shader shader);
void SetPixelShader(dx_context context, dx_pixel_shader shader);

void SetInputLayout(dx_context context, const input_layout_data_t& layout_desc);

void SetVertexBuffers(dx_context context, const input_layout_data_t& input_layout, const mesh_buffer_t& mesh_buffer);

void SetVSConstantBuffer(dx_context context, const pn::string& buffer_name, dx_shader_reflection reflection, dx_buffer& buffer);
void SetPSConstantBuffer(dx_context context, const pn::string& buffer_name, dx_shader_reflection reflection, dx_buffer& buffer);

#define SetProgramVSConstantBuffer(context, name, program) pn::SetVSConstantBuffer(context, #name, program.vertex_shader_data.reflection, name.buffer)

#define SetProgramPSConstantBuffer(context, name, program) pn::SetPSConstantBuffer(context, #name, program.pixel_shader_data.reflection, name.buffer)

#define SetProgramConstantBuffer(context, name, program)	\
SetProgramVSConstantBuffer(context, name, program);			\
SetProgramPSConstantBuffer(context, name, program);

void SetVSShaderResources(dx_context context, const pn::string& resource_name, dx_shader_reflection reflection, dx_resource_view& resource_view);
void SetPSShaderResources(dx_context context, const pn::string& resource_name, dx_shader_reflection reflection, dx_resource_view& resource_view);

#define SetProgramVSShaderResources(context, name, program) pn::SetVSShaderResources(context, #name, program.vertex_shader_data.reflection, name.resource_view)

#define SetProgramPSShaderResources(context, name, program) pn::SetPSShaderResources(context, #name, program.pixel_shader_data.reflection, name.resource_view)

#define SetProgramShaderResources(context, name, program)	\
SetProgramVSShaderResources(context, name, program);		\
SetProgramPSShaderResources(context, name, program);

void SetVSSamplers(dx_context context, const pn::string& sampler_name, dx_shader_reflection reflection, dx_sampler_state& sampler_state);
void SetPSSamplers(dx_context context, const pn::string& sampler_name, dx_shader_reflection reflection, dx_sampler_state& sampler_state);

#define SetProgramVSSamplers(context, name, program) pn::SetVSSamplers(context, #name, program.vertex_shader_data.reflection, name)

#define SetProgramPSSamplers(context, name, program) pn::SetPSSamplers(context, #name, program.pixel_shader_data.reflection, name)

#define SetProgramSamplers(context, name, program)	\
SetProgramVSSamplers(context, name, program);		\
SetProgramPSSamplers(context, name, program);

// ---------- BLENDING -----------------

const D3D11_BLEND_DESC GetDefaultBlendDesc(const int render_target = 0);

dx_blend_state	       CreateBlendState(dx_device device, const D3D11_BLEND_DESC& blend_desc = DEFAULT_BLEND_DESC);

void			       SetBlendState(dx_device device, dx_blend_state blend_state);

// --------- DEPTH STENCIL -------------

const D3D11_DEPTH_STENCIL_DESC GetDefaultDepthStencilDesc();

dx_depth_stencil_state         CreateDepthStencilState(dx_device device, const D3D11_DEPTH_STENCIL_DESC& depth_stencil_desc = DEFAULT_DEPTH_STENCIL_DESC);

// -------- DRAWING FUNCTIONS ---------------

void DrawIndexed(dx_context context, const mesh_buffer_t& mesh_buffer, unsigned int start_vertex_location = 0, unsigned int base_vertex_location = 0);



} // namespace pn