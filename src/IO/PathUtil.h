#pragma once

#include <Utilities/UtilityTypes.h>

namespace pn {

// ------------------- FUNCTIONS --------------------

void	InitPathUtil();

string	GetWorkingDirectory();
void	SetWorkingDirectory(const string& directory);

string	GetResourceDirectory();
void	SetResourceDirectoryName(const string& name);

string	GetResourcePath(const string& resource_name);


} // namespace pn