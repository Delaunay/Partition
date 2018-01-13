#include <UI/UIUtil.h>

#include <Input/Input.h>

#include <imgui/imgui_internal.h>

// -------- VARIABLES ---------

int g_debugVariableCount = 0;

const float NORMAL_DRAG_SPEED = 0.1f;
const float SLOW_DRAG_SPEED   = 0.01f;

// ------ FUNCTIONS ----------

pn::string GetDefaultVariableName(int id) {
    pn::string s = pn::string("v") + std::to_string(id);
    return s;
}

template <>
void ImGuiCall(const pn::string &name, float *value, float min, float max, ui::default_t s) {
    ImGui::SliderFloat(name.c_str(), value, min, max);
}

template <>
void ImGuiCall<pn::mat4f *, float, ui::transform_t>(const pn::string &name, pn::mat4f *value,
                                                    float min, float max, ui::transform_t s) {
    pn::vec4f translation, scale;
    pn::quaternion rotation;
    // Decompose(*value, translation, rotation, scale);

    ImGui::SliderFloat3((name + " position").c_str(), (float *)&translation, min, max);
    ImGui::SliderFloat3((name + " scale").c_str(), (float *)&scale, 0, max);
    ImGuiCall(name + " rotation", &rotation, -10.0f, 10.0f, ui::quaternion_t());
    // ImGui::SliderFloat4((name + " rotation").c_str(), (float*) &rotation, min, max);
    //*value = DirectX::XMMatrixScalingFromVector(scale) *
    //DirectX::XMMatrixRotationQuaternion(rotation) *
    //DirectX::XMMatrixTranslationFromVector(translation);
}

namespace pn {

namespace gui {

float GetDragSpeed() {
    float DRAG_SPEED = NORMAL_DRAG_SPEED;
    if(pn::input::GetKeyState(pn::input::input_key::CONTROL) == pn::input::key_state::PRESSED)
        DRAG_SPEED = SLOW_DRAG_SPEED;
    return DRAG_SPEED;
}

bool DragFloat(const char *label, float *v, float min, float max, float speed_modifer,
               const char *display_format, float power) {
    return ImGui::DragFloat(label, v, GetDragSpeed() * speed_modifer, min, max, display_format,
                            power);
}
bool DragFloat2(const char *label, float *v, float min, float max, float speed_modifier,
                const char *display_format, float power) {
    return ImGui::DragFloat2(label, v, GetDragSpeed() * speed_modifier, min, max, display_format,
                             power);
}
bool DragFloat3(const char *label, float *v, float min, float max, float speed_modifer,
                const char *display_format, float power) {
    return ImGui::DragFloat3(label, v, GetDragSpeed() * speed_modifer, min, max, display_format,
                             power);
}
bool DragFloat4(const char *label, float *v, float min, float max, float speed_modifer,
                const char *display_format, float power) {
    return ImGui::DragFloat4(label, v, GetDragSpeed() * speed_modifer, min, max, display_format,
                             power);
}

void DragRotation(const char *label, quaternion *q, vec3f x, vec3f y, vec3f z,
                  float speed_modifier) {
    const float ROT_SCALE = 0.01f * speed_modifier;

    auto euler = QuaternionToEuler(*q);

    // this is a mess, lots of inlining imgui functions
    using namespace ImGui;
    ImGuiContext &g    = *GImGui;
    float return_delta = 0.0f;
    BeginGroup();
    PushID(label);
    ImGuiWindow *window     = ImGui::GetCurrentWindow();
    const ImGuiStyle &style = GImGui->Style;
    auto w_full             = ImGui::CalcItemWidth();
    int components          = 3;
    const float w_item_one =
        ImMax(1.0f, (float)(int)((w_full - (style.ItemInnerSpacing.x) * (components - 1)) /
                                 (float)components));
    const float w_item_last = ImMax(
        1.0f, (float)(int)(w_full - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));
    window->DC.ItemWidthStack.push_back(w_item_last);
    for(int i = 0; i < components - 1; i++)
        window->DC.ItemWidthStack.push_back(w_item_one);
    window->DC.ItemWidth = window->DC.ItemWidthStack.back();

    {
        PushID(0);
        float xr = ImGui::DeltaDragFloat("##v", &euler.x);
        if(xr != 0.0f)
            *q *= AxisAngleToQuaternion(x, xr * ROT_SCALE);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
    }

    {
        PushID(1);
        float xy = ImGui::DeltaDragFloat("##v", &euler.y);
        if(xy != 0.0f)
            *q *= AxisAngleToQuaternion(y, xy * ROT_SCALE);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
    }

    {
        PushID(2);
        float xz = ImGui::DeltaDragFloat("##v", &euler.z);
        if(xz != 0.0f)
            *q *= AxisAngleToQuaternion(z, xz * ROT_SCALE);
        SameLine(0, g.Style.ItemInnerSpacing.x);
        PopID();
        PopItemWidth();
    }

    PopID();

    TextUnformatted(label, FindRenderedTextEnd(label));
    EndGroup();
}

} // namespace gui

} // namespace pn
