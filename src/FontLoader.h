#ifndef FONTLOADER_H
#define FONTLOADER_H

#include "Loader.h"
#include "Font.h"

class FontLoader : public Loader
{
 public:
    FontLoader();
    
    Font* getFont();
    
 protected:
    void command(const std::string& cmd, const std::vector<std::string>& args) override;
    void fileLoaded() override;
    
 private:
    Font* m_font;
};

#endif // FONTLOADER_H
