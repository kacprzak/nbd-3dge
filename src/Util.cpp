#include "Util.h"
#include <sstream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    static const std::string slash="\\";
#else
    static const std::string slash="/";
#endif


std::string extractDirectory(const std::string& path)
{
    return path.substr(0, path.find_last_of(slash) + 1);
}

std::string extractFilename(const std::string& path)
{
    return path.substr(path.find_last_of(slash) + 1);
}

std::string changeExtension(const std::string& path, const std::string& ext)
{
    std::string filename = extractFilename(path);
    return extractDirectory(path) + filename.substr(0, filename.find_last_of('.')) + ext;
}

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
