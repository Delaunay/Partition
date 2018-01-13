#pragma once

#include <Component/transform_t.h>

#include <Graphics/DirectX.h>

#include <Application/ResourceDatabase.h>

namespace pn {

struct render_data_t {
	pn::rdb::resource_id_t	mesh_id;
	pn::rdb::resource_id_t	material_id;
};

} // namespace pn