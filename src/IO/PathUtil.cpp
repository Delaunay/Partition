#include <IO/PathUtil.h>

namespace pn {

// --------- VARIABLES -----------------

static string working_dir;
static string resource_dir;

// --------- FUNCTIONS -----------------

void InitPathUtil() {
    working_dir  = "";
    resource_dir = "";
}

string GetWorkingDirectory() { return working_dir; }

void SetWorkingDirectory(const string &directory) { working_dir = directory + "/"; }

string GetResourceDirectory() { return resource_dir; }

void SetResourceDirectoryName(const string &name) { resource_dir = working_dir + name + "/"; }

string GetResourcePath(const string &resource_name) { return resource_dir + resource_name; }

} // namespace pn