include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

function(download_if_not_exists url file)
  if(NOT EXISTS ${file})
    message(STATUS "Downloading: " ${file})
    file(DOWNLOAD ${url} ${file})
  endif()
endfunction()

if(NOT TARGET CONAN_PKG::CLI11)
  FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
    LOG_DOWNLOAD   ON
    )

  FetchContent_GetProperties(cli11)
  if(NOT cli11_POPULATED)
    FetchContent_Populate(cli11)
    add_subdirectory(${cli11_SOURCE_DIR} ${cli11_BINARY_DIR})
  endif()
endif()

FetchContent_Declare(
  gli
  GIT_REPOSITORY https://github.com/g-truc/gli.git
  GIT_SHALLOW    ON
  GIT_PROGRESS   ON
  BUILD_COMMAND  ""
  LOG_DOWNLOAD   ON
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

download_if_not_exists("https://raw.githubusercontent.com/jessey-git/fx-gltf/master/include/fx/gltf.h"
  "${CMAKE_CURRENT_BINARY_DIR}/fx/gltf.h")

add_library(fx-gltf INTERFACE)
target_include_directories(fx-gltf INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
add_library(external::fx-gltf ALIAS fx-gltf)

if(TARGET CONAN_PKG::imgui)
  set(IMGUI_EXAMPLES_URL "https://raw.githubusercontent.com/ocornut/imgui/v1.66/examples")
  set(imgui_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/imgui")
  download_if_not_exists("${IMGUI_EXAMPLES_URL}/imgui_impl_opengl3.h" "${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.h")
  download_if_not_exists("${IMGUI_EXAMPLES_URL}/imgui_impl_opengl3.cpp" "${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp")
  download_if_not_exists("${IMGUI_EXAMPLES_URL}/imgui_impl_sdl.h" "${imgui_SOURCE_DIR}/examples/imgui_impl_sdl.h")
  download_if_not_exists("${IMGUI_EXAMPLES_URL}/imgui_impl_sdl.cpp" "${imgui_SOURCE_DIR}/examples/imgui_impl_sdl.cpp")
else()
  FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        v1.66
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
    BUILD_COMMAND  ""
    LOG_DOWNLOAD   ON
    )

  if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
  endif()

  add_library(imgui INTERFACE)
  target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR})
  target_sources(imgui INTERFACE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    )
endif()

add_library(imgui_impl INTERFACE)
target_compile_definitions(imgui_impl INTERFACE IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(imgui_impl INTERFACE ${imgui_SOURCE_DIR})
target_sources(imgui_impl INTERFACE
  ${imgui_SOURCE_DIR}/examples/imgui_impl_sdl.cpp
  ${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3.cpp
  )

if(TARGET imgui)
  target_link_libraries(imgui_impl INTERFACE imgui)
endif()

add_library(external::imgui_impl ALIAS imgui_impl)
