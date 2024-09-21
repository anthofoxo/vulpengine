# Vulpengine
Vulpengine is an experimental early-stage rendering engine for Windows and Linux. Vulpengine is a general toolset used to help myself develop my games.

## Downloading the repo
Vulpengine is designed to be used as a static library and submodule. Add Vulpengine to your submodule with `git submodule add https://github.com/anthofoxo/vulpengine your_submodule_path`.

## Required Dependencies
Vulpengine requires C++20 Support.

Vulpengine requires the following dependencies.
* [GLFW 3.4](https://github.com/glfw/glfw/tree/3.4)
* [Glad](https://gen.glad.sh/#generator=c&api=gl%3D3.3&profile=gl%3Dcore%2Cgles1%3Dcommon)
* * 3.3+ Core Profile

## Optional Dependencies
Vulpengine can optionally support these dependencies.
* [spdlog v1.14.1](https://github.com/gabime/spdlog/tree/v1.14.1)
* [Tracy v0.11.1](https://github.com/wolfpld/tracy/tree/v0.11.1)

## Building

### Include Paths
* `include/vulpengine`
* `glfw/include`
* `glad/include`

### Defines
You should define `GLFW_INCLUDE_NONE` for Vulpengine.

Define `VP_ENTRY_WINMAIN` if you're using the `WinMain` entrypoint.

### Spdlog Support
Vulpengine will detect and use spdlog if its in the include paths.
Add `spdlog/include` to your include paths.

### Tracy Support
Vulpengine will detect and use tracy if its in the include paths.
Add `tracy/public` to your include paths. Make sure to define `TRACY_ENABLE` too.

## Using
Once Vulpengine is built. Simply add the `include` directory to your include paths.

All Vulpengine headers are prefixed with `vp_` so you may directly add `include/vulpengine` if you like.

Vulpengine will define the main function, thus you can't use it. This is mainly to cleanly handle `WinMain` and certain allocations. Vulpengine otherwise will forward everything to its main function.

```cpp
#include "vulpengine/vp_entry.hpp"

int vulpengine::main(int argc, char* argv[]) {
	return 0;
}
```