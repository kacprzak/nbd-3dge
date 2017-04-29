cmake_minimum_required( VERSION 2.8 )
project( nbd-3dge )

set( VERSION_MAJOR 0 )
set( VERSION_MINOR 1 )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

if( MSVC )
  #add_definitions( "/W2" )
else()
  add_definitions( "-Wall -Wextra -g -std=c++14" )
endif()

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH} )

find_package( SDL2 REQUIRED )
find_package( SDL2_image REQUIRED )
find_package( OpenGL REQUIRED )

set( Boost_COMPONENTS program_options )
if( UNIX )
  list( APPEND Boost_COMPONENTS unit_test_framework )
endif()
find_package( Boost COMPONENTS ${Boost_COMPONENTS} REQUIRED )
#find_package( Lua53 REQUIRED )

include_directories(
  ${PROJECT_BINARY_DIR}
  src
  ${SDL2_INCLUDE_DIR}
  ${SDL2IMAGE_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${LUA53_INCLUDE_DIR}
)

if( Boost_UNIT_TEST_FRAMEWORK_FOUND )
#  enable_testing()
#  add_subdirectory( tests )
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/config.h.in
  ${PROJECT_BINARY_DIR}/config.h
)

configure_file(
  ${PROJECT_SOURCE_DIR}/config.ini.in
  ${PROJECT_BINARY_DIR}/config.ini
)

set( EXTRA_LIBS GL GLU GLEW )

add_subdirectory( src )


