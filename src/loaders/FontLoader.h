#ifndef FONTLOADER_H
#define FONTLOADER_H

#include "../gfx/Font.h"
#include "Loader.h"

class FontLoader : public Loader
{
  public:
    gfx::Font getFont();

  protected:
    void command(const std::string& cmd, const std::vector<std::string>& args) override;
    void fileLoaded() override;

  private:
    gfx::Font m_font;
};

#endif // FONTLOADER_H
