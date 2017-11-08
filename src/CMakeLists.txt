set( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR} )

set( nbd-3dge_SRCS
    main.cpp
    FpsCounter.cpp
    RenderNode.cpp
    Mesh.cpp
    MeshData.cpp
    RenderSystem.cpp
    InputSystem.cpp
    SDLWindow.cpp
    GameClient.cpp
    GameLogic.cpp
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
    Engine.cpp
    Logger.cpp
    Text.cpp
    ResourcesMgr.cpp
    Actor.cpp
    ActorFactory.cpp
    PhysicsSystem.cpp
    PhysicsDebugDrawer.cpp
)

file( GLOB_RECURSE nbd-3dge_HDRS "*.h" )

source_group( "Headers" FILES ${nbd-3dge_HDRS} )

add_executable( nbd-3dge WIN32 ${nbd-3dge_HDRS} ${nbd-3dge_SRCS} )
if( MSVC )
  target_compile_options( nbd-3dge PRIVATE /W3 )
else()
  target_compile_options( nbd-3dge PRIVATE -Wall -Wextra )
endif()
target_include_directories( nbd-3dge PRIVATE ${PROJECT_BINARY_DIR} )
target_link_libraries( nbd-3dge PRIVATE ${LINK_LIBS} )
