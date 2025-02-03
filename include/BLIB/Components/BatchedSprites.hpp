#ifndef BLIB_COMPONENTS_BATCHEDSPRITES_HPP
#define BLIB_COMPONENTS_BATCHEDSPRITES_HPP

#include <BLIB/Render/Buffers/BatchIndexBuffer.hpp>
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
 * @brief Component for batched 2d sprites
 *
 * @ingroup Components
 */
class BatchedSprites : public rc::rcom::DrawableBase {
public:
    /**
     * @brief Creates an empty BatchedSprites
     */
    BatchedSprites() = default;

    /**
     * @brief Creates the BatchedSprites so that it may be added to a scene
     *
     * @param renderer The renderer instance
     * @param texture The texture to render
     * @param initialCapacity The estimated number of sprites that will be batched
     */
    BatchedSprites(rc::Renderer& renderer, const rc::res::TextureRef& texture,
                   unsigned int initialCapacity);

    /**
     * @brief Creates the BatchedSprites so that it may be added to a scene
     *
     * @param renderer The renderer instance
     * @param texture The texture to render
     * @param initialCapacity The estimated number of sprites that will be batched
     */
    void create(rc::Renderer& renderer, const rc::res::TextureRef& texture,
                unsigned int initialCapacity);

    /**
     * @brief Updates and commits the draw parameters
     */
    void updateDrawParams();

    /**
     * @brief Returns the buffer to be used for batching
     */
    rc::buf::BatchIndexBuffer& getBuffer() { return buffer; }

    /**
     * @brief Returns the default material pipeline for rendering
     */
    virtual std::uint32_t getDefaultMaterialPipelineId() const override {
        return rc::Config::MaterialPipelineIds::Geometry2DSkinned;
    }

private:
    rc::buf::BatchIndexBuffer buffer;
    rc::res::TextureRef texture;

    void refreshTrans();
};

} // namespace com
} // namespace bl

#endif
