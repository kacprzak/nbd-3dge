set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set( nbd-3dge_SRCS
    main.cpp
    FpsCounter.cpp
    Actor.cpp
    Mesh.cpp
    GameObjectManager.cpp
    GameCore.cpp
    Game.cpp
    Util.cpp
    Texture.cpp
    Loader.cpp
    ObjLoader.cpp
    MtlLoader.cpp
    FontLoader.cpp
    Font.cpp
    Skybox.cpp
    Shader.cpp
    ShaderProgram.cpp
    Camera.cpp
    Terrain.cpp
    Script.cpp
)

# Add winsock lib
if( WIN32 )
  set( LINK_LIBS ${LINK_LIBS} wsock32 ws2_32 )
endif( WIN32 )

add_executable( nbd-3dge ${nbd-3dge_SRCS} )
target_link_libraries( nbd-3dge ${LINK_LIBS} )
