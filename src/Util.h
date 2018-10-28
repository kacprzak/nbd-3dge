#ifndef UTIL_H
#define UTIL_H

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>

inline float to_float(const std::string& s) { return boost::lexical_cast<float>(s); }

inline int to_int(const std::string& s) { return boost::lexical_cast<int>(s); }

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);

#endif // UTIL_H
