#ifndef ACTORFACTORY_H
#define ACTORFACTORY_H

#include "Actor.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

class GameLogic;

class ActorFactory
{
  public:
    static std::unique_ptr<Actor> create(boost::property_tree::ptree::value_type& v);

  private:
    static unsigned int getNextId();
};

#endif // ACTORFACTORY_H
