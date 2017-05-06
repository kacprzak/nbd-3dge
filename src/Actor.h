#ifndef ACTOR_H
#define ACTOR_H

#include "Components.h"

#include <map>
#include <memory>
#include <string>

class GameLogic;

using ActorId = unsigned int;

/*!
 * \brief Game entity.
 *
 * Represents game objects like NPCs, powerups, bullets, static walls.
 */
class Actor final
{
    friend class ActorFactory;

    using ComponentsMap = std::map<ComponentId, std::shared_ptr<Component>>;

  public:
    Actor(ActorId id);
    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;
    ~Actor();

    ActorId id() const { return m_id; }

    void die();
    bool dead() const { return m_dead; }

#ifndef NDEBUG
    void setName(const std::string& name) { m_name = name; }
    const std::string& name() const { return m_name; }
#endif

    void addComponent(ComponentId id, const std::shared_ptr<Component>& c)
    {
        m_components.insert(std::make_pair(id, c));
    }

    template <class T>
    std::weak_ptr<T> getComponent(ComponentId id)
    {
        ComponentsMap::iterator found = m_components.find(id);
        if (found != m_components.end()) {
            // Cast to subclass
            std::shared_ptr<T> sub{std::static_pointer_cast<T>(found->second)};
            // Conver to weak_ptr
            return std::weak_ptr<T>{sub};
        } else {
            return std::weak_ptr<T>{};
        }
    }

  private:
    ActorId m_id;
    ComponentsMap m_components;
    bool m_dead; //!< Flag indicating that this actor should be deleted by
                 //! GameLogic
#ifndef NDEBUG
    std::string m_name; //!< Name used in debug
#endif
};

#endif // ACTOR_H
