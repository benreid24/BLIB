#ifndef BLIB_ASSETS_DRIVERS_CUBEMAPDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_CUBEMAPDRIVER_HPP

#include <BLIB/Assets/Builtin/CubemapPayload.hpp>
#include <BLIB/Assets/Driver.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Asset driver for cubemaps
 *
 * @ingroup Assets
 */
class CubemapDriver : public as::Driver<CubemapPayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Cubemaps";

    /**
     * @brief Creates the driver
     */
    CubemapDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~CubemapDriver() = default;

    /**
     * @brief Creates a new cubemap asset
     *
     * @param ctx Context containing a path to load from
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(as::CreateContext& ctx, CubemapPayload& payload) override;

    /**
     * @brief Reads the cubemap asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(as::ReadContext& ctx, CubemapPayload& payload) override;

    /**
     * @brief Writes the cubemap asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(as::WriteContext& ctx, const CubemapPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
