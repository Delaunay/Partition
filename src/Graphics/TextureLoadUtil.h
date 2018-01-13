#pragma once

#include <Utilities/UtilityTypes.h>
#include <Graphics/DirectX.h>

namespace pn {

// ------- FUNCTIONS ---------

void InitTextureFactory(dx_device device);
texture_t LoadTexture2D(const string& filepath);


} // namespace pn