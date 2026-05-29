#ifndef BLIB_ASSETS_BUILTIN_ANIMATION2DDRIVER_HPP
#define BLIB_ASSETS_BUILTIN_ANIMATION2DDRIVER_HPP

#include <BLIB/Assets/Builtin/Animation2DPayload.hpp>
#include <BLIB/Assets/Driver.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Driver for 2D animation assets
 *
 * @ingroup Assets
 */
class Animation2DDriver : public as::Driver<Animation2DPayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Animation2D";

    /**
     * @brief Creates the driver
     */
    Animation2DDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~Animation2DDriver() = default;

    /**
     * @brief Creates a new animation asset
     *
     * @param ctx Context containing a path, CreateParams, or no custom data
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(as::CreateContext& ctx, Animation2DPayload& payload) override;

    /**
     * @brief Reads the animation asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(as::ReadContext& ctx, Animation2DPayload& payload) override;

    /**
     * @brief Writes the animation asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(as::WriteContext& ctx, const Animation2DPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
