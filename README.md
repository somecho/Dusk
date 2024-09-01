# Dusk

An experimental C++ creative coding framework with a WebGPU
([Dawn](https://dawn.googlesource.com/dawn/)) backend. 

## Installation

Dusk is currently known to work on Linux. However, since this project uses
CMake, you can get probably get it to work on any platform.

### Requirements

Dusk requires:
- CMake 3.15 and above
- GLFW
- glm
- WebGPU Dawn

You can use your distribution's package manager to install GLFW and glm.

As for [WebGPU Dawn](https://dawn.googlesource.com/dawn/), clone it, [build](https://dawn.googlesource.com/dawn/+/HEAD/docs/building.md) it and install it using the CMake install target.


### Using

Dusk currently doesn't provide a CMake install target. The easiest way to get started is to just clone it in your project directory. Then in your  `CMakeLists.txt`:

```CMake
add_subdirectory(Dusk)
target_link_libraries(${YOUR_TARGET} Dusk)
```

## Why Dusk? 

The current two biggest creative coding frameworks in C++ are OpenFrameworks and
Cinder. Both require a fair amount of programming knowledge in C++, but
OpenFrameworks is probably easier to get started with for the seasoned
Processing user. 

Both are also not very easy to use as a standalone C++ library, which Dusk aims
to do by using CMake. Dusk uses WebGPU as the rendering backend. Inspired by Nannou, Dusk makes use of a less conventional _fluent_ api. 

In summary, Dusk
- wants to be a cross-platform, build system agnostic, standalone C++ library
- uses WebGPU as a rendering backend
- has a fluent API inspired by Nannou