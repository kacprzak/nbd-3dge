include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare(
  gli
  GIT_REPOSITORY https://github.com/g-truc/gli.git
  GIT_SHALLOW    ON
  )

FetchContent_GetProperties(gli)
if(NOT gli_POPULATED)
  FetchContent_Populate(gli)
  #add_subdirectory(${gli_SOURCE_DIR} ${gli_BINARY_DIR})
endif()

add_library(glm INTERFACE)
target_compile_definitions(glm INTERFACE GLM_FORCE_RADIANS)
target_include_directories(glm INTERFACE ${gli_SOURCE_DIR}/external)
add_library(external::glm ALIAS glm)

add_library(gli INTERFACE)
target_include_directories(gli INTERFACE ${gli_SOURCE_DIR})
target_link_libraries(gli INTERFACE glm)
add_library(external::gli ALIAS gli)

file(DOWNLOAD "https://raw.githubusercontent.com/jessey-git/fx-gltf/master/include/fx/gltf.h"
  "${CMAKE_CURRENT_BINARY_DIR}/fx/gltf.h")

add_library(fx-gltf INTERFACE)
target_include_directories(fx-gltf INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
add_library(external::fx-gltf ALIAS fx-gltf)

set(IMGUI_EXAMPLES_URL "https://raw.githubusercontent.com/ocornut/imgui/v1.66/examples")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_opengl3.h" "${CMAKE_CURRENT_BINARY_DIR}/imgui/imgui_impl_opengl3.h")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_opengl3.cpp" "${CMAKE_CURRENT_BINARY_DIR}/imgui/imgui_impl_opengl3.cpp")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_sdl.h" "${CMAKE_CURRENT_BINARY_DIR}/imgui/imgui_impl_sdl.h")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_sdl.cpp" "${CMAKE_CURRENT_BINARY_DIR}/imgui/imgui_impl_sdl.cpp")

add_library(imgui_impl INTERFACE)
target_compile_definitions(imgui_impl INTERFACE IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(imgui_impl INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
target_sources(imgui_impl INTERFACE
  ${CMAKE_CURRENT_BINARY_DIR}/imgui/imgui_impl_sdl.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/imgui/imgui_impl_opengl3.cpp
  )
add_library(external::imgui_impl ALIAS imgui_impl)
