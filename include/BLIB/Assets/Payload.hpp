#ifndef BLIB_ASSETS_PAYLOAD_HPP
#define BLIB_ASSETS_PAYLOAD_HPP

#include <BLIB/Logging.hpp>
#include <stdexcept>
#include <type_traits>

namespace bl
{
namespace as
{
/**
 * @brief Base class for asset payloads. Payloads are the actual data of assets such as images
 *
 * @ingroup Assets
 */
class Payload {
public:
    /**
     * @brief Destroys the payload
     */
    virtual ~Payload() = default;

    /**
     * @brief Safely casts the payload to the given type. Will throw if the cast is invalid
     *
     * @tparam T The payload type to cast to
     * @return A reference to this object as the given type
     */
    template<typename T>
    T& as() {
        static_assert(std::is_base_of_v<Payload, T>, "T must be a subclass of Payload");
        T* casted = dynamic_cast<T*>(this);
        if (!casted) {
            BL_LOG_ERROR << "Invalid Asset payload cast from " << typeid(*this).name() << " to "
                         << typeid(T).name();
            throw std::bad_cast();
        }
        return *casted;
    }

protected:
    //

private:
    //
};

} // namespace as
} // namespace bl

#endif
