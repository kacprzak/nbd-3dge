#include "Loader.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include "Util.h"

Loader::Loader()
{
}

Loader::~Loader()
{
}

void Loader::load(const std::string &filename)
{
    using namespace std;
    m_folder = extractDirectory(filename);

    string tmp;
    ifstream f(filename.c_str());

    if (f.is_open() == true) {
        while (f.good()) {
            getline(f, tmp);
            boost::algorithm::trim(tmp);
            parseLine(tmp);
        }
        f.close();
    } else {
        cerr << "Error: unable to open " << filename << endl;
    }

    fileLoaded();
}

void Loader::parseLine(const std::string &line)
{
    using namespace boost;

    // Ignore lines
    if (line.empty() || algorithm::starts_with(line, "#"))
        return;

    typedef tokenizer<char_separator<char> >
            tokenizer;

    char_separator<char> sep(" ");
    tokenizer tokens(line, sep);

    tokenizer::iterator it = tokens.begin();
    const std::string cmd = *it;
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
