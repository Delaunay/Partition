#pragma once

#include <spdlog/spdlog.h>

namespace pn {

// ------------------ TYPEDEFS -------------------------

template<typename T>
using log_ptr = std::shared_ptr<T>;

// ---------------- VARIABLES -------------

extern log_ptr<spdlog::logger> console;

// ---------------- FUNCTIONS --------------

std::string ErrMsg(const HRESULT hr);
std::string ErrMsg(const DWORD error);

void InitLogger();
void CloseLogger();

extern constexpr const char* file_name_from_path(const char* str);

template<typename ... Args>
void __Log(const spdlog::level::level_enum level, const std::string& filename, const std::string& fn_name, const int line_number, const char* fmt, const Args& ... args) {
	auto message = fmt::format(fmt, args...);
	auto final_message = fmt::format("{3} ({0}:{1}:{2})", filename, fn_name, line_number, message);
	console->log(level, final_message);
}

} // namespace pn

  // ----------------- MACRO DEFINITIONS ------------------
#define TYPENAME(t) typeid(t).name()

#define BaseLog(level, ...) pn::__Log(level, pn::file_name_from_path(__FILE__), __FUNCTION__, __LINE__, __VA_ARGS__)
#define Log(...) BaseLog(spdlog::level::level_enum::trace, __VA_ARGS__)
#define LogDebug(...) BaseLog(spdlog::level::level_enum::debug, __VA_ARGS__)
#define LogError(...) BaseLog(spdlog::level::level_enum::err, __VA_ARGS__)
