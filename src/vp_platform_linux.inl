#include <sys/utsname.h>

#include <string_view>

namespace vulpengine {
	bool is_wsl() {
		utsname uname_info;
		uname(&uname_info);

		std::string_view result = uname_info.release;
		return (result.find("microsoft") != std::string_view::npos) && (result.find("WSL2") != std::string_view::npos);
	}
}