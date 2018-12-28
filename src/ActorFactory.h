#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include "Actor.h"

#include <nlohmann/json_fwd.hpp>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/xml_parser.hpp>

#include <memory>

class GameLogic;

class ActorFactory
{
  public:
    void registerPrototype(const nlohmann::json& node);
    std::unique_ptr<Actor> create(const nlohmann::json& mode);

  private:
    unsigned int getNextId();

    std::map<std::string, std::unique_ptr<Actor>> m_prototypes;
};

#endif // ACTORFACTORY_H
