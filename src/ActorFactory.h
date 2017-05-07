#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include "Actor.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <memory>

class GameLogic;

class ActorFactory
{
  public:
    void registerPrototype(boost::property_tree::ptree::value_type& v);
    std::unique_ptr<Actor> create(boost::property_tree::ptree::value_type& v);

  private:
    unsigned int getNextId();

    std::map<std::string, std::unique_ptr<Actor>> m_prototypes;
};

#endif // ACTORFACTORY_H
