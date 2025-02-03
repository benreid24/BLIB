#ifndef BLIB_GRAPHICS_COMPONENTS_TEXTURED_HPP
#define BLIB_GRAPHICS_COMPONENTS_TEXTURED_HPP

#include <BLIB/Components/MaterialInstance.hpp>
#include <BLIB/ECS.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
/**
 * @brief Base component class for textured Drawables
 *
 * @ingroup Graphics
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
    void setTexture(const rc::res::TextureRef& texture);

    /**
     * @brief Returns the texture. Must only be called after create()
     */
    const rc::res::TextureRef& getTexture() const;

protected:
    /**
     * @brief Creates the texture component
     *
     * @param renderer The renderer instance
     * @param The entity's material instance
     * @param texture The texture to use
     */
    void create(rc::Renderer& renderer, com::MaterialInstance* material,
                const rc::res::TextureRef& texture);

private:
    rc::Renderer* renderer;
    com::MaterialInstance* handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const rc::res::TextureRef& Textured::getTexture() const {
    return handle->getMaterial()->getTexture();
}

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
