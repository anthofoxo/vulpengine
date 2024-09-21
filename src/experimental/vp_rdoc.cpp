#include "experimental/vp_rdoc.hpp"

#if defined(__has_include) && __has_include(<renderdoc_app.h>)
#	define VP_HAS_RDOC
#endif

#ifdef VP_HAS_RDOC

#ifdef VP_WINDOWS
#	include <Windows.h>
#	include <shlobj_core.h>
#
#	include <format>
#else
#	include <dlfcn.h>
#endif

#include <renderdoc_app.h>

namespace {
	RENDERDOC_API_1_0_0* gApi = nullptr;
}

namespace vulpengine::experimental::rdoc {
#ifdef VP_WINDOWS
	void setup(bool load) {
		HMODULE library = GetModuleHandleA("renderdoc.dll");

		// This method of attaching IS NOT SUPPORTED by the RenderDoc developers.
		if (load && library == nullptr) {
			CHAR pf[MAX_PATH];
			SHGetSpecialFolderPathA(nullptr, pf, CSIDL_PROGRAM_FILES, false);
			library = LoadLibraryA(std::format("{}/RenderDoc/renderdoc.dll", pf).c_str());
		}

		if (library == nullptr) return;

		pRENDERDOC_GetAPI getApi = (pRENDERDOC_GetAPI)GetProcAddress(library, "RENDERDOC_GetAPI");
		if (getApi == nullptr) return;
		getApi(eRENDERDOC_API_Version_1_0_0, (void**)&gApi);
		if (!gApi) return;

		gApi->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
		gApi->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);
	}
#else
	void setup(bool load) {
		void* library = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
		// This method of attaching IS NOT SUPPORTED by the RenderDoc developers.
		if (load && library == nullptr) library = dlopen("librenderdoc.so", RTLD_NOW);
		//
		if (library == nullptr) return;

		pRENDERDOC_GetAPI getApi = (pRENDERDOC_GetAPI)dlsym(library, "RENDERDOC_GetAPI");
		if (getApi == nullptr) return;
		getApi(eRENDERDOC_API_Version_1_0_0, (void**)&gApi);
		if (!gApi) return;

		gApi->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
		gApi->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);
	}
#endif

	bool is_attached() {
		return gApi;
	}

	bool is_target_control_connected() {
		if (!gApi) return false;
		return gApi->IsTargetControlConnected();
	}

	bool is_frame_capturing() {
		if (!gApi) return false;
		return gApi->IsFrameCapturing();
	}

	void trigger_capture() {
		if (gApi) gApi->TriggerCapture();
	}

	void launch_replay_ui() {
		if (gApi) gApi->LaunchReplayUI(true, nullptr);
	}
}

#else // VP_HAS_RDOC

namespace vulpengine::experimental::rdoc {
	void setup(bool load) {}
	bool is_attached() { return false; }
	bool is_target_control_connected() { return false; }
	bool is_frame_capturing() { return false; }
	void trigger_capture() {}
	void launch_replay_ui() {}
}

#endif // !VP_HAS_RDOC