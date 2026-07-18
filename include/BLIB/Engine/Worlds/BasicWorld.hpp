#ifndef BLIB_ENGINE_WORLDS_BASICWORLD_HPP
#define BLIB_ENGINE_WORLDS_BASICWORLD_HPP

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/World.hpp>

namespace bl
{
namespace engine
{
/**
 * @brief Basic world that uses a specific scene type
 *
 * @tparam TScene The type of scene for the world
 * @ingroup Engine
 */
template<typename TScene>
class BasicWorld : public World {
public:
    /**
     * @brief Creates the world
     *
     * @param owner The game engine instance
     */
    BasicWorld(Engine& owner);

    /**
     * @brief Destroys the world
     */
    virtual ~BasicWorld() = default;

    /**
     * @brief Returns the scene for this world cast to the correct type
     */
    TScene& typedScene() { return *static_cast<TScene*>(scene().get()); }

    /**
     * @brief Returns the scene for this world cast to the correct type
     */
    const TScene* typedScene() const { return static_cast<const TScene*>(scene().get()); }
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TScene>
BasicWorld<TScene>::BasicWorld(Engine& owner)
: World(owner, owner.renderer().scenePool().allocateScene<TScene>()) {}

} // namespace engine
} // namespace bl

#endif
