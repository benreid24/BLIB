#ifndef BLIB_GUI_RENDERERS_RENDERER_HPP
#define BLIB_GUI_RENDERERS_RENDERER_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

namespace bl
{
namespace gui
{
class Element;

/**
 * @brief Utility class to render GUI elements. Derived classes may override whichever
 *        rendering methods they need to create custom themes
 *
 */
class Renderer : public bl::NonCopyable {
public:
    typedef std::shared_ptr<Renderer> Ptr;

    /**
     * @brief Create the default renderer
     *
     */
    static Ptr create();

    /**
     * @brief Destroy the Renderer object
     *
     */
    virtual ~Renderer() = default;

    /**
     * @brief Render a user defined GUI Element. User code may use group() and id() to
     *        determine how to render
     *
     */
    virtual void renderCustom(sf::RenderTarget& target, const Element& element) const;

    // TODO - method for each Element type

private:
    Renderer() = default;
};

} // namespace gui
} // namespace bl

#endif