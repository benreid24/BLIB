#ifndef BLIB_GUI_RENDERERS_RENDERER_HPP
#define BLIB_GUI_RENDERERS_RENDERER_HPP

#include <BLIB/GUI/Renderers/RenderSettings.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>

namespace bl
{
namespace gui
{
class Element;
class Container;
class Label;

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
     * @brief Add or set RenderSettings for the given group. Overriden for Elements with id
     *        level overrides
     *
     * @param group The group to apply the settings to
     * @param settings The render settings
     */
    void setGroupSettings(const std::string& group, const RenderSettings& settings);

    /**
     * @brief Add or set RenderSettings for the given id. Settings applied in the element
     *        itself, if any, override these
     *
     * @param id Id of the Element to set for
     * @param settings The render settings
     */
    void setIdSettings(const std::string& id, const RenderSettings& settings);

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

    /**
     * @brief Renders a Container element
     *
     * @param target Target to render to
     * @param container Container to render
     */
    virtual void renderContainer(sf::RenderTarget& target, const Container& container) const;

    /**
     * @brief Renders a Label element
     *
     * @param target The target to render to
     * @param label Label to render
     */
    virtual void renderLabel(sf::RenderTarget& target, const Label& label) const;

protected:
    /**
     * @brief Construct a new default renderer
     *
     */
    Renderer() = default;

    /**
     * @brief Returns an aggregated RenderSettings object for the given group and id. Settings
     *        with no values are left empty. Care must still be taken to respect element level
     *        overrides
     *
     */
    RenderSettings getSettings(const std::string& group, const std::string& id) const;

private:
    std::unordered_map<std::string, RenderSettings> groupSettings;
    std::unordered_map<std::string, RenderSettings> idSettings;
};

} // namespace gui
} // namespace bl

#endif