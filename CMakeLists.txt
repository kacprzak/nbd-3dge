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
find_package( Bullet REQUIRED )

set( Boost_COMPONENTS program_options log )
if( UNIX )
  list( APPEND Boost_COMPONENTS unit_test_framework )
endif()
find_package( Boost COMPONENTS ${Boost_COMPONENTS} REQUIRED )
#find_package( Lua53 REQUIRED )

if( UNIX )
  # For irony-mode to work
  set( LIBCLANG_INCLUDES /usr/include/c++/6 )
endif()

include_directories(
  ${LIBCLANG_INCLUDES}
  ${PROJECT_BINARY_DIR}
  src
  ${SDL2_INCLUDE_DIR}
  ${SDL2IMAGE_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${LUA53_INCLUDE_DIR}
  ${BULLET_INCLUDE_DIRS}
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

set( LINK_LIBS
  GL GLU GLEW
  ${SDL2_LIBRARY}
  ${SDL2MAIN_LIBRARY}
  ${SDL2IMAGE_LIBRARY}
  ${OPENGL_LIBRARIES}
  ${LUA53_LIBRARIES}
  ${Boost_LIBRARIES}
  ${BULLET_LIBRARIES}
)

# Add winsock lib
if( WIN32 )
  set( LINK_LIBS ${LINK_LIBS} wsock32 ws2_32 )
endif( WIN32 )

add_subdirectory( src )


