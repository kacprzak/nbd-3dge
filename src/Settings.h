#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings
{
    std::string windowTitle = "nbd-edge";
    int screenWidth         = 1024;
    int screenHeight        = 768;
    bool fullscreen         = false;
    int msaa                = 0;
    std::string dataFolder;
    std::string shadersFolder;
#ifndef NDEBUG
    std::string logLevel = "debug";
#else
    std::string logLevel = "warning";
#endif
};

#endif // SETTINGS_H
