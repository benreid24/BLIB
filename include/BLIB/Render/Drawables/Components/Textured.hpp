#ifndef BLIB_RENDER_DRAWABLES_COMPONENTS_TEXTURED_HPP
#define BLIB_RENDER_DRAWABLES_COMPONENTS_TEXTURED_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Render/Components/Texture.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
/**
 * @brief Base component class for textured Drawables
 *
 * @ingroup Renderer
 */
class Textured {
public:
    /**
     * @brief Does nothing
     */
    Textured() = default;

    /**
     * @brief Changes the texture for this drawable
     *
     * @param texture The new texture to use
     */
    void setTexture(const res::TextureRef& texture);

    /**
     * @brief Returns the texture. Must only be called after create()
     */
    const res::TextureRef& getTexture() const;

protected:
    /**
     * @brief Creates the texture component in the ECS
     *
     * @tparam ...TArgs Argument types to the texture's constructor
     * @param registry The ECS registry instance
     * @param entity The ECS entity id
     * @param ...args Arguments to the texture's constructor
     */
    template<typename... TArgs>
    void create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args);

private:
    ecs::StableHandle<com::Texture> handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline void Textured::setTexture(const res::TextureRef& t) { handle.get().setTexture(t); }

inline const res::TextureRef& Textured::getTexture() const { return handle.get().getTexture(); }

template<typename... TArgs>
void Textured::create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args) {
    registry.emplaceComponent<com::Texture>(entity, std::forward<TArgs>(args)...);
    handle.assign(registry, entity);
}

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl

#endif
