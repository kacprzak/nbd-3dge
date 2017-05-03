#include "Engine.h"
#include "GameLogic.h"
#include "GameClient.h"
#include "Settings.h"
#include "Logger.h"

#include <boost/program_options.hpp>
#include <boost/log/expressions.hpp>
#include <iostream>
#include <fstream>

void initLogger(const std::string& logLevel)
{
    using namespace boost;

    auto level = log::trivial::trace;
    if (logLevel == "debug") level = log::trivial::debug;
    if (logLevel == "info") level = log::trivial::info;
    if (logLevel == "warning") level = log::trivial::warning;
    if (logLevel == "error") level = log::trivial::error;
    if (logLevel == "fatal") level = log::trivial::fatal;
    
    log::core::get()->set_filter(log::trivial::severity >= level);
}

//------------------------------------------------------------------------------

bool loadSettings(Settings& s, int ac, char** av)
{
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Produce help message")
        ("screenWidth", po::value<unsigned short>()->default_value(s.screenWidth), "Screen resolution")
        ("screenHeight", po::value<unsigned short>()->default_value(s.screenHeight), "Screen resolution")
        ("fullscreen", "Full screen mode")
        ("dataFolder", po::value<std::string>(), "Path to textures, sounds etc.")
        ("shadersFolder", po::value<std::string>(), "Path to shaders code")
        ("logLevel", po::value<std::string>()->default_value(s.logLevel),
         "Can be trace, debug, info, warning, error or fatal")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    std::ifstream ifs{"config.ini"};
    if (ifs)
        store(parse_config_file(ifs, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << '\n';
        return false;
    }

    s.screenWidth = vm["screenWidth"].as<unsigned short>();
    s.screenHeight = vm["screenHeight"].as<unsigned short>();
    if (vm.count("fullscreen"))
        s.fullscreen = true;

    s.dataFolder = vm["dataFolder"].as<std::string>();
    s.shadersFolder = vm["shadersFolder"].as<std::string>();

    s.logLevel = vm["logLevel"].as<std::string>();

    return true;
}

//------------------------------------------------------------------------------

int main(int ac, char** av)
{
    Settings settings;
    if(!loadSettings(settings, ac, av))
        return 1;

    initLogger(settings.logLevel);
    
    Engine engine;
    GameLogic game(settings);
    game.attachView(std::make_shared<GameClient>(settings));
    engine.mainLoop(&game);

    return 0;
}
