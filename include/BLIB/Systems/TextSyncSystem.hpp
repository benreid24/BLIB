#ifndef BLIB_SYSTEMS_TEXTSYNCSYSTEM_HPP
#define BLIB_SYSTEMS_TEXTSYNCSYSTEM_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/OverlayEntityScaled.hpp>
#include <vector>

namespace bl
{
namespace rc
{
namespace draw
{
class Text;
}
} // namespace rc

namespace sys
{
/**
 * @brief Simple system which ensures that Text geometry stays updated
 *
 * @ingroup Systems
 */
class TextSyncSystem
: public engine::System
, public bl::event::Listener<rc::event::OverlayEntityScaled> {
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
    std::vector<rc::draw::Text*> texts;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;
    virtual void observe(const rc::event::OverlayEntityScaled& event) override;

    void registerText(rc::draw::Text* text);
    void removeText(rc::draw::Text* text);

    friend class rc::draw::Text;
};

} // namespace sys
} // namespace bl

#endif
