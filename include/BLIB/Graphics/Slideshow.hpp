#ifndef BILB_GRAPHICS_SLIDESHOW_HPP
#define BILB_GRAPHICS_SLIDESHOW_HPP

#include <BLIB/Components/Slideshow.hpp>
#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Graphics/Components/Animation2DPlayer.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Specific type of 2d animation where the texture coordinates change over time
 *
 * @ingroup Graphics
 */
class Slideshow
: public Drawable<com::Slideshow>
, public bcom::OverlayScalable
, public bcom::Animation2DPlayer {
public:
    /**
     * @brief Does nothing
     */
    Slideshow() = default;

    /**
     * @brief Creates the slideshow with its own player
     *
     * @param engine Game engine instance
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    Slideshow(engine::Engine& engine, const resource::Ref<a2d::AnimationData>& animation,
              bool play = false, bool forceLoop = false);

    /**
     * @brief Creates the slideshow sharing animation state with the given slideshow
     *
     * @param engine Game engine instance
     * @param player The other slideshow to share animation state with
     */
    Slideshow(engine::Engine& engine, const Slideshow& player);

    /**
     * @brief Creates the slideshow with its own player
     *
     * @param engine Game engine instance
     * @param animation The animation to use
     * @param play True to play immediately, false to start paused
     * @param forceLoop True to always loop, false defer to animation setting
     */
    void createWithUniquePlayer(engine::Engine& engine,
                                const resource::Ref<a2d::AnimationData>& animation,
                                bool play = false, bool forceLoop = false);

    /**
     * @brief Creates the slideshow sharing animation state with the given slideshow
     *
     * @param engine Game engine instance
     * @param player The other slideshow to share animation state with
     */
    void createWithSharedPlayer(engine::Engine& engine, const Slideshow& player);

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

protected:
    /**
     * @brief Called when the local size is queried
     */
    virtual void ensureLocalSizeUpdated() override;
};

} // namespace gfx
} // namespace bl

#endif
