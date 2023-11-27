#ifndef BLIB_GUI_RENDERER_BASIC_SHAPEBATCHPROVIDER_HPP
#define BLIB_GUI_RENDERER_BASIC_SHAPEBATCHPROVIDER_HPP

#include <BLIB/Graphics/BatchedShapes2D.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Intermediate base class for components that provide panes for batching geometry
 *
 * @ingroup GUI
 */
class ShapeBatchProvider : public rdr::Component {
public:
    /**
     * @brief Creates the provider
     *
     * @param highlightState How the component should respond to mouse events
     */
    ShapeBatchProvider(HighlightState highlightState);

    /**
     * @brief Destroys the provider
     */
    virtual ~ShapeBatchProvider() = default;

    /**
     * @brief Returns the shape batch to use to batch geometry
     */
    gfx::BatchedShapes2D& getShapeBatch();

    /**
     * @brief Returns whether or not this provider can be used
     */
    bool isEnabled() const { return enabled; }

    /**
     * @brief Helper method to find the correct provider in the parent chain
     *
     * @param component The component to get the provider for
     * @return A pointer to the provider to use, may be nullptr
     */
    static ShapeBatchProvider* findProvider(Component* component);

    /**
     * @brief Helper method to determine the offset for the given component from its batch provider
     *
     * @param component The component to get the offset for
     * @return The position relative to the batch provider
     */
    static glm::vec2 determineOffset(Component* component);

protected:
    gfx::BatchedShapes2D batch;

    /**
     * @brief Sets whether or not the provider is enabled
     *
     * @param enabled True to allow use, false to disable
     */
    void setEnabled(bool enabled);

private:
    bool enabled;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
