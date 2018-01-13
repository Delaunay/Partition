#include <Graphics/TextureLoadUtil.h>

#include <IO/FileUtil.h>

#include <memory>

#include <DirectXTK/WICTextureLoader.h>

namespace pn {

// ------- VARIABLES -----------

static dx_device _device;
static dx_context _context;

// ------ FUNCTIONS ---------

void InitTextureFactory(dx_device device) {
    _device = device;
    device->GetImmediateContext(_context.GetAddressOf());
}

texture_t LoadTexture2D(const string &filepath) {
    auto image_data = ReadResource(filepath);

    dx_resource resource;
    dx_resource_view resource_view;

    auto hr = DirectX::CreateWICTextureFromMemoryEx(
        _device.Get(), _context.Get(), (const uint8_t *)(image_data.data()), image_data.size(), 0,
        D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
        DirectX::WIC_LOADER_IGNORE_SRGB, resource.GetAddressOf(), resource_view.GetAddressOf());

    if(FAILED(hr)) {
        LogError("Couldn't create texture: {}", ErrMsg(hr));
    }

    texture_t t;
    t.resource      = resource;
    t.resource_view = resource_view;
    return t;
}

} // namespace pn