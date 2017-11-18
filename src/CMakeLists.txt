set( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR} )

set( nbd-3dge_SRCS
    main.cpp
    Actor.cpp
    ActorFactory.cpp
    Camera.cpp
    Engine.cpp
    Font.cpp
    FontLoader.cpp
    FpsCounter.cpp
    Framebuffer.cpp
    GameClient.cpp
    GameLogic.cpp
    InputSystem.cpp
    Loader.cpp
    Logger.cpp
    Mesh.cpp
    MeshData.cpp
    MtlLoader.cpp
    ObjLoader.cpp
    PhysicsDebugDrawer.cpp
    PhysicsSystem.cpp
    RenderNode.cpp
    RenderSystem.cpp
    ResourcesMgr.cpp
    SDLWindow.cpp
    Script.cpp
    Shader.cpp
    ShaderProgram.cpp
    Skybox.cpp
    Terrain.cpp
    Text.cpp
    Texture.cpp
    Util.cpp
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
