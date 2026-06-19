#ifndef BLIB_ASSETS_DRIVERS_FONTDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_FONTDRIVER_HPP

#include <BLIB/Assets/Builtin/FontPayload.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <span>

namespace bl
{
namespace asi
{
/**
 * @brief Driver for font assets
 *
 * @ingroup Assets
 */
class FontDriver : public as::Driver<FontPayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Fonts";

    /**
     * @brief Optional parameters for creating a font asset
     */
    struct CreateParams : public as::CreateContext::CreateData {
        /**
         * @brief Creates the font by loading from a source file
         *
         * @param path The file to load from
         */
        CreateParams(const std::string& path);

        /**
         * @brief Creates the font by loading from memory
         *
         * @param data The data to load from
         */
        CreateParams(std::span<const char> data);

        std::span<const char> data;
    };

    /**
     * @brief Creates the driver
     */
    FontDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~FontDriver() = default;

    /**
     * @brief Creates a new font asset
     *
     * @param ctx Context containing a path, CreateParams, or no custom data
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(as::CreateContext& ctx, FontPayload& payload) override;

    /**
     * @brief Reads the font asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(as::ReadContext& ctx, FontPayload& payload) override;

    /**
     * @brief Writes the font asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(as::WriteContext& ctx, const FontPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
