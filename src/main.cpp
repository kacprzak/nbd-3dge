//#include "Game.h"

#include "Engine.h"
#include "GameLogic.h"
#include "Game.h"
#include "Settings.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

bool loadSettings(Settings& s, int ac, char** av)
{
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("screenWidth", po::value<unsigned short>()->default_value(s.screenWidth), "Screen resolution")
        ("screenHeight", po::value<unsigned short>()->default_value(s.screenHeight), "Screen resolution")
        ("fullscreen", "Full screen mode")
        ("dataFolder", po::value<std::string>(), "Path to textures, sounds etc.")
        ("shadersFolder", po::value<std::string>(), "Path to shaders code.")
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

    return true;
}

//------------------------------------------------------------------------------

int main(int ac, char** av)
{
    Settings settings;
    if(!loadSettings(settings, ac, av))
        return 1;
    
    Engine engine;
    GameLogic game;
    game.attachView(std::make_shared<Game>(settings));
    engine.mainLoop(&game);

    return 0;
}
