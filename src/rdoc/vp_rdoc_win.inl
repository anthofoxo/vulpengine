#include <Windows.h>
#include <shlobj_core.h>

#include <format>

namespace vulpengine::rdoc {
	void setup(bool load) {
		HMODULE library = GetModuleHandleA("renderdoc.dll");
		
#ifdef VP_FEATURE_RDOC_UNSUPPORTED
		if (load && library == nullptr) {
			CHAR pf[MAX_PATH];
			SHGetSpecialFolderPathA(nullptr, pf, CSIDL_PROGRAM_FILES, false);
			library = LoadLibraryA(std::format("{}/RenderDoc/renderdoc.dll", pf).c_str());
		}
#endif // VP_FEATURE_RDOC_UNSUPPORTED

		if (library == nullptr) return;

		pRENDERDOC_GetAPI getApi = (pRENDERDOC_GetAPI)GetProcAddress(library, "RENDERDOC_GetAPI");
		if (getApi == nullptr) return;
		getApi(eRENDERDOC_API_Version_1_0_0, (void**)&gApi);
		if (!gApi) return;

		gApi->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
		gApi->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);
	}
}