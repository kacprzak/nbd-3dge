#ifndef FONTLOADER_H
#define FONTLOADER_H

#include "Loader.h"

class FontLoader : public Loader
{
 protected:
    void command(const std::string& cmd, const std::vector<std::string>& args) override;

};

#endif // FONTLOADER_H
