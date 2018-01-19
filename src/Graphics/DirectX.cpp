#include <Graphics/DirectX.h>

#include <functional>
#include <memory>

#include <d3dcompiler.h>

#include <IO/FileUtil.h>
#include <IO/PathUtil.h>

using namespace std::placeholders;

namespace pn {

// --------------- CONSTANTS ---------------
static const D3D_FEATURE_LEVEL SUPPORTED_D3D_FEATURE_LEVELS[] = {
    D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,  D3D_FEATURE_LEVEL_9_1};

static const D3D_FEATURE_LEVEL BAD_LEVELS[] = {D3D_FEATURE_LEVEL_12_0};

#ifdef _DEBUG
const unsigned int DEFAULT_SHADER_COMPILATION_FLAGS =
    D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
const unsigned int DEFAULT_SHADER_COMPILATION_FLAGS = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

const D3D11_BLEND_DESC         DEFAULT_BLEND_DESC         = GetDefaultBlendDesc();
const D3D11_DEPTH_STENCIL_DESC DEFAULT_DEPTH_STENCIL_DESC = GetDefaultDepthStencilDesc();

// --------------- FUNCTIONS --------------------

// ------------ CREATION FUNCTIONS -------------

dx_device CreateDevice() {

    auto GetDeviceFlags = []() {
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	return deviceFlags;
    };

    auto              device_flags = GetDeviceFlags();
    D3D_FEATURE_LEVEL feature_level;
    dx_device         device;
    dx_context        context;
    auto              hr = D3D11CreateDevice(
        nullptr,                  // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE, // Create a device using the hardware graphics driver.
        0,            // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        device_flags, // Set debug and Direct2D compatibility flags.
        SUPPORTED_D3D_FEATURE_LEVELS, // List of feature levels this app can support.
        ARRAYSIZE(SUPPORTED_D3D_FEATURE_LEVELS), // Size of the list above.
        D3D11_SDK_VERSION, // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        &device,           // Returns the Direct3D device created.
        &feature_level,    // Returns feature level of device created.
        &context           // Returns the device immediate context.
    );

    if(FAILED(hr)) {
	// Handle device interface creation failure if it occurs.
	// For example, reduce the feature level requirement, or fail over
	// to WARP rendering.
	LogError("Couldn't create D3D11 device: {}", ErrMsg(hr));
	exit(1);
    }

    Log("Loaded DirectX Version {0:x}", feature_level);

    dx_ptr<IDXGIDevice>  gi_device;
    dx_ptr<IDXGIAdapter> adapter;

    hr = device->QueryInterface(__uuidof(IDXGIDevice), (void **)gi_device.GetAddressOf());
    if(FAILED(hr)) {
	LogError("Couldn't query DXGIDevice interface: {}", ErrMsg(hr));
    }

    hr = gi_device->GetAdapter(adapter.GetAddressOf());
    if(FAILED(hr)) {
	LogError("Couldn't get DXGIAdapter: {}", ErrMsg(hr));
    }

    DXGI_ADAPTER_DESC desc;
    hr = adapter->GetDesc(&desc);
    if(FAILED(hr)) {
	LogError("Couldn't get DXGI_ADAPTER_DESC: {}", ErrMsg(hr));
    }

    char description[256];
    wcstombs(description, desc.Description, 256);
    Log("Graphics Card: {}", description);
    return device;
}

dx_swap_chain CreateSwapChain(dx_device device, DXGI_SWAP_CHAIN_DESC swap_chain_desc) {
    // Create the DXGI device object to use in other factories, such as Direct2D.
    dx_ptr<IDXGIDevice3> dxgiDevice;
    device.As(&dxgiDevice);

    // Create swap chain.
    dx_ptr<IDXGIAdapter> adapter;
    dx_ptr<IDXGIFactory> factory;

    auto          hr = dxgiDevice->GetAdapter(&adapter);
    dx_swap_chain swap_chain;
    if(SUCCEEDED(hr)) {
	adapter->GetParent(IID_PPV_ARGS(&factory));
	hr = factory->CreateSwapChain(device.Get(), &swap_chain_desc, &swap_chain);
	if(FAILED(hr)) {
	    LogError("Couldn't create swap chain: {}", ErrMsg(hr));
	    exit(1);
	}
    } else {
	LogError("Couldn't get device adapter: {}", ErrMsg(hr));
	exit(1);
    }
    return swap_chain;
}

dx_swap_chain CreateMainWindowSwapChain(dx_device device, const window_handle hwnd,
                                        const application_window_desc awd) {
    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
    desc.Windowed           = !awd.fullscreen;
    desc.BufferCount        = 2;
    desc.BufferDesc.Format  = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.BufferDesc.Width   = awd.width;
    desc.BufferDesc.Height  = awd.height;
    desc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count   = 1; // multisampling setting
    desc.SampleDesc.Quality = 0; // vendor-specific flag
    desc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.OutputWindow       = hwnd;
    desc.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching

    return CreateSwapChain(device, desc);
}

dx_render_target_view CreateRenderTargetView(dx_device device, dx_texture2d texture) {
    dx_render_target_view render_target_view;
    auto                  hr =
        device->CreateRenderTargetView(texture.Get(), nullptr, render_target_view.GetAddressOf());

    if(FAILED(hr)) {
	LogError("Couldn't create render target view: {}", ErrMsg(hr));
    }

    return render_target_view;
}

dx_render_target_view CreateRenderTargetView(dx_device device, CD3D11_TEXTURE2D_DESC texture_desc,
                                             const D3D11_SUBRESOURCE_DATA *initial_data = nullptr) {
    auto render_target = CreateTexture2D(device, texture_desc, initial_data);
    return CreateRenderTargetView(device, render_target);
}

dx_depth_stencil_view CreateDepthStencilView(dx_device     device,
                                             dx_texture2d &depth_stencil_texture) {
    dx_depth_stencil_view          depth_stencil_view;
    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    auto hr = device->CreateDepthStencilView(depth_stencil_texture.Get(), &depthStencilViewDesc,
                                             &depth_stencil_view);

    if(FAILED(hr)) {
	LogError("Couldn't create depth stencil view: {}", ErrMsg(hr));
    }

    return depth_stencil_view;
}

dx_texture2d CreateTexture2D(dx_device device, CD3D11_TEXTURE2D_DESC texture_desc,
                             const D3D11_SUBRESOURCE_DATA *initial_data = nullptr) {
    dx_texture2d texture;
    auto         hr = device->CreateTexture2D(&texture_desc, initial_data, &texture);

    if(FAILED(hr)) {
	LogError("Couldn't create texture 2D: {}", ErrMsg(hr));
    }

    return texture;
}

dx_sampler_state CreateSamplerState(dx_device device, CD3D11_SAMPLER_DESC sampler_desc) {
    dx_sampler_state sampler_state;
    auto             hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
    if(FAILED(hr)) {
	LogError("Coudln't create sampler state: {}", ErrMsg(hr));
    }
    return sampler_state;
}

dx_sampler_state CreateSamplerState(dx_device device) {
    CD3D11_SAMPLER_DESC sampler_desc;
    sampler_desc.Filter         = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
    sampler_desc.AddressU       = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.AddressV       = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.AddressW       = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.MipLODBias     = 0.0f;
    sampler_desc.MaxAnisotropy  = 1;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
    sampler_desc.MinLOD         = -FLT_MAX;
    sampler_desc.MaxLOD         = FLT_MAX;
    // sampler_desc.BorderColor = pn::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

    return CreateSamplerState(device, sampler_desc);
}

mesh_buffer_t CreateMeshBuffer(dx_device device, const mesh_t &mesh) {
    mesh_buffer_t mesh_buffer;

    if(mesh.vertices.empty()) {
	LogError("Mesh has no vertices");
	return mesh_buffer;
    }

    mesh_buffer.name        = mesh.name;
    mesh_buffer.vertices    = CreateVertexBuffer(device, mesh.vertices);
    mesh_buffer.indices     = CreateIndexBuffer(device, mesh.indices);
    mesh_buffer.index_count = static_cast<unsigned int>(mesh.indices.size());
    mesh_buffer.topology    = mesh.topology;

    if(!mesh.normals.empty()) {
	mesh_buffer.normals = CreateVertexBuffer(device, mesh.normals);
    }

    if(!mesh.tangents.empty()) {
	mesh_buffer.tangents   = CreateVertexBuffer(device, mesh.tangents);
	mesh_buffer.bitangents = CreateVertexBuffer(device, mesh.bitangents);
    }

    if(!mesh.colors.empty()) {
	mesh_buffer.colors = CreateVertexBuffer(device, mesh.colors);
    }

    if(!mesh.uvs.empty()) {
	mesh_buffer.uvs = CreateVertexBuffer(device, mesh.uvs);
    }

    if(!mesh.uv2s.empty()) {
	mesh_buffer.uv2s = CreateVertexBuffer(device, mesh.uv2s);
    }

    return mesh_buffer;
}

vector<mesh_buffer_t> CreateMeshBuffer(dx_device device, const pn::vector<mesh_t> &meshs) {
    pn::vector<mesh_buffer_t> mesh_buffers;
    for(const auto &mesh : meshs) {
	mesh_buffers.emplace_back(CreateMeshBuffer(device, mesh));
    }
    return mesh_buffers;
}

// -------------- SHADER CREATION -------------

pn::bytes CompileShader(const pn::string &filename, const D3D_SHADER_MACRO *defines,
                        const pn::string &entry_point, const pn::string &shader_type,
                        unsigned int flags) {
    pn::wstring wfilename(filename.size(), L'#');
    mbstowcs(wfilename.data(), filename.c_str(), filename.size());
    ID3DBlob *byte_code  = nullptr;
    ID3DBlob *error_msgs = nullptr;
    auto      hr = D3DCompileFromFile(wfilename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                 entry_point.c_str(), shader_type.c_str(), flags, 0, &byte_code,
                                 &error_msgs);
    if(FAILED(hr)) {
	LogError("Couldn't compile {}::{}: {}", filename, entry_point, ErrMsg(hr));
    }
    if(error_msgs != NULL) {
	LogError("Compilation errors: {}", (char *)(error_msgs->GetBufferPointer()));
    }
    pn::bytes return_bytes;
    if(byte_code != nullptr) {
	return_bytes.resize(byte_code->GetBufferSize());
	memcpy(return_bytes.data(), byte_code->GetBufferPointer(), byte_code->GetBufferSize());
    }
    return return_bytes;
}

shader_program_t CompileShaderProgram(dx_device device, const pn::string &filename,
                                      const D3D_SHADER_MACRO *defines, unsigned int flags) {
    shader_program_t program;

    auto vs_byte_code                     = pn::CompileVertexShader(filename, defines, flags);
    program.vertex_shader_data.shader     = pn::CreateVertexShader(device, vs_byte_code);
    program.input_layout_data             = pn::CreateInputLayout(device, vs_byte_code);
    program.vertex_shader_data.reflection = pn::GetShaderReflector(vs_byte_code);

    auto ps_byte_code                    = pn::CompilePixelShader(filename, defines, flags);
    program.pixel_shader_data.shader     = pn::CreatePixelShader(device, ps_byte_code);
    program.pixel_shader_data.reflection = pn::GetShaderReflector(ps_byte_code);

    return program;
}

pn::bytes CompileVertexShader(const pn::string &filename, const D3D_SHADER_MACRO *defines,
                              unsigned int flags) {
    return CompileShader(filename, defines, "VS_main", "vs_5_0", flags);
}

dx_vertex_shader CreateVertexShader(dx_device device, const pn::bytes &bytes) {
    return CreateShader<ID3D11VertexShader>(
        device, bytes, std::bind(&ID3D11Device::CreateVertexShader, device.Get(), _1, _2, _3, _4));
}

dx_vertex_shader CreateVertexShader(dx_device device, const std::string &filename) {
    auto vs_data = pn::ReadFile(filename);
    return CreateVertexShader(device, vs_data);
}

pn::bytes CompilePixelShader(const pn::string &filename, const D3D_SHADER_MACRO *defines,
                             unsigned int flags) {
    return CompileShader(filename, defines, "PS_main", "ps_5_0", flags);
}

dx_pixel_shader CreatePixelShader(dx_device device, const pn::bytes &ps_data) {
    return CreateShader<ID3D11PixelShader>(
        device, ps_data, std::bind(&ID3D11Device::CreatePixelShader, device.Get(), _1, _2, _3, _4));
}

dx_pixel_shader CreatePixelShader(dx_device device, const std::string &filename) {
    auto ps_data = pn::ReadFile(filename);
    return CreatePixelShader(device, ps_data);
}

input_layout_data_t CreateInputLayout(dx_device device, const pn::bytes &vs_byte_code,
                                      const vertex_input_desc &desc) {

    dx_ptr<ID3D11InputLayout> ptr;
    auto                      hr =
        device->CreateInputLayout(reinterpret_cast<const D3D11_INPUT_ELEMENT_DESC *>(desc.data()),
                                  static_cast<unsigned int>(desc.size()), vs_byte_code.data(),
                                  static_cast<unsigned int>(vs_byte_code.size()), &ptr);

    if(FAILED(hr)) {
	LogError("Couldn't create input layout: {}", ErrMsg(hr));
    }

    auto layout = input_layout_data_t(ptr, desc);
    return layout;
}

input_layout_data_t CreateInputLayout(dx_device device, const pn::bytes &vs_byte_code) {
    auto desc = GetVertexInputDescFromShader(vs_byte_code);
    return CreateInputLayout(device, vs_byte_code, desc);
}

std::pair<dx_vertex_shader, input_layout_data_t>
CreateVertexShaderAndInputLayout(dx_device device, const pn::bytes &vs_data,
                                 const vertex_input_desc &desc) {
    auto vertex_shader = CreateVertexShader(device, vs_data);
    auto input_layout  = CreateInputLayout(device, vs_data, desc);
    return std::make_pair(vertex_shader, input_layout);
}

std::pair<dx_vertex_shader, input_layout_data_t>
CreateVertexShaderAndInputLayout(dx_device device, const std::string &filename,
                                 const vertex_input_desc &desc) {
    auto vs_data = pn::ReadFile(filename);
    return CreateVertexShaderAndInputLayout(device, vs_data, desc);
}

std::pair<dx_vertex_shader, input_layout_data_t>
CreateVertexShaderAndInputLayout(dx_device device, const std::string &filename) {
    auto              vs_data = pn::ReadFile(filename);
    vertex_input_desc desc    = GetVertexInputDescFromShader(vs_data);
    return CreateVertexShaderAndInputLayout(device, vs_data, desc);
}

// ------------ UTILITY FUNCTIONS -------------

dx_texture2d GetSwapChainBackBuffer(dx_swap_chain swap_chain) {
    dx_texture2d back_buffer;
    auto         hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&back_buffer);

    if(FAILED(hr)) {
	// TODO: Fail condition
	LogError("Couldn't get swap chain buffer: {}", ErrMsg(hr));
    }

    return back_buffer;
}

dx_context GetContext(dx_device device) {
    dx_context context;
    device->GetImmediateContext(&context);

    if(!context) {
	LogError("Couldn't get device context");
    }

    return context;
}

CD3D11_TEXTURE2D_DESC GetTextureDesc(dx_texture2d texture) {
    CD3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    return desc;
}

// --------- SHADER REFLECTION ------------

dx_shader_reflection GetShaderReflector(const pn::bytes &shader_byte_code) {
    dx_shader_reflection reflector = nullptr;
    auto                 hr        = D3DReflect(shader_byte_code.data(), shader_byte_code.size(),
                         IID_ID3D11ShaderReflection, (void **)reflector.GetAddressOf());
    if(FAILED(hr)) {
	LogError("Couldn't get reflector from shader");
	return nullptr;
    }
    return reflector;
}

vertex_input_desc GetVertexInputDescFromShader(const pn::bytes &vs_byte_code) {
    dx_shader_reflection reflector = GetShaderReflector(vs_byte_code);
    return GetVertexInputDescFromShader(reflector);
}

vertex_input_desc GetVertexInputDescFromShader(dx_shader_reflection reflector) {
    D3D11_SHADER_DESC shader_desc;
    auto              hr = reflector->GetDesc(&shader_desc);
    if(FAILED(hr)) {
	LogError("Couldn't get vertex shader description from reflector");
	return {};
    }

    vertex_input_desc vertex_desc;
    Reserve(vertex_desc, shader_desc.InputParameters);
    for(unsigned int i = 0; i < shader_desc.InputParameters; ++i) {
	D3D11_SIGNATURE_PARAMETER_DESC param_desc;
	auto                           hr = reflector->GetInputParameterDesc(i, &param_desc);
	if(FAILED(hr)) {
	    LogError("Couldn't get input parameter description from reflector");
	}
	input_element_desc element_desc(param_desc, i);

	pn::PushBack(vertex_desc, element_desc);
    }

    return vertex_desc;
}

D3D11_SHADER_INPUT_BIND_DESC GetResourceBindingDesc(dx_shader_reflection reflector,
                                                    const pn::string &   name) {
    D3D11_SHADER_INPUT_BIND_DESC binding_desc;
    ZeroMemory(&binding_desc, sizeof(D3D11_SHADER_INPUT_BIND_DESC));
    auto hr = reflector->GetResourceBindingDescByName(name.c_str(), &binding_desc);
    if(FAILED(hr)) {
	// Failure could be fine if cbuffer or resource is optimized out of shader
	// LogError("Couldn't get resource binding desc: {}", ErrMsg(hr));
    }
    return binding_desc;
}

unsigned int GetShaderResourceStartSlot(dx_shader_reflection reflector, const pn::string &name) {
    auto binding_desc = GetResourceBindingDesc(reflector, name);
    return binding_desc.BindPoint;
}

// --------- VIEWPORT --------------

void SetViewport(dx_context context, const int width, const int height, const int top_left_x,
                 const int top_left_y) {
    D3D11_VIEWPORT m_viewport;
    ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));

    m_viewport.TopLeftX = (float)top_left_x;
    m_viewport.TopLeftY = (float)top_left_y;
    m_viewport.Height   = (float)height;
    m_viewport.Width    = (float)width;
    m_viewport.MinDepth = 0;
    m_viewport.MaxDepth = 1;

    context->RSSetViewports(1, &m_viewport);
}

void SetRenderTargetViewAndDepthStencilFromSwapChain(dx_device device, dx_swap_chain swap_chain,
                                                     dx_render_target_view &render_target_view,
                                                     dx_depth_stencil_view &depth_stencil_view) {

    auto back_buffer   = pn::GetSwapChainBackBuffer(swap_chain);
    render_target_view = pn::CreateRenderTargetView(device, back_buffer);

    CD3D11_TEXTURE2D_DESC back_buffer_desc = pn::GetTextureDesc(back_buffer);
    CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT,
                                           static_cast<UINT>(back_buffer_desc.Width),
                                           static_cast<UINT>(back_buffer_desc.Height),
                                           1, // This depth stencil view has only one texture.
                                           1, // Use a single mipmap level.
                                           D3D11_BIND_DEPTH_STENCIL);

    auto depth_stencil = pn::CreateTexture2D(device, depthStencilDesc);
    depth_stencil_view = pn::CreateDepthStencilView(device, depth_stencil);

    auto context = pn::GetContext(device);
    context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

void ResizeRenderTargetViewportCamera(dx_device device, unsigned int width, unsigned int height,
                                      dx_swap_chain &        swap_chain,
                                      dx_render_target_view &render_target_view,
                                      dx_depth_stencil_view &depth_stencil_view,
                                      ProjectionMatrix &     camera) {
    pn::SetRenderTargetViewAndDepthStencilFromSwapChain(device, swap_chain, render_target_view,
                                                        depth_stencil_view);
    pn::SetViewport(pn::GetContext(device), width, height);
    camera.SetViewWidth(static_cast<float>(width));
    camera.SetViewHeight(static_cast<float>(height));
}

// --------- SHADER STATE -----------------

void SetShaderProgram(dx_context context, shader_program_t &shader_program) {
    pn::SetInputLayout(context, shader_program.input_layout_data);
    pn::SetVertexShader(context, shader_program.vertex_shader_data.shader);
    pn::SetPixelShader(context, shader_program.pixel_shader_data.shader);
}

void SetVertexShader(dx_context context, dx_vertex_shader shader) {
    context->VSSetShader(shader.Get(), nullptr, 0);
}
void SetPixelShader(dx_context context, dx_pixel_shader shader) {
    context->PSSetShader(shader.Get(), nullptr, 0);
}

void SetVertexBuffers(dx_context context, const input_layout_data_t &layout,
                      const mesh_buffer_t &mesh_buffer) {
    const auto NUM_PARAMETERS = layout.desc.size();

    pn::vector<ID3D11Buffer *> vertex_buffers;
    Reserve(vertex_buffers, NUM_PARAMETERS);

    pn::vector<unsigned int> strides;
    Reserve(strides, NUM_PARAMETERS);

    pn::vector<unsigned int> offsets;
    Reserve(offsets, NUM_PARAMETERS);

    for(size_t i = 0; i < layout.desc.size(); ++i) {
	const auto & el    = layout.desc[i];
	unsigned int index = layout.desc[i].SemanticIndex;
	std::string  type  = el.SemanticName;
	if(type == "POSITION") {
	    PushBack(vertex_buffers, mesh_buffer.vertices.Get());
	    PushBack(strides, sizeof(pn::vec3f));
	    PushBack(offsets, 0);
	} else if(type == "NORMAL") {
	    PushBack(vertex_buffers, mesh_buffer.normals.Get());
	    PushBack(strides, sizeof(pn::vec3f));
	    PushBack(offsets, 0);
	} else if(type == "TEXCOORD") {
	    if(index == 0) {
		PushBack(vertex_buffers, mesh_buffer.uvs.Get());
		PushBack(strides, sizeof(pn::vec2f));
		PushBack(offsets, 0);
	    } else if(index == 1) {
		PushBack(vertex_buffers, mesh_buffer.uv2s.Get());
		PushBack(strides, sizeof(pn::vec2f));
		PushBack(offsets, 0);
	    } else {
		LogError("TEXCOORD with index {} not implemented", index);
	    }
	} else if(type == "TANGENT") {
	    if(index == 0) {
		PushBack(vertex_buffers, mesh_buffer.tangents.Get());
		PushBack(strides, sizeof(pn::vec3f));
		PushBack(offsets, 0);
	    } else if(index == 1) {
		PushBack(vertex_buffers, mesh_buffer.bitangents.Get());
		PushBack(strides, sizeof(pn::vec3f));
		PushBack(offsets, 0);
	    } else {
		LogError("TANGENT with index {} not implemented", index);
	    }
	} else if(type == "COLOR") {
	    PushBack(vertex_buffers, mesh_buffer.colors.Get());
	    PushBack(strides, sizeof(pn::vec4f));
	    PushBack(offsets, 0);
	} else {
	    LogError("Unknown parameter type '{}' in MeshBuffer", type);
	}
    }

    context->IASetVertexBuffers(
        0, vertex_buffers.size(),
        const_cast<const pn::vector<ID3D11Buffer *> &>(vertex_buffers).data(), strides.data(),
        offsets.data());
    context->IASetIndexBuffer(mesh_buffer.indices.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(mesh_buffer.topology);
}

void SetInputLayout(dx_context context, const input_layout_data_t &layout_desc) {
    context->IASetInputLayout(layout_desc.ptr.Get());
}

void SetVSConstantBuffer(dx_context context, const pn::string &buffer_name,
                         dx_shader_reflection reflection, dx_buffer &buffer) {
    unsigned int start_slot = GetShaderResourceStartSlot(reflection, buffer_name);
    if(start_slot == 0)
	return;
    context->VSSetConstantBuffers(start_slot, 1, buffer.GetAddressOf());
}
void SetPSConstantBuffer(dx_context context, const pn::string &buffer_name,
                         dx_shader_reflection reflection, dx_buffer &buffer) {
    unsigned int start_slot = GetShaderResourceStartSlot(reflection, buffer_name);
    if(start_slot == 0)
	return;
    context->PSSetConstantBuffers(start_slot, 1, buffer.GetAddressOf());
}

void SetVSShaderResources(dx_context context, const pn::string &resource_name,
                          dx_shader_reflection reflection, dx_resource_view &resource_view) {
    unsigned int start_slot = GetShaderResourceStartSlot(reflection, resource_name);
    if(start_slot == 0)
	return;
    context->VSSetShaderResources(start_slot, 1, resource_view.GetAddressOf());
}
void SetPSShaderResources(dx_context context, const pn::string &resource_name,
                          dx_shader_reflection reflection, dx_resource_view &resource_view) {
    unsigned int start_slot = GetShaderResourceStartSlot(reflection, resource_name);
    if(start_slot == 0)
	return;
    context->PSSetShaderResources(start_slot, 1, resource_view.GetAddressOf());
}

void SetVSSamplers(dx_context context, const pn::string &sampler_name,
                   dx_shader_reflection reflection, dx_sampler_state &sampler_state) {
    unsigned int start_slot = GetShaderResourceStartSlot(reflection, sampler_name);
    if(start_slot == 0)
	return;
    context->VSSetSamplers(start_slot, 1, sampler_state.GetAddressOf());
}
void SetPSSamplers(dx_context context, const pn::string &sampler_name,
                   dx_shader_reflection reflection, dx_sampler_state &sampler_state) {
    unsigned int start_slot = GetShaderResourceStartSlot(reflection, sampler_name);
    if(start_slot == 0)
	return;
    context->PSSetSamplers(start_slot, 1, sampler_state.GetAddressOf());
}

// ----------- BLENDING ----------------

const D3D11_BLEND_DESC GetDefaultBlendDesc(const int render_target) {
    D3D11_BLEND_DESC blend_desc;
    ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));
    blend_desc.IndependentBlendEnable                            = false;
    blend_desc.RenderTarget[render_target].BlendEnable           = true;
    blend_desc.RenderTarget[render_target].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[render_target].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[render_target].BlendOp               = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[render_target].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[render_target].DestBlendAlpha        = D3D11_BLEND_ZERO;
    blend_desc.RenderTarget[render_target].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[render_target].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    return blend_desc;
}

dx_blend_state CreateBlendState(dx_device device, const D3D11_BLEND_DESC &blend_desc) {
    pn::dx_blend_state blend_state;
    auto               hr = device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());
    if(FAILED(hr)) {
	LogError("Couldn't create blend state: {}", pn::ErrMsg(hr));
    }
    return blend_state;
}

void SetBlendState(dx_device device, dx_blend_state blend_state) {
    GetContext(device)->OMSetBlendState(blend_state.Get(), 0, 0xffffffff);
}

// --------- DEPTH STENCIL -------------

const D3D11_DEPTH_STENCIL_DESC GetDefaultDepthStencilDesc() {

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;

    depth_stencil_desc.DepthEnable    = true;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc      = D3D11_COMPARISON_LESS;

    depth_stencil_desc.StencilEnable    = false;
    depth_stencil_desc.StencilReadMask  = 255;
    depth_stencil_desc.StencilWriteMask = 255;

    D3D11_DEPTH_STENCILOP_DESC front_face_op;
    front_face_op.StencilPassOp      = D3D11_STENCIL_OP_INCR;
    front_face_op.StencilFailOp      = D3D11_STENCIL_OP_DECR;
    front_face_op.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    front_face_op.StencilFunc        = D3D11_COMPARISON_LESS;
    depth_stencil_desc.FrontFace     = front_face_op;

    D3D11_DEPTH_STENCILOP_DESC back_face_op;
    back_face_op.StencilPassOp      = D3D11_STENCIL_OP_INCR;
    back_face_op.StencilFailOp      = D3D11_STENCIL_OP_DECR;
    back_face_op.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    back_face_op.StencilFunc        = D3D11_COMPARISON_LESS;
    depth_stencil_desc.FrontFace    = back_face_op;

    return depth_stencil_desc;
}

dx_depth_stencil_state CreateDepthStencilState(dx_device                       device,
                                               const D3D11_DEPTH_STENCIL_DESC &depth_stencil_desc) {
    dx_depth_stencil_state depth_stencil_state;

    auto hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
    if(FAILED(hr)) {
	LogError("Couldn't create depth stencil state: {}", pn::ErrMsg(hr));
    }
    return depth_stencil_state;
}

// ----------- DRAWING FUNCTIONS ------------

void DrawIndexed(dx_context context, const mesh_buffer_t &mesh_buffer,
                 unsigned int start_vertex_location, unsigned int base_vertex_location) {
    context->DrawIndexed(static_cast<unsigned int>(mesh_buffer.index_count), start_vertex_location,
                         base_vertex_location);
}

} // namespace pn