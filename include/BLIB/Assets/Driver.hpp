#ifndef BLIB_ASSETS_DRIVER_HPP
#define BLIB_ASSETS_DRIVER_HPP

#include <BLIB/Assets/Context.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <memory>

namespace bl
{
namespace as
{
/**
 * @brief Base class for drivers that provide support for different asset formats
 *
 * @ingroup Assets
 */
class Driver {
public:
    /**
     * @brief Destroys the driver
     */
    virtual ~Driver() = default;

    /**
     * @brief Imports the asset from an external source
     *
     * @param ctx The context for the import operation
     * @return The payload containing the imported asset data, or nullptr if the import failed
     */
    virtual std::unique_ptr<Payload> importExternal(const ImportContext& ctx) = 0;

    /**
     * @brief Reads an existing asset into memory
     *
     * @param ctx The context for the read operation
     * @return The payload containing the read asset data, or nullptr if the read failed
     */
    virtual std::unique_ptr<Payload> read(const ReadContext& ctx) = 0;

    /**
     * @brief Writes the asset data to disk in the appropriate format
     *
     * @param ctx The context for the write operation
     * @return True if the write succeeded, false otherwise
     */
    virtual bool write(const WriteContext& ctx) = 0;
};

} // namespace as
} // namespace bl

#endif
