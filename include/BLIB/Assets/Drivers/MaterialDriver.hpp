#ifndef BLIB_ASSETS_DRIVERS_MATERIALDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_MATERIALDRIVER_HPP

#include <BLIB/Assets/Builtin/MaterialPayload.hpp>
#include <BLIB/Assets/Driver.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Asset driver for loading and saving materials
 *
 * @ingroup Assets
 */
class MaterialDriver : public as::Driver<MaterialPayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Materials";

    /**
     * @brief Creates the driver
     */
    MaterialDriver() = default;

    /**
     * @brief Destroys the driver
     */
    virtual ~MaterialDriver() = default;

    /**
     * @brief Creates a new material asset
     *
     * @param ctx Context containing a path, CreateParams, or no custom data
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(const as::CreateContext& ctx, MaterialPayload& payload) override;

    /**
     * @brief Reads the material asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(const as::ReadContext& ctx, MaterialPayload& payload) override;

    /**
     * @brief Writes the material asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(const as::WriteContext& ctx, const MaterialPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
