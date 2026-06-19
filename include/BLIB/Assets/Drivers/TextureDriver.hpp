#ifndef BLIB_ASSETS_DRIVERS_TEXTUREDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_TEXTUREDRIVER_HPP

#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Assets/Driver.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Asset driver for loading and saving textures
 *
 * @ingroup Assets
 */
class TextureDriver : public as::Driver<TexturePayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Textures";

    /**
     * @brief Creates the driver
     */
    TextureDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~TextureDriver() = default;

    /**
     * @brief Creates a new texture asset
     *
     * @param ctx Context containing a path, CreateParams, or no custom data
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(as::CreateContext& ctx, TexturePayload& payload) override;

    /**
     * @brief Reads the texture asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(as::ReadContext& ctx, TexturePayload& payload) override;

    /**
     * @brief Writes the texture asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(as::WriteContext& ctx, const TexturePayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
