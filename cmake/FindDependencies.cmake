# Exported target Threads::Threads required by Dawn
find_package(Threads REQUIRED)

message(STATUS "Find Dawn")
find_package(Dawn QUIET)

if(NOT Dawn_FOUND)
  message(STATUS "Find Dawn: not found")
  message(STATUS "Find Dawn: fetching")

  include(FetchContent)
  set(FETCHCONTENT_QUIET FALSE)

  set(DAWN_FETCH_DEPENDENCIES ON)

  set(DAWN_BUILD_PROTOBUF OFF)
  set(DAWN_BUILD_SAMPLES OFF)
  set(DAWN_BUILD_TESTS OFF)

  # Dusk does not use Tint, it is disabled as much as possible.
  set(TINT_BUILD_CMD_TOOLS OFF)
  set(TINT_BUILD_IR_BINARY OFF)
  set(TINT_BUILD_TESTS OFF)
  set(TINT_BUILD_SPV_READER OFF)
  set(TINT_BUILD_WGSL_READER OFF)
  set(TINT_BUILD_GLSL_WRITER OFF)
  set(TINT_BUILD_GLSL_VALIDATOR OFF)
  set(TINT_BUILD_HLSL_WRITER OFF)
  set(TINT_BUILD_MSL_WRITER OFF)
  set(TINT_BUILD_SPV_WRITER OFF)
  set(TINT_BUILD_WGSL_WRITER OFF)
  set(TINT_BUILD_NULL_WRITER OFF)

  FetchContent_Declare(
    Dawn
    GIT_REPOSITORY https://dawn.googlesource.com/dawn
    GIT_TAG 0bdfde1457030635c7295d7ad6ea1440756564ef
    GIT_SUBMODULES ""
    GIT_PROGRESS TRUE)
  FetchContent_MakeAvailable(dawn)
else()
  message(STATUS "Find Dawn: found")
endif()
