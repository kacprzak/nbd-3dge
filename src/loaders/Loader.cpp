#include "Loader.h"

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

void Loader::load(const std::filesystem::path& file)
{
    using namespace std;

    ifstream f(file);

    if (f.is_open() == true) {
        load(f);
        f.close();
    } else {
        cerr << "Error: unable to open " << file.string() << endl;
    }

    fileLoaded();
}

void Loader::load(std::istream& stream)
{
    std::string tmp;

    while (stream.good()) {
        getline(stream, tmp);
        boost::algorithm::trim(tmp);
        parseLine(tmp);
    }
}

void Loader::parseLine(const std::string& line)
{
    using namespace boost;

    // Ignore lines
    if (line.empty() || algorithm::starts_with(line, "#")) return;

    typedef tokenizer<char_separator<char>> tokenizer;

    char_separator<char> sep(" ");
    tokenizer tokens(line, sep);

    tokenizer::iterator it = tokens.begin();
    const std::string cmd  = *it;
    ++it; // skip command;

    std::vector<std::string> args;
    for (; it != tokens.end(); ++it) {
        args.push_back(*it);
    }

    command(cmd, args);
}

void Loader::fileLoaded()
{
    // Do nothing by default
}
