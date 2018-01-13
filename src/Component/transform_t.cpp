#include <Component/transform_t.h>

#include <UI/UIUtil.h>
#include <Utilities/Logging.h>

#include <imgui/imgui_internal.h>

namespace pn {

pn::mat4f LocalToWorldMatrix(const transform_t &transform) {
    auto cur_srt     = TransformToMatrix(transform);
    auto *cur_parent = transform.parent;
    while(cur_parent != nullptr) {
        cur_srt *= TransformToMatrix(*cur_parent);
        cur_parent = cur_parent->parent;
    }
    return cur_srt;
}

pn::mat4f WorldToLocalMatrix(const transform_t &transform) {
    return Inverse(LocalToWorldMatrix(transform));
}

pn::vec3f TransformPoint(const transform_t &transform, const pn::vec3f &point) {
    return TransformPoint(transform, vec4f(point, 1.0f)).xyz();
}
pn::vec3f TransformDirection(const transform_t &transform, const pn::vec3f &direction) {
    return TransformDirection(transform, vec4f(direction, 0.0f)).xyz();
}
pn::vec3f TransformVector(const transform_t &transform, const pn::vec3f &vector) {
    return TransformVector(transform, vec4f(vector, 0.0f)).xyz();
}

pn::vec4f TransformPoint(const transform_t &transform, const pn::vec4f &point) {
    assert(point.w == 1.0f);
    return point * LocalToWorldMatrix(transform);
}
pn::vec4f TransformDirection(const transform_t &transform, const pn::vec4f &direction) {
    assert(direction.w == 0.0f);
    return Normalize(direction * LocalToWorldMatrix(transform));
}
pn::vec4f TransformVector(const transform_t &transform, const pn::vec4f &vector) {
    assert(vector.w == 0.0f);
    return vector * LocalToWorldMatrix(transform);
}

pn::vec3f InverseTransformPoint(const transform_t &transform, const pn::vec3f &point) {
    return InverseTransformPoint(transform, vec4f(point, 1.0f)).xyz();
}
pn::vec3f InverseTransformDirection(const transform_t &transform, const pn::vec3f &direction) {
    return InverseTransformDirection(transform, vec4f(direction, 0.0f)).xyz();
}
pn::vec3f InverseTransformVector(const transform_t &transform, const pn::vec3f &vector) {
    return InverseTransformVector(transform, vec4f(vector, 0.0f)).xyz();
}

pn::vec4f InverseTransformPoint(const transform_t &transform, const pn::vec4f &point) {
    assert(point.w == 1.0f);
    return point * WorldToLocalMatrix(transform);
}
pn::vec4f InverseTransformDirection(const transform_t &transform, const pn::vec4f &direction) {
    assert(direction.w == 0.0f);
    return Normalize(direction * WorldToLocalMatrix(transform));
}
pn::vec4f InverseTransformVector(const transform_t &transform, const pn::vec4f &vector) {
    assert(vector.w == 0.0f);
    return vector * WorldToLocalMatrix(transform);
}

pn::mat4f TransformToMatrix(const transform_t &transform) {
    return SRTMatrix(transform.scale, transform.rotation, transform.position);
}

namespace gui {

template <> void EditStruct(pn::transform_t &transform) {
    DragFloat3("position", &transform.position.x, -INFINITY, INFINITY);
    DragRotation("rotation", &transform.rotation, TransformDirection(transform, vec3f::UnitX),
                 TransformDirection(transform, vec3f::UnitY),
                 TransformDirection(transform, vec3f::UnitZ));
    DragFloat3("scale", &transform.scale.x, -INFINITY, INFINITY);
}

} // namespace gui

} // namespace pn
