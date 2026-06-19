#ifndef BLIB_ASSETS_PAYLOADGENERIC_HPP
#define BLIB_ASSETS_PAYLOADGENERIC_HPP

#include <BLIB/Assets/Payload.hpp>

namespace bl
{
namespace as
{
/**
 * @brief Generic payload container for arbitrary payload types
 *
 * @tparam T The type of data that is stored in the payload
 * @ingroup Assets
 */
template<typename T>
class PayloadGeneric : public Payload {
public:
    /**
     * @brief Creates the payload
     *
     * @param ctx The repo context
     */
    PayloadGeneric(const Payload::ConstructContext& ctx)
    : Payload(ctx) {}

    /**
     * @brief Destroys the payload
     */
    virtual ~PayloadGeneric() = default;

    /**
     * @brief Returns the payload data
     */
    T& get() { return data; }

    /**
     * @brief Returns the payload data
     */
    const T& get() const { return data; }

    /**
     * @brief Flushes the payload data to the asset repository. Only needs to be called if mutated
     *        after creation
     */
    void saveChangesToAssetRepo() { flush(); }

private:
    T data;
};

} // namespace as
} // namespace bl

#endif
