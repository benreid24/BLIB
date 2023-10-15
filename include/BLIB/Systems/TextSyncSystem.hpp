#ifndef BLIB_SYSTEMS_TEXTSYNCSYSTEM_HPP
#define BLIB_SYSTEMS_TEXTSYNCSYSTEM_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <vector>

namespace bl
{
namespace gfx
{
class Text;
}

namespace sys
{
/**
 * @brief Simple system which ensures that Text geometry stays updated
 *
 * @ingroup Systems
 */
class TextSyncSystem : public engine::System {
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
    std::vector<gfx::Text*> texts;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float, float, float) override;

    void registerText(gfx::Text* text);
    void removeText(gfx::Text* text);

    friend class gfx::Text;
};

} // namespace sys
} // namespace bl

#endif
