#include <dlfcn.h>

namespace vulpengine::rdoc {
	void setup(bool load) {
		void* library = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
#ifdef VP_FEATURE_RDOC_UNSUPPORTED
		if (load && library == nullptr) library = dlopen("librenderdoc.so", RTLD_NOW);
#endif // VP_FEATURE_RDOC_UNSUPPORTED
		if (library == nullptr) return;

		pRENDERDOC_GetAPI getApi = (pRENDERDOC_GetAPI)dlsym(library, "RENDERDOC_GetAPI");
		if (getApi == nullptr) return;
		getApi(eRENDERDOC_API_Version_1_0_0, (void**)&gApi);
		if (!gApi) return;

		gApi->MaskOverlayBits(eRENDERDOC_Overlay_None, eRENDERDOC_Overlay_None);
		gApi->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);
	}
}