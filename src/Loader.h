#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <vector>

/**
 * @brief Base class for Loaders
 *
 * Obj, Mtl, Map or any other file loader can inherit from this class.
 */
class Loader
{
public:
    Loader();
    virtual ~Loader();

    void load(const std::string& filename);

protected:
    virtual void command(const std::string& cmd, const std::vector<std::string>& args) = 0;
    virtual void fileLoaded();
    virtual void parseLine(const std::string& line);

    const std::string& currentFolder() { return m_folder; }

private:
    std::string m_folder;
};

#endif // LOADER_H
