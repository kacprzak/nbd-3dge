#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

std::string extractDirectory(const std::string& path);
std::string extractFilename(const std::string& path);
std::string changeExtension(const std::string& path, const std::string& ext);

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);

#endif // UTIL_H
