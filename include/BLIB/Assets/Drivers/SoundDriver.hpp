#ifndef BLIB_ASSETS_DRIVERS_SOUNDDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_SOUNDDRIVER_HPP

#include <BLIB/Assets/Builtin/SoundPayload.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/PayloadGeneric.hpp>

namespace bl
{
/// Contains built-in payloads and drivers
namespace asi
{
/**
 * @brief Driver for sf::SoundBuffer assets
 *
 * @ingroup Assets
 */
class SoundDriver : public as::Driver<SoundPayload> {
public:
    /**
     * @brief Creates the driver
     */
    SoundDriver() = default;

    /**
     * @brief Destroys the driver
     */
    virtual ~SoundDriver() = default;

    /**
     * @brief Creates a new sound asset
     *
     * @param ctx Context containing a path to load from
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(const as::CreateContext& ctx, SoundPayload& payload) override;

    /**
     * @brief Reads the sound asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(const as::ReadContext& ctx, SoundPayload& payload) override;

    /**
     * @brief Writes the sound asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(const as::WriteContext& ctx, const SoundPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
