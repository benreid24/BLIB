#ifndef BLIB_COMPONENTS_TEXT_HPP
#define BLIB_COMPONENTS_TEXT_HPP

#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

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
     * @brief Updates the draw parameters to render the entire vertex buffer
     */
    void updateDrawParams();
};

} // namespace com
} // namespace bl

#endif
