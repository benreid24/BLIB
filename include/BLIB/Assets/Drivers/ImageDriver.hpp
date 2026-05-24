#ifndef BLIB_ASSETS_DRIVERS_IMAGE_HPP
#define BLIB_ASSETS_DRIVERS_IMAGE_HPP

#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/PayloadGeneric.hpp>

namespace bl
{
/// Contains built-in payloads and drivers
namespace asi
{
/**
 * @brief Driver for sf::Image assets
 *
 * @ingroup Assets
 */
class ImageDriver : public as::Driver<ImagePayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Images";

    /**
     * @brief Optional creation parameters for image assets
     */
    struct CreateParams : public as::CreateContext::CreateData {
        unsigned int width;
        unsigned int height;
        sf::Color fillColor;

        /// Takes priority over above parameters
        const sf::Image* sourceImage = nullptr;
    };

    /**
     * @brief Creates the driver
     */
    ImageDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~ImageDriver() = default;

    /**
     * @brief Creates a new image asset
     *
     * @param ctx Context containing a path, CreateParams, or no custom data
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(const as::CreateContext& ctx, ImagePayload& payload) override;

    /**
     * @brief Reads the image asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(const as::ReadContext& ctx, ImagePayload& payload) override;

    /**
     * @brief Writes the image asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(const as::WriteContext& ctx, const ImagePayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
