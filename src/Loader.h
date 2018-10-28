#ifndef LOADER_H
#define LOADER_H

#include <istream>
#include <string>
#include <vector>
#include <filesystem>

/**
 * @brief Base class for Loaders
 *
 * Obj, Mtl, Map or any other file loader can inherit from this class.
 */
class Loader
{
  public:
    Loader() = default;
    virtual ~Loader() = default;

    void load(const std::filesystem::path& file);
    void load(std::istream& stream);

  protected:
    virtual void command(const std::string& cmd, const std::vector<std::string>& args) = 0;
    virtual void fileLoaded();
    virtual void parseLine(const std::string& line);
};

#endif // LOADER_H
