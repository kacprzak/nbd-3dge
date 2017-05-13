set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set( nbd-3dge_SRCS
    main.cpp
    FpsCounter.cpp
    RenderNode.cpp
    Mesh.cpp
    RenderSystem.cpp
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

add_executable( nbd-3dge ${nbd-3dge_SRCS} )
target_link_libraries( nbd-3dge ${LINK_LIBS} )
