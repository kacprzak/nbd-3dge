#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings
{
    std::string windowTitle = "nbd-edge";
    int screenWidth = 1024;
    int screenHeight = 768;
    bool fullscreen = false;
    std::string dataFolder;
    std::string shadersFolder;
};

#endif // SETTINGS_H
