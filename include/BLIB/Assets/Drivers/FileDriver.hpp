#ifndef BLIB_ASSETS_DRIVERS_FILEDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_FILEDRIVER_HPP

#include <BLIB/Assets/Builtin/FilePayload.hpp>
#include <BLIB/Assets/Driver.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Driver for generic file assets
 *
 * @ingroup Assets
 */
class FileDriver : public as::Driver<FilePayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "PlainFiles";

    /**
     * @brief Creates the driver
     */
    FileDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~FileDriver() = default;

    /**
     * @brief Creates a new music asset
     *
     * @param ctx Context containing a path to load from
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(as::CreateContext& ctx, FilePayload& payload) override;

    /**
     * @brief Reads the music asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(as::ReadContext& ctx, FilePayload& payload) override;

    /**
     * @brief Writes the music asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(as::WriteContext& ctx, const FilePayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
