#include "Engine.h"
#include "GameClient.h"
#include "GameLogic.h"
#include "Logger.h"
#include "Settings.h"
#include "config.h"

#include <CLI/CLI.hpp>

#if PLATFORM == PLATFORM_WINDOWS
#include <direct.h>
#define getcwd _getcwd
#define chdir _chrdir
#else
#include <unistd.h>
#endif

#include <fstream>
#include <iostream>

void initLogger(const std::string& logLevel)
{
    using namespace boost;

    auto level = spdlog::level::trace;

    if (logLevel == "debug") level = spdlog::level::debug;
    if (logLevel == "info") level = spdlog::level::info;
    if (logLevel == "warning") level = spdlog::level::warn;
    if (logLevel == "error") level = spdlog::level::err;
    if (logLevel == "fatal") level = spdlog::level::critical;

#ifdef _MSC_VER
    auto console = spdlog::create<spdlog::sinks::msvc_sink_mt>("console");
#else
    auto console = spdlog::stdout_color_mt("console");
#endif
    console->set_level(level);
    console->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
}

//------------------------------------------------------------------------------

void setupCli(CLI::App& app, Settings& s)
{
    app.set_config("--config", "config.ini");
    app.add_option("--screenWidth", s.screenWidth, "Screen resolution", true);
    app.add_option("--screenHeight", s.screenHeight, "Screen resolution", true);
    app.add_flag("--fullscreen", s.fullscreen, "Full screen mode");
    app.add_option("--msaa", s.msaa, "Multisample anti-aliasing", true);
    app.add_option("--dataFolder", s.dataFolder, "Path to textures, sounds etc.")
        ->check(CLI::ExistingDirectory)
        ->required();
    app.add_option("--shadersFolder", s.shadersFolder, "Path to shaders code")
        ->check(CLI::ExistingDirectory)
        ->required();
    app.add_set("--logLevel", s.logLevel, {"trace", "debug", "info", "warning", "error", "fatal"});
}

//------------------------------------------------------------------------------

int main(int ac, char** av)
{
    CLI::App app{"No Big Deal 3D Game Engine v" XSTR(VERSION_MAJOR) "." XSTR(VERSION_MINOR)};
    Settings settings;
    setupCli(app, settings);
    CLI11_PARSE(app, ac, av);

    initLogger(settings.logLevel);

    try {
        Engine engine;
        auto resourcesMgr =
            std::make_shared<ResourcesMgr>(settings.dataFolder, settings.shadersFolder);

        GameLogic game(settings, resourcesMgr);
        game.attachView(std::make_shared<GameClient>(settings, resourcesMgr));
        engine.mainLoop(&game);
    } catch (const std::exception& e) {
        LOG_FATAL(e.what());
        spdlog::drop_all();
    }

    spdlog::drop_all();

    return 0;
}
