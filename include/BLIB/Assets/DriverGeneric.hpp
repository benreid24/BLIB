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
/**
 * @brief Generic driver that can be used for assets that are trivially serializable. Intended for
 *        use with PayloadGeneric and the underlying asset payload which should be serializable
 *
 * @tparam T The type of data stored in the payload. Must be serializable
 * @tparam Name The string literal name of this driver type
 * @ingroup Assets
 */
template<typename T, util::TemplateString Name>
class DriverGeneric : public Driver<PayloadGeneric<T>> {
public:
    /// The name of the type that this driver is for
    static constexpr std::string_view TypeName = Name.toStringView();

    /**
     * @brief Optional create parameters to use. Will default construct if not provided
     *
     * @ingroup Assets
     */
    struct CreateParams : public CreateContext::CreateData {
        const T& data;

        /**
         * @brief Creates the create params with the given data
         *
         * @param data The payload value to create with
         */
        CreateParams(const T& data)
        : data(data) {}
    };

    /**
     * @brief Creates the driver
     */
    DriverGeneric() = default;

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
    virtual bool doCreate(const CreateContext& ctx, PayloadGeneric<T>& payload) override {
        const auto* createData = ctx.getCustomDataAsMaybe<CreateParams>();
        if constexpr (std::is_copy_assignable_v<T>) {
            if (createData) { payload.get() = createData->data; }
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
    virtual bool doRead(const ReadContext& ctx, PayloadGeneric<T>& payload) override {
        stream::InputStream input;
        if (!ctx.setupReadStream("payload.json", input)) { return false; }
        if (ctx.getMode() == Mode::Editor) {
            return serial::json::Serializer<T>::deserializeStream(input, payload.get());
        }
        else { return serial::binary::Serializer<T>::deserialize(input, payload.get()); }
    }

    /**
     * @brief Writes the asset to storage
     *
     * @param ctx The write context
     * @param payload The asset to write
     * @return True if the asset could be written successfully, false otherwise
     */
    virtual bool doWrite(const WriteContext& ctx, const PayloadGeneric<T>& payload) override {
        stream::OutputStream output;
        if (!ctx.setupWriteStream("payload.json", output)) { return false; }
        if (ctx.getMode() == Mode::Editor) {
            return serial::json::Serializer<T>::serializeStream(output, payload.get(), 4, 0);
        }
        else { return serial::binary::Serializer<T>::serialize(output, payload.get()); }
    }
};

} // namespace as
} // namespace bl

#endif
