cmake_minimum_required( VERSION 3.5 FATAL_ERROR )
project( nbd-3dge VERSION 0.1 LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

if( MSVC )
  #set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows" )
else()
  add_definitions( "-Wall -Wextra" )
endif()

if( WIN32 )
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
  
  include_directories( ${CONAN_INCLUDE_DIRS_BULLET3}/bullet )
  set( LINK_LIBS ${CONAN_LIBS} )
  list( REMOVE_ITEM LINK_LIBS libboost_test_exec_monitor-vc140-mt-1_62 libboost_unit_test_framework-vc140-mt-1_62 )
  set( LINK_LIBS ${LINK_LIBS} wsock32 ws2_32 )
endif()

if( UNIX )
  set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH} )

  find_package( OpenGL REQUIRED )
  find_package( GLEW REQUIRED )
  find_package( GLM REQUIRED )
  find_package( SDL2 REQUIRED )
  find_package( SDL2_image REQUIRED )
  find_package( Bullet REQUIRED )
  #find_package( Lua53 REQUIRED )

  set( Boost_COMPONENTS program_options log )
  list( APPEND Boost_COMPONENTS unit_test_framework )

  find_package( Boost 1.61 REQUIRED COMPONENTS ${Boost_COMPONENTS} )

  add_library( OpenGL::gl UNKNOWN IMPORTED )
  set_property( TARGET OpenGL::gl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENGL_INCLUDE_DIR} )
  set_property( TARGET OpenGL::gl PROPERTY IMPORTED_LOCATION ${OPENGL_gl_LIBRARY} )

  add_library( OpenGL::glew UNKNOWN IMPORTED )
  set_property( TARGET OpenGL::glew PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIR} )
  set_property( TARGET OpenGL::glew PROPERTY IMPORTED_LOCATION ${GLEW_LIBRARIES} )

  add_library( OpenGL::glm INTERFACE IMPORTED )
  set_property( TARGET OpenGL::glm PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLM_INCLUDE_DIR} )
  
  add_library( SDL::sdl UNKNOWN IMPORTED )
  set_property( TARGET SDL::sdl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR} )
  set_property( TARGET SDL::sdl PROPERTY IMPORTED_LOCATION ${SDL2_LIBRARIES} )

  # add_library( SDL::main STATIC IMPORTED )
  # set_property( TARGET SDL::main PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR} )
  # set_property( TARGET SDL::main PROPERTY IMPORTED_LOCATION "${SDL2MAIN_LIBRARY}")
  
  add_library( SDL::image UNKNOWN IMPORTED )
  set_property( TARGET SDL::image PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SDL2IMAGE_INCLUDE_DIR} )
  set_property( TARGET SDL::image PROPERTY IMPORTED_LOCATION ${SDL2IMAGE_LIBRARY} )

  add_library( Bullet::math UNKNOWN IMPORTED )
  set_property( TARGET Bullet::math PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BULLET_INCLUDE_DIR} )
  set_property( TARGET Bullet::math PROPERTY IMPORTED_LOCATION ${BULLET_MATH_LIBRARY} )

  add_library( Bullet::dynamics UNKNOWN IMPORTED )
  set_property( TARGET Bullet::dynamics PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BULLET_INCLUDE_DIR} )
  set_property( TARGET Bullet::dynamics PROPERTY IMPORTED_LOCATION ${BULLET_DYNAMICS_LIBRARY} )

  add_library( Bullet::collision UNKNOWN IMPORTED )
  set_property( TARGET Bullet::collision PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BULLET_INCLUDE_DIR} )
  set_property( TARGET Bullet::collision PROPERTY IMPORTED_LOCATION ${BULLET_COLLISION_LIBRARY} )

  add_library( Lua::lua UNKNOWN IMPORTED )
  set_property( TARGET Lua::lua PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${LUA53_INCLUDE_DIR} )
  set_property( TARGET Lua::lua PROPERTY IMPORTED_LOCATION ${LUA53_LIBRARY} )

  # For irony-mode to work
  set( LIBCLANG_INCLUDES /usr/include/c++/6 )
  include_directories( ${LIBCLANG_INCLUDES} )

  set( LINK_LIBS
    OpenGL::gl
    OpenGL::glew
    OpenGL::glm
    SDL::sdl
    SDL::image
    #Lua::lua
    Boost::boost
    Boost::log
    Boost::program_options
    Bullet::math
    Bullet::dynamics
    Bullet::collision
    )
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

if( Boost_UNIT_TEST_FRAMEWORK_FOUND )
  enable_testing()
  add_subdirectory( tests )
endif()

