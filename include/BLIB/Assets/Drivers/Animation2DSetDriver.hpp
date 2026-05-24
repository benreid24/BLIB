#ifndef BLIB_ASSETS_DRIVERS_ANIMATION2DSETDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_ANIMATION2DSETDRIVER_HPP

#include <BLIB/Assets/Builtin/Animation2DSetPayload.hpp>
#include <BLIB/Assets/Driver.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Driver for 2d animation set assets
 *
 * @ingroup Assets
 */
class Animation2DSetDriver : public as::Driver<Animation2DSetPayload> {
public:
    static constexpr std::string_view TypeName = "Animation2DSets";

    /**
     * @brief Creates the driver
     */
    Animation2DSetDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~Animation2DSetDriver() = default;

    /**
     * @brief Creates a new image asset
     *
     * @param ctx Context containing a path, CreateParams, or no custom data
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(const as::CreateContext& ctx, Animation2DSetPayload& payload) override;

    /**
     * @brief Reads the image asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(const as::ReadContext& ctx, Animation2DSetPayload& payload) override;

    /**
     * @brief Writes the image asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(const as::WriteContext& ctx,
                         const Animation2DSetPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
