#ifndef BLIB_COMPONENTS_SPRITE_HPP
#define BLIB_COMPONENTS_SPRITE_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Color.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Renderer;
}

namespace com
{
/**
 * @brief Sprite component repersenting a 2d image for an entity
 *
 * @ingroup Components
 */
class Sprite : public rc::rcom::DrawableBase {
public:
    /**
     * @brief Creates an empty sprite
     */
    Sprite() = default;

    /**
     * @brief Creates the sprite so that it may be added to a scene
     *
     * @param renderer The renderer instance
     * @param texture The texture to render
     * @param region The region of the texture to render
     */
    Sprite(rc::Renderer& renderer, const rc::res::TextureRef& texture,
           const sf::FloatRect& region = {});

    /**
     * @brief Creates the sprite so that it may be added to a scene
     *
     * @param renderer The renderer instance
     * @param texture The texture to render
     * @param region The region of the texture to render
     */
    void create(rc::Renderer* renderer, const rc::res::TextureRef& texture,
                sf::FloatRect region = {});

    /**
     * @brief Swap out the texture to use. Does not reset texture region. Call create() to reset
     *        region texture
     *
     * @param texture The new texture to render
     * @param resetVertices Whether to reset vertex positions and texture coords
     */
    void setTexture(const rc::res::TextureRef& texture, bool resetVertices = false);

    /**
     * @brief Updates the texture coordinates to render with
     *
     * @param region The source region from the texture
     */
    void setTextureSource(const sf::FloatRect& region);

    /**
     * @brief Returns the texture that is rendered by the sprite
     */
    const rc::res::TextureRef& getTexture() const;

    /**
     * @brief Sets the color to multiply the texture color by
     *
     * @param color The color to multiply with
     */
    void setColor(const rc::Color& color);

    /**
     * @brief Returns the color that is being applied
     */
    rc::Color getColor() const;

    /**
     * @brief Returns the pre-transform size of the Sprite
     */
    const glm::vec2& getSize() const;

    /**
     * @brief Returns the default pipeline for regular scenes
     */
    virtual std::uint32_t getDefaultScenePipelineId() const override {
        return rc::Config::PipelineIds::LitSkinned2DGeometry;
    }

    /**
     * @brief Returns the default pipeline for overlays
     */
    virtual std::uint32_t getDefaultOverlayPipelineId() const override {
        return rc::Config::PipelineIds::UnlitSkinned2DGeometry;
    }

private:
    rc::buf::IndexBuffer buffer;
    rc::res::TextureRef texture;
    glm::vec2 size;

    void refreshTrans();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const rc::res::TextureRef& Sprite::getTexture() const { return texture; }

inline const glm::vec2& Sprite::getSize() const { return size; }

inline rc::Color Sprite::getColor() const { return buffer.vertices()[0].color; }

} // namespace com
} // namespace bl

#endif
