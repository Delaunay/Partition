#pragma once

#include <Utilities/UtilityTypes.h>

namespace pn {

bytes ReadFile(const string& filename);
bytes ReadResource(const string& resource_path);

} // namespace pn