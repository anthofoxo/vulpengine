#include "vp_entry.hpp"
#include "vp_log.hpp"

namespace vulpengine {
	void entry_init() {
#ifdef VP_HAS_SPDLOG
		spdlog::set_level(spdlog::level::level_enum::trace);
#endif
	}

	void entry_uninit() {
#ifdef VP_HAS_SPDLOG
		spdlog::default_logger()->flush();
		spdlog::drop_all();
		spdlog::shutdown();
#endif
	}
}

#ifndef VP_NO_ENTRYPOINT

/*
* To disable vulpengine from implementing main:
* #define VP_NO_ENTRYPOINT
*/
int main(int argc, char* argv[]) {
	vulpengine::entry_init();
	int status = vulpengine::main(argc, argv);
	vulpengine::entry_uninit();
	return status;
}

#ifdef VP_ENTRY_WINMAIN
#include <Windows.h>
#include <stdlib.h> // __argc, __argv

/*
* To disable vulpengine from implementing main:
* #define VP_NO_ENTRYPOINT
*/
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	return main(__argc, __argv);
}
#endif // VP_ENTRY_WINMAIN

#endif // VP_NO_ENTRYPOINT