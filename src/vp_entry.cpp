#include "vp_entry.hpp"
#include "vp_log.hpp"

int main(int argc, char* argv[]) {
#ifdef VP_HAS_SPDLOG
	spdlog::set_level(spdlog::level::level_enum::trace);
#endif
	int code = vulpengine::main(argc, argv);
#ifdef VP_HAS_SPDLOG
	spdlog::shutdown();
#endif
	return code;
}

#ifdef VP_ENTRY_WINMAIN
#include <Windows.h>
#include <stdlib.h> // __argc, __argv

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	return main(__argc, __argv);
}
#endif