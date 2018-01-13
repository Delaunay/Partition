#pragma once

#include <UI/UIUtil.h>

#include <Utilities/UtilityTypes.h>

#include <functional>
#include <type_traits>
#include <any>

using std::conditional_t;

template<typename T, typename F>
using to_type = std::conditional_t<true, T, F>;

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace pn {

namespace gui {

// --------- CLASS DEFINITIONS ---------

struct StringValue {
	pn::string data;

	StringValue(const string& s) : data(s) {}
	StringValue(const char* s) : data(s) {}
	StringValue(int s) : data(std::to_string(s)) {}
	StringValue(unsigned int s) : data(std::to_string(s)) {}
	StringValue(float s) : data(std::to_string(s)) {}
	StringValue(double s) : data(std::to_string(s)) {}
	StringValue(char s) : data(std::to_string(s)) {}
	StringValue(unsigned char s) : data(std::to_string(s)) {}

	operator string() { return data; }

	template<typename T>
	operator T() {
		std::istringstream iss(data);
		T new_data;
		iss >> new_data;
		return new_data;
	}
};



// ----------- FUNCTIONS ----------------

void InitEditorUI();
void ShutdownEditorUI();

void SetMainMenuVisible(bool value);
void DrawMainMenu(const unsigned int screen_width);

// --------- COMMAND LINE --------------

void SetCommandLine(bool value);
void DrawCommandLine();

using function_map = map<string, std::any>;
extern map<int, function_map> functions;

template<typename R, typename... Args>
void RegisterCommand(const pn::string& name, std::function<R(Args...)> f) {
	std::function<StringValue(to_type<StringValue, Args>...)> command = [=](to_type<StringValue, Args>... args) {
		return StringValue(f(static_cast<Args>(args)...));
	};
	const int nargs = sizeof...(Args);
	if (!Contains(functions, nargs)) {
		functions.emplace(std::make_pair(nargs, function_map()));
	}
	else if (Contains(functions[nargs], name)) {
		Log("Command map already contains {}", name);
		return;
	}
	functions[nargs].emplace(std::make_pair(name, command));
}
#define REGISTER_COMMAND(f, R, ...) pn::gui::RegisterCommand(#f, std::function<R(__VA_ARGS__)>(f))

}

}