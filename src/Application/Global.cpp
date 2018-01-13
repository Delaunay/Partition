#include <Application/Global.h>

namespace pn {

namespace app {

// -------- VARIABLES -----------

pn::application_window_desc window_desc;

float FPS;
float FIXED_DT;
float dt;
float time_since_application_start;

bool exit;

// ---------- FUNCTIONS ---------

void Exit() { exit = true; }
bool ShouldExit() { return exit; }

} // namespace app

} // namespace pn