#pragma once

#include <string>
#include <vector>

#include <Application/ResourceDatabaseTypes.h>
#include <Graphics/DirectX.h>
#include <Utilities/UtilityTypes.h>

namespace pn {

// -------- CLASS DEFINITIONS ------------

struct MeshLoadData {
	bool triangulate;
	bool convert_left;
};

// ---------- FUNCTIONS --------------------

void InitMeshLoadUtil(dx_device device);

pn::rdb::resource_id_t	LoadMesh(const std::string& filename, const MeshLoadData& mesh_load_data);
pn::rdb::resource_id_t	LoadMesh(const std::string& filename);

} // namespace pn