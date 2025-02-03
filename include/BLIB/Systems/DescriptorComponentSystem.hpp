#ifndef BLIB_SYSTEMS_GENERICDESCRIPTORCOMPONENTSYSTEM_HPP
#define BLIB_SYSTEMS_GENERICDESCRIPTORCOMPONENTSYSTEM_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Templated system for managing ECS components that sync into renderer descriptor sets.
 *        Components should derive from DescriptorComponentBase
 *
 * @tparam TCom The type of component to manage
 * @tparam TPayload The type of data stored in the descriptor set buffer
 * @ingroup Systems
 */
template<typename TCom, typename TPayload>
class DescriptorComponentSystem : public engine::System {
    static_assert(
        std::is_base_of_v<rc::rcom::priv::DescriptorComponentPayloadBase<TCom, TPayload>, TCom>,
        "Component must derive from DescriptorComponentBase");

public:
    /**
     * @brief Destroys the system
     */
    virtual ~DescriptorComponentSystem() = default;

    /**
     * @brief Refreshes dirty components and synchronizes them to scene buffers
     *
     * @param Unused
     * @param Unused
     * @param Unused
     * @param Unused
     * @param Unused
     */
    virtual void update(std::mutex&, float, float, float, float) override;

    /**
     * @brief Initializes the system
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) override;

private:
    ecs::ComponentPool<TCom>* pool;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename TP>
void DescriptorComponentSystem<T, TP>::update(std::mutex&, float, float, float, float) {
    pool->forEach([](ecs::Entity, T& component) {
        if (component.isDirty()) { component.refresh<TP>(); }
    });
}

template<typename T, typename TP>
void DescriptorComponentSystem<T, TP>::init(engine::Engine& engine) {
    pool = &engine.ecs().getAllComponents<T>();
}

} // namespace sys
} // namespace bl

#endif
