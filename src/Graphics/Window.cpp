#include <Graphics/Window.h>

#include <Utilities/Logging.h>

namespace {

auto CreateRect(const int x, const int y, const int width, const int height,
                const DWORD window_style) {
    RECT rc;
    SetRect(&rc, x, y, x + width, y + height);
    AdjustWindowRect(&rc, window_style, false);
    return rc;
}

} // namespace

namespace pn {

window_class CreateWindowClass(const pn::string &class_name, const instance_handle h_instance,
                               const window_procedure window_proc) {
    auto GetIcon = []() {
	HICON h_icon = NULL;
	// TODO: load icon
	// LPSTR exe_path = NULL;
	// GetModuleFileName(NULL, exe_path, MAX_PATH);
	// h_icon = ExtractIcon(h_i, exe_path, 0);
	return h_icon;
    };

    auto GetClassStyle = []() {
	// The class styles define additional elements of the window class
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
	// CS_DBLCLKS => Send double-click message to the window procedure when the user
	// double-clicks
	return CS_DBLCLKS;
    };

    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms633576(v=vs.85).aspx
    window_class wc = {};
    wc.style        = GetClassStyle();
    wc.lpfnWndProc  = window_proc;
    wc.cbClsExtra =
        0; // The number of extra bytes to allocate following the window-class structure.
    wc.cbWndExtra = 0; // The number of extra bytes to allocate following the window instance
    wc.hInstance  = h_instance;
    wc.hCursor    = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground =
        (HBRUSH)GetStockObject(BLACK_BRUSH); // A handle to the class background brush. This member
                                             // can be a handle to the physical brush to be used for
                                             // painting the background, or it can be a color value.
    wc.lpszClassName = class_name.c_str();
    wc.lpszMenuName  = NULL; // The resource name of the class menu, as the name appears in the
                             // resource file. If you use an integer to identify the menu, use the
                             // MAKEINTRESOURCE macro. If this member is NULL, windows belonging to
                             // this class have no default menu.
    wc.hIcon = GetIcon();
    wc.style = CS_OWNDC;

    return wc;
}

void RegisterWindowClass(const window_class &wc) {
    auto success = RegisterClass(&wc);
    if(!success) {
	LogError("Couldn't register window class");
	exit(1);
    }
}

window_handle CreateWindowHandle(const instance_handle h_instance, const pn::string &window_class,
                                 const application_window_desc &awd) {
    int x = 0;
    int y = 0;

    DWORD window_style = WS_OVERLAPPEDWINDOW;
    auto  rect         = CreateRect(x, y, awd.width, awd.height, window_style);

    auto hwnd =
        CreateWindow(window_class.c_str(), "Window Name", window_style, x, y,
                     (rect.right - rect.left), (rect.bottom - rect.top), 0, NULL, h_instance, 0);
    if(hwnd == NULL) {
	LogError("Couldn't create window handle");
	exit(1);
    }
    return hwnd;
}

window_handle CreateApplicationWindow(const application_window_desc &awd,
                                      const window_procedure         window_proc) {
    // Create and register window class
    const pn::string class_name = "ParitionEngine";
    auto             wc         = CreateWindowClass(class_name, awd.h_instance, window_proc);
    RegisterWindowClass(wc);

    // Create window
    auto h_wnd = CreateWindowHandle(awd.h_instance, class_name, awd);

    return h_wnd;
}

void CreateConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

} // namespace pn