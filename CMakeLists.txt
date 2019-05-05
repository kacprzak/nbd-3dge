cmake_minimum_required(VERSION 3.12...3.14)
project(nbd-3dge VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
  target_compile_definitions(CONAN_PKG::glew INTERFACE GLEW_NO_GLU)
  set(nbd-3dge_DEPS
    CONAN_PKG::sdl2
    CONAN_PKG::bullet
    CONAN_PKG::glew
    CONAN_PKG::spdlog
    CONAN_PKG::boost_program_options
    CONAN_PKG::boost_tokenizer
    CONAN_PKG::boost_algorithm
    CONAN_PKG::jsonformoderncpp
    CONAN_PKG::imgui)
else()
  find_package(SDL2 REQUIRED CONFIG)
  
  add_library(sdl2 INTERFACE)
  target_include_directories(sdl2 INTERFACE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(sdl2 INTERFACE ${SDL2_LIBRARIES})
  
  find_package(spdlog REQUIRED)
  #find_package(nlohmann_json REQUIRED)
  find_package(Bullet REQUIRED CONFIG)

  add_library(Bullet INTERFACE)
  target_include_directories(Bullet INTERFACE ${BULLET_ROOT_DIR}/${BULLET_INCLUDE_DIRS})
  list(REMOVE_ITEM BULLET_LIBRARIES BulletInverseDynamics)
  target_link_libraries(Bullet INTERFACE ${BULLET_LIBRARIES})

  find_package(Boost COMPONENTS program_options)
  find_package(OpenGL)
  find_package(GLEW)
  find_package(fmt)
  
  set(nbd-3dge_DEPS
    sdl2
    Bullet
    spdlog::spdlog
    fmt::fmt
    Boost::program_options
    OpenGL::GL
    GLEW::GLEW
    )
endif()

if(WIN32)
  list(REMOVE_ITEM nbd-3dge_DEPS libboost_test_exec_monitor-vc140-mt-1_62 libboost_unit_test_framework-vc140-mt-1_62)
  set(nbd-3dge_DEPS ${nbd-3dge_DEPS} wsock32 ws2_32)
endif()

if(UNIX)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(nbd-3dge_DEPS ${nbd-3dge_DEPS} c++abi c++fs)
    set(nbd-3dge_DEPS ${nbd-3dge_DEPS} sndio)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(nbd-3dge_DEPS ${nbd-3dge_DEPS} stdc++fs)
  endif()
  # For Emacs irony-mode to work
  #set(LIBCLANG_INCLUDES /usr/include/c++/8)
  #include_directories(${LIBCLANG_INCLUDES})
endif()

add_subdirectory(external)
set(nbd-3dge_DEPS ${nbd-3dge_DEPS}
  external::gli external::glm external::fx-gltf external::imgui_impl)
add_subdirectory(src)

configure_file(
  ${PROJECT_SOURCE_DIR}/config.h.in
  ${PROJECT_BINARY_DIR}/config.h
  )

configure_file(
  ${PROJECT_SOURCE_DIR}/config.ini.in
  ${PROJECT_BINARY_DIR}/bin/config.ini
  )

if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
  enable_testing()
  add_subdirectory(tests)
endif()

