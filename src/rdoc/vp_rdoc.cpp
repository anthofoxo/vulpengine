#include "vulpengine/vp_rdoc.hpp"

#if defined(__has_include) && __has_include(<renderdoc_app.h>)
#	define VP_HAS_RDOC
#endif

#ifdef VP_HAS_RDOC

#include <renderdoc_app.h>

namespace {
	RENDERDOC_API_1_0_0* gApi = nullptr;
}

// These platform files should define `void attach_shared_lib()`

#ifdef VP_WINDOWS
#	include "vp_rdoc_win.inl"
#endif

#ifdef VP_LINUX
#	include "vp_rdoc_linux.inl"
#endif

namespace vulpengine::rdoc {
	void setup(bool load) {
		attach_shared_lib(load);
		if (!is_attached()) return;
		gApi->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
		gApi->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);
	}

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