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
  FetchContent_Declare(
    Dawn
    GIT_REPOSITORY https://dawn.googlesource.com/dawn
    GIT_SHALLOW TRUE
    GIT_TAG 0bdfde1457030635c7295d7ad6ea1440756564ef
    GIT_SUBMODULES ""
    GIT_PROGRESS TRUE)
  FetchContent_MakeAvailable(dawn)
else()
  message(STATUS "Find Dawn: found")
endif()
