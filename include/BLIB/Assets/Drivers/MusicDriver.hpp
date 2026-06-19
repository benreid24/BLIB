#ifndef BLIB_ASSETS_DRIVERS_MUSICDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_MUSICDRIVER_HPP

#include <BLIB/Assets/Builtin/MusicPayload.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/PayloadGeneric.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <cstdint>
#include <vector>

namespace bl
{
/// Contains built-in payloads and drivers
namespace asi
{
/**
 * @brief Driver for sf::Music assets
 *
 * @ingroup Assets
 */
class MusicDriver : public as::Driver<MusicPayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Music";

    /**
     * @brief Optional creation parameters for music assets using in-memory samples
     */
    struct CreateParams : public as::CreateContext::CreateData {
        /// Raw 16-bit audio samples. Takes priority over path
        const std::int16_t* samples = nullptr;
        std::uint64_t sampleCount   = 0;
        unsigned int channelCount   = 1;
        unsigned int sampleRate     = 44100;
        std::vector<sf::SoundChannel> channelMap;
    };

    /**
     * @brief Creates the driver
     */
    MusicDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~MusicDriver() = default;

    /**
     * @brief Creates a new music asset
     *
     * @param ctx Context containing a path to load from
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(as::CreateContext& ctx, MusicPayload& payload) override;

    /**
     * @brief Reads the music asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(as::ReadContext& ctx, MusicPayload& payload) override;

    /**
     * @brief Writes the music asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(as::WriteContext& ctx, const MusicPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
