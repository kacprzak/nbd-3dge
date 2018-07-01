cmake_minimum_required( VERSION 3.5 )
project( nbd-3dge VERSION 0.1 LANGUAGES CXX C )

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

add_definitions( -DGLM_ENABLE_EXPERIMENTAL -DGLM_FORCE_RADIANS )

include( ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake )
conan_basic_setup( TARGETS )
set( LINK_LIBS CONAN_PKG::sdl2 CONAN_PKG::bullet CONAN_PKG::glew CONAN_PKG::boost_log CONAN_PKG::boost_program_options )

if( WIN32 )
  list( REMOVE_ITEM LINK_LIBS libboost_test_exec_monitor-vc140-mt-1_62 libboost_unit_test_framework-vc140-mt-1_62 )
  set( LINK_LIBS ${LINK_LIBS} wsock32 ws2_32 )
endif()

if( UNIX )
  set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH} )

  # For irony-mode to work
  set( LIBCLANG_INCLUDES /usr/include/c++/6 )
  include_directories( ${LIBCLANG_INCLUDES} )

  include_directories( ${CMAKE_SOURCE_DIR}/external/gli )
  include_directories( ${CMAKE_SOURCE_DIR}/external/gli/external )

  #set( LINK_LIBS ${LINK_LIBS} sndio )
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/config.h.in
  ${PROJECT_BINARY_DIR}/config.h
  )

configure_file(
  ${PROJECT_SOURCE_DIR}/config.ini.in
  ${PROJECT_BINARY_DIR}/bin/config.ini
  )

add_subdirectory( src )

if( Boost_UNIT_TEST_FRAMEWORK_FOUND )
  enable_testing()
  add_subdirectory( tests )
endif()

