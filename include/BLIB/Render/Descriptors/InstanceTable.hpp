#ifndef BLIB_RENDER_DESCRIPTORS_INSTANCETABLE_HPP
#define BLIB_RENDER_DESCRIPTORS_INSTANCETABLE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <vector>

namespace bl
{
namespace rc
{
class RenderTarget;
class Scene;

namespace ds
{
class DescriptorSetInstance;
class DescriptorSetFactory;

/**
 * @brief Table of descriptor set instances. Needed because instances are managed by scenes but
 *        descriptors are per render target
 *
 * @ingroup Renderer
 */
class InstanceTable {
public:
    /**
     * @brief Creates an empty instance table
     */
    InstanceTable();

    /**
     * @brief Initializes the table with the factory to use to allocate sets
     *
     * @param scene The scene that owns this table
     * @param factory The factory to use to allocate sets
     */
    void init(Scene* scene, DescriptorSetFactory* factory);

    /**
     * @brief Allocates a new descriptor set instance for an observer. It is up to the caller to
     *        ensure that all prior added objects get added for the new observer
     *
     * @param index The scene index of the observer
     * @param observer The observer being added
     */
    void addObserver(unsigned int index, RenderTarget& observer);

    /**
     * @brief Returns the descriptor set instance for the given observer index
     *
     * @param index The observer scene index
     * @return The descriptor set instance for the observer
     */
    DescriptorSetInstance* get(unsigned int index) const { return sets[index]; }

    /**
     * @brief Allocates a new object in all set instances
     *
     * @param entity The entity id of this object in the ECS
     * @param key The scene key of the new object
     * @return True if the object could be added, false otherwise
     */
    bool allocateObject(ecs::Entity entity, scene::Key key);

    /**
     * @brief Allocates an object in a single set instance
     *
     * @param instanceIndex The index of the instance to add the object to
     * @param entity The entity id of this object in the ECS
     * @param key The scene key of the new object
     * @return True if the object could be added, false otherwise
     */
    bool allocateObject(unsigned int instanceIndex, ecs::Entity entity, scene::Key key);

    /**
     * @brief Called by the scene when an object is destroyed
     *
     * @param entity The ECS id of the object being removed
     * @param key The scene key of the object being removed
     */
    void releaseObject(ecs::Entity entity, scene::Key key);

private:
    DescriptorSetFactory* factory;
    Scene* scene;
    std::vector<DescriptorSetInstance*> sets;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
