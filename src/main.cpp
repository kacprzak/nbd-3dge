#include "Engine.h"
#include "GameClient.h"
#include "GameLogic.h"
#include "Logger.h"
#include "Settings.h"

#include <boost/program_options.hpp>

#ifdef _WIN32
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

    if (logLevel == "debug") level   = spdlog::level::debug;
    if (logLevel == "info") level    = spdlog::level::info;
    if (logLevel == "warning") level = spdlog::level::warn;
    if (logLevel == "error") level   = spdlog::level::err;
    if (logLevel == "fatal") level   = spdlog::level::critical;

    auto console = spdlog::stdout_color_mt("console");
    console->set_level(level);
    console->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
}

//------------------------------------------------------------------------------

bool loadSettings(Settings& s, int ac, char** av)
{
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help,h", "Produce help message")
        ("screenWidth", po::value<unsigned short>()->default_value(s.screenWidth), "Screen resolution")
        ("screenHeight", po::value<unsigned short>()->default_value(s.screenHeight), "Screen resolution")
        ("fullscreen", "Full screen mode")
        ("msaa", po::value<unsigned short>()->default_value(s.msaa), "Multisample anti-aliasing")
        ("dataFolder", po::value<std::string>(), "Path to textures, sounds etc.")
        ("shadersFolder", po::value<std::string>(), "Path to shaders code")
        ("logLevel", po::value<std::string>()->default_value(s.logLevel),
         "Can be trace, debug, info, warning, error or fatal")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);

    const char* configFile = "config.ini";
    std::ifstream ifs{configFile};
    if (ifs) {
        store(parse_config_file(ifs, desc), vm);
    } else {
        char* buffer;

        // Get the current working directory:
        if ((buffer = getcwd(NULL, 0)) == NULL)
            perror("getcwd error");
        else {
            LOG_WARNING("No {} in {}", configFile, buffer);
            free(buffer);
        }
    }

    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << '\n';
        return false;
    }

    s.screenWidth  = vm["screenWidth"].as<unsigned short>();
    s.screenHeight = vm["screenHeight"].as<unsigned short>();

    if (vm.count("fullscreen")) s.fullscreen = true;

    s.msaa = vm["msaa"].as<unsigned short>();

    s.dataFolder    = vm["dataFolder"].as<std::string>();
    s.shadersFolder = vm["shadersFolder"].as<std::string>();

    s.logLevel = vm["logLevel"].as<std::string>();

    return true;
}

//------------------------------------------------------------------------------

int main(int ac, char** av)
{
    Settings settings;
    if (!loadSettings(settings, ac, av)) return 1;

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
    }

    spdlog::drop_all();

    return 0;
}
