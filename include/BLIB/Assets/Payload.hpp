#ifndef BLIB_ASSETS_PAYLOAD_HPP
#define BLIB_ASSETS_PAYLOAD_HPP

#include <BLIB/Logging.hpp>
#include <type_traits>

namespace bl
{
namespace as
{
class Payload {
public:
    virtual ~Payload() = default;

    template<typename T>
    T& as() {
        static_assert(std::is_base_of_v<Payload, T>, "T must be a subclass of Payload");
        T* casted = dynamic_cast<T*>(this);
        if (!casted) {
            BL_LOG_ERROR << "Invalid Asset payload cast from " << typeid(*this).name() << " to "
                         << typeid(T).name();
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
