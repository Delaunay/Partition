#pragma once

#include <Application/ResourceDatabaseTypes.h>

#include <Component/transform_t.h>

#include <Utilities/UtilityTypes.h>

#include <Graphics/DirectX.h>

namespace pn::rdb {

// ----- TYPEDEFS ----------
using mesh_resource_t	= pn::mesh_buffer_t;
using mesh_transform_t	= pn::transform_t;
using mesh_children_t	= pn::vector<mesh_resource_id_t>;

// -------- FUNCTIONS ------------

// ----- MESH DATA FUNCTIONS -----------

mesh_resource_id_t		AddMeshResource(mesh_resource_t& mesh);
void				RemoveMeshResource(const mesh_resource_id_t key);
mesh_resource_t		GetMeshResource(const mesh_resource_id_t key);
mesh_resource_t		GetMeshResource(const pn::string& name);

void				AddMeshTransform(const mesh_resource_id_t mesh_id, const mesh_transform_t& transform);
void				RemoveMeshTransform(const mesh_resource_id_t mesh_id, const mesh_transform_t& transform);
mesh_transform_t	GetMeshTransform(const mesh_resource_id_t mesh_id);

void				AddMeshChild(const mesh_resource_id_t mesh_id, const mesh_resource_id_t child_id);
void				RemoveMeshChild(const mesh_resource_id_t mesh_id, const mesh_resource_id_t child_id);
mesh_children_t		GetMeshChildren(const mesh_resource_id_t mesh_id);

} // namespace pn::rdb