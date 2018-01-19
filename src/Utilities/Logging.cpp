#include <Utilities/Logging.h>

#include <comdef.h> // _com_error
#include <iostream>

#include <Utilities/UtilityTypes.h>

namespace pn {

// ------------ VARIABLES -------------

log_ptr<spdlog::logger> console;

// --------------- FUNCTIONS ----------

std::string ErrMsg(const HRESULT hr) {
    _com_error err(hr);
    return std::string(err.ErrorMessage());
}

std::string ErrMsg(const DWORD error) {
    auto hresult = HRESULT_FROM_WIN32(error);
    return ErrMsg(hresult);
}

void InitLogger() {
    try {
	pn::vector<spdlog::sink_ptr> sinks;

	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
	auto file_sink = std::make_shared<spdlog::sinks::simple_file_sink_mt>("log.txt", "logfile");
	PushBack(sinks, stdout_sink, file_sink);

	console = std::make_shared<spdlog::logger>("console", std::begin(sinks), std::end(sinks));

	console->set_level(spdlog::level::level_enum::trace);
	console->flush_on(spdlog::level::level_enum::trace);

	spdlog::register_logger(console);
	spdlog::set_pattern("[%D %T.%e] [%l] %v");
    } catch(const spdlog::spdlog_ex &ex) {
	std::cout << "Failed to initialize spdlog: " << ex.what() << '/n';
    }
}

void CloseLogger() { spdlog::drop_all(); }

// from
// https://stackoverflow.com/questions/31050113/how-to-extract-the-source-filename-without-path-and-suffix-at-compile-time
constexpr const char *str_end(const char *str) { return *str ? str_end(str + 1) : str; }

constexpr bool str_slant(const char *str) {
    return *str == '//' ? true : (*str ? str_slant(str + 1) : false);
}

constexpr const char *r_slant(const char *str) {
    return *str == '//' ? (str + 1) : r_slant(str - 1);
}

constexpr const char *file_name_from_path(const char *str) {
    return str_slant(str) ? r_slant(str_end(str)) : str;
}

} // namespace pn