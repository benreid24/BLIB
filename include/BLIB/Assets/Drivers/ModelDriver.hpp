#ifndef BLIB_ASSETS_DRIVERS_MODELDRIVER_HPP
#define BLIB_ASSETS_DRIVERS_MODELDRIVER_HPP

#include <BLIB/Assets/Builtin/ModelPayload.hpp>
#include <BLIB/Assets/Driver.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Asset driver for loading and saving 3d models using Assimp
 *
 * @ingroup Assets
 */
class ModelDriver : public as::Driver<ModelPayload> {
public:
    /// The string id of this driver type
    static constexpr std::string_view TypeName = "Models";

    /**
     * @brief Creates the driver
     */
    ModelDriver();

    /**
     * @brief Destroys the driver
     */
    virtual ~ModelDriver() = default;

    /**
     * @brief Creates a new model asset
     *
     * @param ctx Context containing a path, CreateParams, or no custom data
     * @param payload The payload to populate with the created asset data
     * @return True on success, false on failure
     */
    virtual bool doCreate(as::CreateContext& ctx, ModelPayload& payload) override;

    /**
     * @brief Reads the model asset data from the repository
     *
     * @param ctx The read context
     * @param payload The payload to populate with the read asset data
     * @return True on success, false on failure
     */
    virtual bool doRead(as::ReadContext& ctx, ModelPayload& payload) override;

    /**
     * @brief Writes the model asset data to the repository
     *
     * @param ctx The write context
     * @param payload The payload containing the asset data to write
     * @return True on success, false on failure
     */
    virtual bool doWrite(as::WriteContext& ctx, const ModelPayload& payload) override;
};

} // namespace asi
} // namespace bl

#endif
