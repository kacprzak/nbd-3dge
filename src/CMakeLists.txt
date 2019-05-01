set(nbd-3dge_SRCS
    gfx/Buffer.cpp
    gfx/Camera.cpp
    gfx/Font.cpp
    gfx/Framebuffer.cpp
    gfx/Light.cpp
    gfx/Material.cpp
    gfx/Mesh.cpp
    gfx/Node.cpp
    gfx/Model.cpp
    gfx/Shader.cpp
    gfx/ShaderProgram.cpp
    gfx/Skybox.cpp
    gfx/Texture.cpp
    gfx/Text.cpp
    Actor.cpp
    ActorFactory.cpp
    Engine.cpp
    FontLoader.cpp
    GameClient.cpp
    GameLogic.cpp
    InputSystem.cpp
    Loader.cpp
    Logger.cpp
    MeshData.cpp
    MtlLoader.cpp
    ObjLoader.cpp
    PhysicsDebugDrawer.cpp
    PhysicsSystem.cpp
    RenderSystem.cpp
    ResourcesMgr.cpp
    SDLWindow.cpp
    Script.cpp
    Terrain.cpp
    Util.cpp
    main.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Sources" FILES ${nbd-3dge_SRCS})

file(GLOB_RECURSE nbd-3dge_HDRS "*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Headers" FILES ${nbd-3dge_HDRS})

add_executable(nbd-3dge WIN32 ${nbd-3dge_HDRS} ${nbd-3dge_SRCS})

if( MSVC )
  target_compile_options(nbd-3dge PRIVATE /W3)
else()
  target_compile_options(nbd-3dge PRIVATE -Wall -Wextra)
endif()
target_include_directories(nbd-3dge PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(nbd-3dge PRIVATE ${LINK_LIBS})
