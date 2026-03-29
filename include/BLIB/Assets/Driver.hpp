#ifndef BLIB_ASSETS_DRIVER_HPP
#define BLIB_ASSETS_DRIVER_HPP

#include <BLIB/Assets/Context.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <memory>
#include <string_view>
#include <type_traits>

namespace bl
{
namespace as
{
class Repository;

namespace detail
{
/**
 * @brief Base class for drivers that provide support for different asset formats
 *
 * @ingroup Assets
 */
class DriverBase {
public:
    /**
     * @brief Destroys the driver
     */
    virtual ~DriverBase() = default;

    /**
     * @brief Returns the human friendly type tag the driver supports
     */
    std::string_view getSupportedType() const { return supportedType; }

    /**
     * @brief Imports the asset from an external source
     *
     * @param ctx The context for the import operation
     * @return The payload containing the imported asset data, or nullptr if the import failed
     */
    virtual std::unique_ptr<Payload> create(const CreateContext& ctx) = 0;

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

private:
    std::string_view supportedType;

    friend class Repository;
};
} // namespace detail

/**
 * @brief Base class for drivers that provide support for different payload types
 *
 * @tparam T The payload type this driver supports
 * @ingroup Assets
 */
template<typename T>
class Driver : public detail::DriverBase {
    static_assert(std::is_base_of_v<Payload, T>, "T must be a subclass of Payload");

public:
    /// The type of payload this driver supports
    using TPayload = T;

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
    virtual std::unique_ptr<Payload> create(const CreateContext& ctx) override final {
        Payload::ConstructContext constructCtx{ctx.getMode(), ctx.getRepository(), ctx.getAsset()};
        std::unique_ptr<Payload> payload = std::make_unique<T>(constructCtx);
        if (!doCreate(ctx, payload->as<T>())) { return nullptr; }
        return payload;
    }

    /**
     * @brief Reads an existing asset into memory
     *
     * @param ctx The context for the read operation
     * @return The payload containing the read asset data, or nullptr if the read failed
     */
    virtual std::unique_ptr<Payload> read(const ReadContext& ctx) override final {
        Payload::ConstructContext constructCtx{ctx.getMode(), ctx.getRepository(), ctx.getAsset()};
        std::unique_ptr<Payload> payload = std::make_unique<T>(constructCtx);
        if (!payload->loadDependencies()) {
            BL_LOG_ERROR << "Failed to load dependencies for asset " << ctx.getAsset().getUUID();
            return nullptr;
        }
        if (!doRead(ctx, payload->as<T>())) { return nullptr; }
        return payload;
    }

    /**
     * @brief Writes the asset data to disk in the appropriate format
     *
     * @param ctx The context for the write operation
     * @return True if the write succeeded, false otherwise
     */
    virtual bool write(const WriteContext& ctx) override final {
        return doWrite(ctx, ctx.getAsset().getPayload().as<T>());
    }

protected:
    /**
     * @brief Payload type specific create logic goes here
     *
     * @param ctx The context for the create operation
     * @param payload The payload to populate
     * @return True on success, false on error
     */
    virtual bool doCreate(const CreateContext& ctx, T& payload) = 0;

    /**
     * @brief Payload type specific read logic goes here
     *
     * @param ctx The context for the read operation
     * @param payload The payload to populate
     * @return True on success, false on error
     */
    virtual bool doRead(const ReadContext& ctx, T& payload) = 0;

    /**
     * @brief Payload type specific write logic goes here
     *
     * @param ctx The context for the write operation
     * @param payload The payload to write
     * @return True on success, false on error
     */
    virtual bool doWrite(const WriteContext& ctx, const T& payload) = 0;
};

} // namespace as
} // namespace bl

#endif
