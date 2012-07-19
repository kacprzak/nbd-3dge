cmake_minimum_required( VERSION 2.8 )
project( nbd-3dge )

if( MSVC )
  #add_definitions( "" )
else()
  add_definitions( "-Wall -Wextra -g -std=gnu++0x" )
endif()

set( nbd-3dge_SRCS
    src/main.cpp
    src/fpscounter.cpp
    src/actor.cpp
    src/mesh.cpp
    src/gameobjectmanager.cpp
    src/game.cpp
    src/gametest1.cpp
    src/util.cpp
    src/texture.cpp

    src/objloader.h
    src/vector.h
)

add_executable( nbd-3dge ${nbd-3dge_SRCS} )

file( COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR}/ )

# Detect SFML
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package( SFML 2.0 REQUIRED system window graphics network audio )
if( SFML_FOUND )
    include_directories( ${SFML_INCLUDE_DIR} )
    target_link_libraries( nbd-3dge ${SFML_LIBRARIES} GL GLU GLEW )
endif()


