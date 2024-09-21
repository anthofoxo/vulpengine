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

## The `Wrap` Struct
Wrap is simply defined as:
```cpp
template<class T> struct Wrap { T value; };
```

This wrapper is used to get around some type issues related to references, in particular when combined with value container types such as `std::optional` and `std::span`.

`std::reference_wrapper` may be a good choice but this hasn't been tested.

Some utility functions are defined to assist in handling wrapping reference types.

### `wrap_cref`
This takes a reference type and wraps the reference.

### `wrap_rvref`
This should be treated like a `std::move`.
This performs a `std::move` on the argument and stores the rvalue reference into the wrapper. This is used during resource transfer into Meshes.

## Experimental Mesh API
Meshes are split into 3 parts. Buffers, Vertex Arrays, and Meshes.
Buffers are just OpenGL buffers: Array buffers, element Buffers, uniform buffers etc. Vertex arrays are OpenGL vertex arrays. These are constructed with a list of buffers and a list of attributes. Meshes are simple owning containers for these resources.

### Usage
For the example usage assume we have these structs defined:
```cpp
struct Vec3f32 final {
	float x, y;
};

struct Vertex final {
	Vec3f32 position;
};
```

### Create a Buffer and Upload Vertex Data
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

### Create a Vertex Array with the Buffer and Attributes
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
You'll typically want to transfer all static data into the Mesh however the Mesh doesn't require ownership of buffers.

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