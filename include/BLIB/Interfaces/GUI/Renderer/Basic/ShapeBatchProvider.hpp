#ifndef BLIB_GUI_RENDERER_BASIC_SHAPEBATCHPROVIDER_HPP
#define BLIB_GUI_RENDERER_BASIC_SHAPEBATCHPROVIDER_HPP

#include <BLIB/Graphics/BatchedShapes2D.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
class Component;
}

namespace defcoms
{
/**
 * @brief Intermediate base class for components that provide panes for batching geometry
 *
 * @ingroup GUI
 */
class ShapeBatchProvider {
public:
    /**
     * @brief Creates the provider
     */
    ShapeBatchProvider();

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
    static ShapeBatchProvider* findProvider(rdr::Component* component);

protected:
    /**
     * @brief Sets whether or not the provider is enabled
     *
     * @param enabled True to allow use, false to disable
     */
    void setEnabled(bool enabled);

private:
    gfx::BatchedShapes2D batch;
    bool enabled;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
