#pragma once

#include <Graphics/Window.h>

namespace pn {

namespace app {

// -------- VARIABLES -----------

extern pn::application_window_desc window_desc;

extern float FPS;
extern float FIXED_DT;
extern float dt;
extern float time_since_application_start;

// ------ FUNCTIONS ------------

void Exit();
bool ShouldExit();

} // namespace app

} // namespace pn