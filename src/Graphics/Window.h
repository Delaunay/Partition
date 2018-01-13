#pragma once

#include <Windows.h>

#include <Utilities/UtilityTypes.h>

namespace pn {

// ------------- TYPEDEFS ------------------

using instance_handle	= HINSTANCE;
using window_handle		= HWND;
using menu_handle		= HMENU;

using window_class		= WNDCLASS;
using window_procedure	= WNDPROC;

using window_long		= LONG_PTR;
using window_uint		= UINT_PTR;
using window_pwstr		= PWSTR;

// --------- CLASS DEFINITIONS -------------

struct application_window_desc {
	HINSTANCE	h_instance;
	int			width;
	int			height;
	bool		fullscreen;
};

// ------------- FUNCTIONS -----------------

window_class	CreateWindowClass(const pn::string& class_name, const instance_handle h_instance, const window_procedure window_proc);
void			RegisterWindowClass(const window_class& wc);

window_handle	CreateWindowHandle(const instance_handle h_instance, const pn::string& window_class, const application_window_desc& awd);
window_handle	CreateApplicationWindow(const application_window_desc& awd, const window_procedure window_proc);

void			CreateConsole();

// TODO:
// LoadApplicationWindowDescFromFile


} // namespace pn