#pragma once

#include <Utilities/Math.h>
#include <Utilities/UtilityTypes.h>

#include <UI/EditStruct.h>

namespace pn {

struct transform_t {
	using transform_ref = transform_t*;
	pn::vec3f					position	= { 0, 0, 0 };
	pn::quaternion				rotation	= { 0, 0, 0, 1 };
	pn::vec3f					scale		= { 1, 1, 1 };
	transform_ref				parent		= nullptr;
	pn::vector<transform_ref>	children;
};

pn::mat4f LocalToWorldMatrix(const transform_t& transform);
pn::mat4f WorldToLocalMatrix(const transform_t& transform);

pn::vec3f TransformPoint(const transform_t& transform, const pn::vec3f& point);
pn::vec3f TransformDirection(const transform_t& transform, const pn::vec3f& direction);
pn::vec3f TransformVector(const transform_t& transform, const pn::vec3f& vector);

pn::vec4f TransformPoint(const transform_t& transform, const pn::vec4f& point);
pn::vec4f TransformDirection(const transform_t& transform, const pn::vec4f& direction);
pn::vec4f TransformVector(const transform_t& transform, const pn::vec4f& vector);

pn::vec3f InverseTransformPoint(const transform_t& transform, const pn::vec3f& point);
pn::vec3f InverseTransformDirection(const transform_t& transform, const pn::vec3f& direction);
pn::vec3f InverseTransformVector(const transform_t& transform, const pn::vec3f& vector);

pn::vec4f InverseTransformPoint(const transform_t& transform, const pn::vec4f& point);
pn::vec4f InverseTransformDirection(const transform_t& transform, const pn::vec4f& direction);
pn::vec4f InverseTransformVector(const transform_t& transform, const pn::vec4f& vector);

pn::mat4f TransformToMatrix(const transform_t& transform);

namespace gui {

template<>
void EditStruct(pn::transform_t& transform);

} // namespace gui

} // namespace pn

