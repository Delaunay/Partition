#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>

#include <Utilities/UtilityTypes.h>
#include <Utilities/Math.h>

#include <Component/transform_t.h>

#include <type_traits>

// ------ MACROS --------

#define VDBM(value, min, max) VDB(#value, value, min, max, ui::default_t())
#define VDBMS(value, min, max, s) VDB(#value, value, min, max, s)

// -------- CLASSES ---------

namespace ui {

struct default_t {};
struct transform_t {};
struct quaternion_t {};

}

// ------ VARIABLES ---------

extern int	g_debugVariableCount;

// ----- FUNCTIONS -------------

pn::string	GetDefaultVariableName(int id);

template<typename T, typename M, typename S>
void		ImGuiCall(const pn::string& name, T value, M min, M max, S s) {}

template<>
void		ImGuiCall(const pn::string& name, float* value, float min, float max, ui::default_t s);

template<>
void		ImGuiCall(const pn::string& name, pn::vec4f* value, float min, float max, ui::quaternion_t s);

template<>
void		ImGuiCall(const pn::string& name, pn::mat4f* value, float min, float max, ui::transform_t s);

// T for data type being modified, S for semantic info about the data
// e.g. a mat4 for S = transform is different than plain mat4
template<typename T, typename M, typename S>
void		VDB(const pn::string& name, T value, M min, M max, S s) {
	g_debugVariableCount += 1;
	auto variable_name = name;
	if (name.empty()) {
		variable_name = GetDefaultVariableName(g_debugVariableCount);
	}
	ImGuiCall(variable_name, value, min, max, s);
}

namespace pn {

namespace gui {

bool DragFloat(const char* label, float* v, float min, float max, float speed_modifier = 1.0f, const char* display_format = "%.3f", float power = 1.0f);
bool DragFloat2(const char* label, float* v, float min, float max, float speed_modifier = 1.0f, const char* display_format = "%.3f", float power = 1.0f);
bool DragFloat3(const char* label, float* v, float min, float max, float speed_modifier = 1.0f, const char* display_format = "%.3f", float power = 1.0f);
bool DragFloat4(const char* label, float* v, float min, float max, float speed_modifier = 1.0f, const char* display_format = "%.3f", float power = 1.0f);

void DragRotation(const char* label, quaternion* q, const vec3f x = vec3f::UnitX, const vec3f y = vec3f::UnitY, const vec3f z = vec3f::UnitZ, float speed_modifier = 1.0f);

} // namespace gui

} // namespace pn::gui
