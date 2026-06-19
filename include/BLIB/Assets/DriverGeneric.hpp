#ifndef BLIB_ASSETS_DRIVERGENERIC_HPP
#define BLIB_ASSETS_DRIVERGENERIC_HPP

#include <BLIB/Assets/Context.hpp>
#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/PayloadGeneric.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Serialization.hpp>
#include <BLIB/Util/TemplateString.hpp>
#include <type_traits>

namespace bl
{
namespace as
{
namespace detail
{
template<typename T>
struct ExtractPayload {
    using Type = T;
};

template<typename T>
struct ExtractPayload<PayloadGeneric<T>> {
    using Type = T;
};

} // namespace detail

/**
 * @brief Generic driver that can be used for assets that are trivially serializable
 *
 * @tparam T The type of data stored in the payload. Must be serializable
 * @tparam Name The string literal name of this driver type
 * @tparam BundleConfig The bundle config for assets of this type
 * @ingroup Assets
 */
template<typename T, util::TemplateString Name,
         bdl::AssetBundleConfig BundleConfig = bdl::AssetBundleConfig()>
class DriverGeneric : public Driver<T> {
public:
    /// The type of the underlying payload. Extracts from PayloadGeneric
    using DataType = typename detail::ExtractPayload<T>::Type;

    /// Helper to determine if T is an instance of PayloadGeneric
    static constexpr bool IsPayloadGeneric = std::is_same_v<T, PayloadGeneric<DataType>>;

    /// The name of the type that this driver is for
    static constexpr std::string_view TypeName = Name.toStringView();

    /**
     * @brief Optional create parameters to use. Will default construct if not provided
     *
     * @ingroup Assets
     */
    struct CreateParams : public CreateContext::CreateData {
        const DataType& data;

        /**
         * @brief Creates the create params with the given data
         *
         * @param data The payload value to create with
         */
        CreateParams(const DataType& data)
        : data(data) {}
    };

    /**
     * @brief Creates the driver
     */
    DriverGeneric()
    : Driver<T>(BundleConfig) {}

    /**
     * @brief Destroys the driver
     */
    virtual ~DriverGeneric() = default;

    /**
     * @brief Creates the asset payload, optionally from CreateParams if provided
     *
     * @param ctx The context containing optional CreateParams
     * @param payload The payload to populate with the created asset data
     * @return Always true
     */
    virtual bool doCreate(CreateContext& ctx, T& payload) override {
        const auto* createData = ctx.getCustomDataAsMaybe<CreateParams>();
        if constexpr (std::is_copy_assignable_v<DataType>) {
            if (createData) { getData(payload) = createData->data; }
        }
        else {
            if (createData) {
                BL_LOG_ERROR << "CreateParams provided for non-copy-assignable type: "
                             << typeid(T).name();
            }
        }
        return true;
    }

    /**
     * @brief Reads the asset from storage
     *
     * @param ctx The read context
     * @param payload The asset to populate
     * @return True if the asset could be read successfully, false otherwise
     */
    virtual bool doRead(ReadContext& ctx, T& payload) override {
        stream::InputStream input;
        if (!ctx.setupReadStream("payload.json", input)) { return false; }
        if (ctx.getMode() == Mode::Editor) {
            return serial::json::Serializer<DataType>::deserializeStream(input, getData(payload));
        }
        else { return serial::binary::Serializer<DataType>::deserialize(input, getData(payload)); }
    }

    /**
     * @brief Writes the asset to storage
     *
     * @param ctx The write context
     * @param payload The asset to write
     * @return True if the asset could be written successfully, false otherwise
     */
    virtual bool doWrite(WriteContext& ctx, const T& payload) override {
        stream::OutputStream output;
        if (!ctx.setupWriteStream("payload.json", output)) { return false; }
        if (ctx.getMode() == Mode::Editor) {
            return serial::json::Serializer<DataType>::serializeStream(
                output, getData(payload), 4, 0);
        }
        else { return serial::binary::Serializer<DataType>::serialize(output, getData(payload)); }
    }

private:
    DataType& getData(T& payload) {
        if constexpr (IsPayloadGeneric) { return payload.get(); }
        else { return payload; }
    }

    const DataType& getData(const T& payload) const {
        if constexpr (IsPayloadGeneric) { return payload.get(); }
        else { return payload; }
    }
};

} // namespace as
} // namespace bl

#endif
