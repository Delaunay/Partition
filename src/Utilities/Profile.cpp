
#include <time.h>

#include <Utilities/Logging.h>
#include <Utilities/Profile.h>

#include <chrono>

// There are other clocks, but this is usually the one you want.
// It corresponds to CLOCK_MONOTONIC at the syscall level.
using Clock = std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::time_point;
using namespace std::literals::chrono_literals;

namespace pn {

struct profile_data {
    string            name;
    time_point<Clock> start_time;
};

pn::vector<profile_data> profiling_stack{};

void StartProfile(const string &name) {
    profile_data data = {name, Clock::now()};
    pn::PushBack(profiling_stack, data);
}
void EndProfile() {
    time_point<Clock> now             = Clock::now();
    auto              current_profile = pn::Pop(profiling_stack);
    auto              diff = duration_cast<milliseconds>(now - current_profile.start_time);
    auto              ms   = diff.count();
    Log("[ PROFILE ] {} took {} ms", current_profile.name, ms);
}

} // namespace pn