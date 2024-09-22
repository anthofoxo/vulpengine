# Vulpengine
Vulpengine is an experimental early-stage rendering engine for Windows and Linux. Vulpengine is a general toolset developed by myself used to help myself develop games.

## Downloading the repo
Vulpengine is designed to be used as a static library and git submodule. Add Vulpengine to your submodules with `git submodule add https://github.com/anthofoxo/vulpengine your_submodule_path`.

## Required Dependencies
Vulpengine requires C++20 Support.

Vulpengine requires the following dependencies.
* [GLFW 3.4](https://github.com/glfw/glfw/tree/3.4)
* [Glad](https://gen.glad.sh/#generator=c&api=gl%3D4.6&profile=gl%3Dcore%2Cgles1%3Dcommon) (OpenGL 4.6+)

## Building

### Include Paths
Building Vulpengine is kept simple. Add `include/vulpengine` to the include paths and add the include paths for all the required libraries.
* `include/vulpengine`
* `glfw/include`
* `glad/include`

### Platforms
Vulpengine supports Windows and Linux.
* `#define VP_WINDOWS` for Windows builds.
* `#define VP_LINUX` for Linux builds.

#### Linux
If you're building GLFW yourself you will need to generate the xdg headers for wayland. We've bundled a small tool for this `glfw_generate_xdg.sh`.

### Configurations
Vulpengine supports 3 different configurations. If you only have `debug` and `release` then you can use those.

* For debug builds `#define VP_DEBUG`.
* For release builds `#define VP_RELEASE`.
* For dist builds `#define VP_DIST`.

Dist is similar to release but with extra debugging tools stripped.

### Defines
* You should define `GLFW_INCLUDE_NONE`.
* Define `VP_ENTRY_WINMAIN` if you're using the `WinMain` entrypoint.

## Optional Dependencies

### Spdlog
Vulpengine supports [spdlog](https://github.com/gabime/spdlog/tree/v1.14.1). To enable spdlog support add `spdlog/include` to the include paths.

### Tracy
Vulpengine can detect and use [Tracy](https://github.com/wolfpld/tracy/tree/v0.11.1) if available.

Add `tracy/public` to your include paths. Make sure to `#define TRACY_ENABLE` too.

### RenderDoc
Vulpengine can support [RenderDoc](https://renderdoc.org/) detection. RenderDoc installations include `renderdoc_app.h` in the installation directory. Add this directory to the include paths for Vulpengine to detect and enable support for it.

The code below shows how to detect RenderDoc.

If enabled with `VP_FEATURE_RDOC_UNSUPPORTED` the parameter may be set to `true` to attempt to inject RenderDoc at startup. While this does works currently, it *IS NOT SUPPORTED* by the RenderDoc developers and may break.
```cpp
#include <vulpengine/vp_rdoc.hpp>

// Do this before graphics api creation
vulpengine::experimental::rdoc::setup(false);
```

## stb_image
If stb_image is available the `Image` class will be available along with helper functions for `Texture` creation and upload.

# glm
If glm is available, the `Transform` and `Frustum` features will be enabled.

## General Usage
Once Vulpengine is built. Simply add our `include` directory to your include paths.

All Vulpengine headers are prefixed with `vp_` to avoid name clashes. You may directly add `include/vulpengine` if you like.

## Entry Point / Main Function
Vulpengine will define the main entrypoint, so you can't directly use that. This is mainly to perform some backend work to ensure logging will work.

Otherwise no other processing happens and the argments are directly forwarded to the Vulpengine entry point.

```cpp
#include <vulpengine/vp_entry.hpp>

int vulpengine::main(int argc, char* argv[]) {
	return 0;
}
```

## Wrap (Experimental)
Wrap is a simply box type to get around some odd reference semantics when using value types such as `std::optional` and `std::span`.

Wrap is simply defined as:
```cpp
template<class T> struct Wrap { T value; };
```

These utility functions help to easily assist in wrapping reference types.

### `wrap_cref`
This takes a reference type and wraps the reference.

### `wrap_rvref`
This should be treated like a `std::move`.
This performs a `std::move` on the argument and stores the rvalue reference into the wrapper. This is used during resource transfer into Meshes.

## Mesh API (Experimental)
Meshes are split into 3 parts. Buffers, Vertex Arrays, and Meshes.
Buffers are just OpenGL buffers: Array buffers, element buffers, uniform buffers etc. Vertex arrays are OpenGL vertex arrays. These are constructed with a list of buffers and a list of attributes. Meshes are simple containers to transfer ownership of these resources.

For the example usage assume we have these structs defined:
```cpp
struct Vec3f32 final {
	float x, y;
};

struct Vertex final {
	Vec3f32 position;
};
```

### Buffer (Experimental)
```cpp
Vertex positions[] = {
	{ -0.5f, -0.5f },
	{  0.5f, -0.5f },
	{  0.0f,  0.5f }
};

vulpengine::experimental::Buffer vertexBuffer = {{
	.content = std::as_bytes(std::span(positions)),
	.flags = GL_NONE,
	.label = "Test vertex buffer" // May be omitted
}};
```

### Vertex Array (Experimental)
```cpp
vulpengine::experimental::VertexArray vertexArray = {{
	.buffers = std::array {
		vulpengine::experimental::VertexArray::BufferInfo {
			.buffer = vertexBuffer,
			.offset = 0,
			.stride = sizeof(Vertex),
			.divisor = 0
		}
	},
	.attributes = std::array {
		vulpengine::experimental::VertexArray::AttributeInfo {
			.size = 2,
			.type = GL_FLOAT,
			.relativeoffset = offsetof(Vertex, position),
			.bindingindex = 0,
		}
	},
	// Optionally attach an index buffer
	// .indexBuffer = vulpengine::experimental::wrap_cref(indexBuffer),
	.label = "Test vertex array"
}};
```

### Transfer Ownership into a Mesh
You'll typically want to transfer all static data into the Mesh, however the Mesh doesn't require ownership of buffers.

```cpp
vulpengine::experimental::Mesh mesh = {{
	// usage of std::move is required, this hints to the developer that this takes ownership
	.vertexArray = std::move(vertexArray),
	.buffers = std::array {
		// wrap_rvref
		vulpengine::experimental::wrap_rvref(vertexBuffer),
		vulpengine::experimental::wrap_rvref(indexBuffer),
	},
	.mode = GL_TRIANGLES,
	.count = 3,
	// May be omitted or `GL_NONE` if no index buffer is used.
	.type = GL_UNSIGNED_INT
}};
```