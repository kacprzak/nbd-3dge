cmake_minimum_required(VERSION 3.12...3.14)
project(nbd-3dge VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DGLEW_NO_GLU -DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)

include(FetchContent)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(LINK_LIBS
  CONAN_PKG::sdl2
  CONAN_PKG::bullet
  CONAN_PKG::glew
  CONAN_PKG::spdlog
  CONAN_PKG::boost_program_options
  CONAN_PKG::boost_tokenizer
  CONAN_PKG::boost_algorithm
  CONAN_PKG::jsonformoderncpp
  CONAN_PKG::imgui)

if(WIN32)
  list(REMOVE_ITEM LINK_LIBS libboost_test_exec_monitor-vc140-mt-1_62 libboost_unit_test_framework-vc140-mt-1_62)
  set(LINK_LIBS ${LINK_LIBS} wsock32 ws2_32)
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(LINK_LIBS ${LINK_LIBS} c++abi c++fs)
  #set(LINK_LIBS ${LINK_LIBS} stdc++fs)
  set(LINK_LIBS ${LINK_LIBS} sndio)
  
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

  # For Emacs irony-mode to work
  #set(LIBCLANG_INCLUDES /usr/include/c++/8)
  #include_directories(${LIBCLANG_INCLUDES})
endif()

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
set(LINK_LIBS ${LINK_LIBS} glm)

add_library(gli INTERFACE)
target_include_directories(gli INTERFACE ${gli_SOURCE_DIR})
set(LINK_LIBS ${LINK_LIBS} gli)

set(EXTERNALS_DIR ${CMAKE_BINARY_DIR}/external)

file(DOWNLOAD "https://raw.githubusercontent.com/jessey-git/fx-gltf/master/include/fx/gltf.h" "${EXTERNALS_DIR}/fx/gltf.h")

add_library(fx-gltf INTERFACE)
target_include_directories(fx-gltf INTERFACE ${EXTERNALS_DIR})
set(LINK_LIBS ${LINK_LIBS} fx-gltf)

set(IMGUI_EXAMPLES_URL "https://raw.githubusercontent.com/ocornut/imgui/v1.66/examples")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_opengl3.h" "${EXTERNALS_DIR}/imgui/imgui_impl_opengl3.h")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_opengl3.cpp" "${EXTERNALS_DIR}/imgui/imgui_impl_opengl3.cpp")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_sdl.h" "${EXTERNALS_DIR}/imgui/imgui_impl_sdl.h")
file(DOWNLOAD "${IMGUI_EXAMPLES_URL}/imgui_impl_sdl.cpp" "${EXTERNALS_DIR}/imgui/imgui_impl_sdl.cpp")

add_library(imgui_impl INTERFACE)
target_compile_definitions(imgui_impl INTERFACE IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(imgui_impl INTERFACE ${EXTERNALS_DIR})
target_sources(imgui_impl INTERFACE
  ${EXTERNALS_DIR}/imgui/imgui_impl_sdl.cpp
  ${EXTERNALS_DIR}/imgui/imgui_impl_opengl3.cpp
  )
set(LINK_LIBS ${LINK_LIBS} imgui_impl)

configure_file(
  ${PROJECT_SOURCE_DIR}/config.h.in
  ${PROJECT_BINARY_DIR}/config.h
  )

configure_file(
  ${PROJECT_SOURCE_DIR}/config.ini.in
  ${PROJECT_BINARY_DIR}/bin/config.ini
  )

add_subdirectory(src)

if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
  enable_testing()
  add_subdirectory(tests)
endif()

