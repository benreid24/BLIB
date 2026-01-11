#ifndef BLIB_COMPONENTS_TEXT_HPP
#define BLIB_COMPONENTS_TEXT_HPP

#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component for rendered text
 *
 * @ingroup Components
 */
struct Text : public rc::rcom::DrawableBase {
    rc::buf::VertexBuffer vertices;

    /**
     * @brief Creates an empty component
     */
    Text();

    /**
     * @brief Updates the draw parameters to render the entire vertex buffer
     *
     * @param vertexCount The number of vertices to actually render
     */
    void updateDrawParams(std::uint32_t vertexCount);

    /**
     * @brief Returns the default material pipeline for rendering
     */
    virtual std::uint32_t getDefaultMaterialPipelineId() const override {
        return rc::cfg::MaterialPipelineIds::Text;
    }
};

} // namespace com
} // namespace bl

#endif
