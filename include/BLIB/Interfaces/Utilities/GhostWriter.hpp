#ifndef BLIB_INTERFACES_UTILITIES_GHOSTWRITER_HPP
#define BLIB_INTERFACES_UTILITIES_GHOSTWRITER_HPP

#include <BLIB/Graphics/Text.hpp>
#include <string>
#include <string_view>

namespace bl
{
/// Collection of classes and utilities for interfaces
namespace interface
{
/**
 * @brief Basic string wrapper that allows for text to be unhidden over time. The speed of text
 *        reveal, in characters per second, is managed by the "blib.interface.ghost_speed"
 *        configuration key
 *
 * @ingroup InterfaceUtilities
 */
class GhostWriter {
public:
    /**
     * @brief Construct a new GhostWriter with empty content
     */
    GhostWriter();

    /**
     * @brief Construct a new GhostWriter with the given content
     *
     * @param content The string to show
     */
    GhostWriter(const std::string& content);

    /**
     * @brief Set the content to display. Resets to showing 0 characters
     *
     * @param content The string to slowly reveal
     */
    void setContent(const std::string& content);

    /**
     * @brief Returns the full content of the writer
     */
    const std::string& getContent() const;

    /**
     * @brief Returns the visible portion of the content of the writer
     */
    std::string_view getVisible() const;

    /**
     * @brief Updates the writer and reveals more characters based on time elapsed
     *
     * @param dt Time elapsed, in seconds
     * @return True if any new characters were revealed, false if unchanged
     */
    bool update(float dt);

    /**
     * @brief Immediately shows all characters
     */
    void showAll();

    /**
     * @brief Returns whether or not all characters are visible
     */
    bool finished() const;

    /**
     * @brief The ghost writer will update the first section of text
     *
     * @param text The text object to manage
     */
    void manage(gfx::Text& text);

    /**
     * @brief The ghost writer will update the given section of text
     *
     * @param section The specific section of text to manage
     */
    void manage(gfx::txt::BasicText& section);

    /**
     * @brief The ghost writer will stop managing graphical text
     */
    void stopManaging();

private:
    const float speed;
    std::string content;
    unsigned int showing;
    float residual;
    gfx::txt::BasicText* managing;

    void syncText();
};

} // namespace interface
} // namespace bl

#endif
