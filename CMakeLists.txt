cmake_minimum_required( VERSION 2.8 )
project( nbd-3dge )

set( VERSION_MAJOR 0 )
set( VERSION_MINOR 1 )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

if( MSVC )
  #add_definitions( "/SUBSYSTEM:WINDOWS" )
else()
  add_definitions( "-Wall -Wextra -std=c++14" )
endif()

include_directories(${PROJECT_BINARY_DIR} src)

if( WIN32 )
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
  
  include_directories( ${CONAN_INCLUDE_DIRS_BULLET3}/bullet )
  set( LINK_LIBS ${CONAN_LIBS} )
  list( REMOVE_ITEM LINK_LIBS libboost_test_exec_monitor-vc140-mt-1_62 libboost_unit_test_framework-vc140-mt-1_62)
  set( LINK_LIBS ${LINK_LIBS} wsock32 ws2_32 )
endif()

if( UNIX )
  set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH} )

  find_package( SDL2 REQUIRED )
  find_package( SDL2_image REQUIRED )
  find_package( OpenGL REQUIRED )
  find_package( Bullet REQUIRED )

  set( Boost_COMPONENTS program_options log )

  list( APPEND Boost_COMPONENTS unit_test_framework )

  find_package( Boost COMPONENTS ${Boost_COMPONENTS} REQUIRED )
  #find_package( Lua53 REQUIRED )

  # For irony-mode to work
  set( LIBCLANG_INCLUDES /usr/include/c++/6 )

  include_directories(
    ${LIBCLANG_INCLUDES}
    ${SDL2_INCLUDE_DIR}
    ${SDL2IMAGE_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${LUA53_INCLUDE_DIR}
    ${BULLET_INCLUDE_DIRS}
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
endif()

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

add_subdirectory( src )


