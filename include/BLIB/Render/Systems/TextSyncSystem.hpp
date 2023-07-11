#ifndef BLIB_RENDER_SYSTEMS_TEXTSYNCSYSTEM_HPP
#define BLIB_RENDER_SYSTEMS_TEXTSYNCSYSTEM_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/OverlayEntityScaled.hpp>
#include <vector>

namespace bl
{
namespace gfx
{
namespace draw
{
class Text;
}

namespace sys
{
/**
 * @brief Simplpe system which ensures that Text geometry stays updated
 *
 * @ingroup Renderer
 */
class TextSyncSystem
: public engine::System
, public bl::event::Listener<event::OverlayEntityScaled> {
public:
    /**
     * @brief Initializes the system
     */
    TextSyncSystem();

    /**
     * @brief Destroys the system
     */
    virtual ~TextSyncSystem() = default;

private:
    std::vector<draw::Text*> texts;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
    virtual void observe(const event::OverlayEntityScaled& event) override;

    void registerText(draw::Text* text);
    void removeText(draw::Text* text);

    friend class draw::Text;
};

} // namespace sys
} // namespace gfx
} // namespace bl

#endif
