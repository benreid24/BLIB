#ifndef BLIB_RENDER_DESCRIPTORS_INSTANCETABLE_HPP
#define BLIB_RENDER_DESCRIPTORS_INSTANCETABLE_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <array>
#include <vector>

namespace bl
{
namespace rc
{
class RenderTarget;
class Scene;

namespace vk
{
class PipelineLayout;
}
namespace scene
{
class TargetTable;
struct SceneObject;
} // namespace scene

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
    using LayoutSets = std::array<DescriptorSetInstance*, cfg::Limits::MaxDescriptorSets>;

    /**
     * @brief Creates an empty instance table
     */
    InstanceTable();

    /**
     * @brief Initializes the table with the layout to create sets with
     *
     * @param scene The scene that owns this table
     * @param layout The pipeline layout to get the set factories from
     */
    void init(Scene* scene, const vk::PipelineLayout& layout);

    /**
     * @brief Reinitializes the table of descriptor sets for the new layout for the object that uses
     *        this table. Intended for use when the table is for a single object
     *
     * @param newLayout The new pipeline layout
     * @param observers The table of observers that use this instance table
     * @param object The object that uses this instance table
     */
    void reinit(const vk::PipelineLayout& newLayout, scene::TargetTable& observers,
                const scene::SceneObject& object);

    /**
     * @brief Allocates new descriptor set instances for an observer. It is up to the caller to
     *        ensure that all prior added objects get added for the new observer
     *
     * @param index The scene index of the observer
     * @param observer The observer being added
     */
    void addObserver(unsigned int index, RenderTarget& observer);

    /**
     * @brief Adds all observers of a scene
     *
     * @param observers The table of observers to add
     */
    void addObservers(const scene::TargetTable& observers);

    /**
     * @brief Sets all descriptor sets to nullptr for the given observer index
     *
     * @param index The index of the observer to clear out
     */
    void removeObserver(unsigned int index);

    /**
     * @brief Returns the descriptor set instance for the given observer index
     *
     * @param index The observer scene index
     * @return The descriptor set instances for the observer
     */
    LayoutSets& get(unsigned int index) { return sets[index]; }

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

    /**
     * @brief Returns whether the sets for this layout are bindless
     */
    bool isBindless() const { return bindless; }

    /**
     * @brief Returns the index of the first per-object descriptor set
     */
    unsigned int getPerObjectStart() const { return perObjStart; }

    /**
     * @brief Returns the number of descriptor sets in the layout
     */
    unsigned int getDescriptorSetCount() const { return factories.size(); }

private:
    ctr::StaticVector<DescriptorSetFactory*, cfg::Limits::MaxDescriptorSets> factories;
    Scene* scene;
    std::vector<LayoutSets> sets;
    bool checkedBindless;
    bool bindless;
    unsigned int perObjStart;

    void checkBindless();
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
