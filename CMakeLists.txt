cmake_minimum_required( VERSION 2.8 )
project( nbd-3dge )

if( MSVC )
  #add_definitions( "" )
else()
  add_definitions( "-Wall -Wextra -g -std=c++14" )
endif()

set( nbd-3dge_SRCS
    src/main.cpp
    src/FpsCounter.cpp
    src/Actor.cpp
    src/Mesh.cpp
    src/GameObjectManager.cpp
    src/GameCore.cpp
    src/Game.cpp
    src/Util.cpp
    src/Texture.cpp
    src/Loader.cpp
    src/ObjLoader.cpp
    src/MtlLoader.cpp
    src/Skybox.cpp
    src/Shader.cpp
    src/ShaderProgram.cpp
    src/Camera.cpp
    src/Terrain.cpp
)

add_executable( nbd-3dge ${nbd-3dge_SRCS} )

configure_file( ${CMAKE_SOURCE_DIR}/config.xml ${CMAKE_BINARY_DIR}/config.xml COPYONLY )

# Detect SFML
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH} )
find_package( SFML 2 COMPONENTS system window graphics audio REQUIRED )
if( SFML_FOUND )
    include_directories( ${SFML_INCLUDE_DIR} )
    target_link_libraries( nbd-3dge ${SFML_LIBRARIES} GL GLU GLEW )
endif()


