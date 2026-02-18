#ifndef BLIB_ASSETS_TYPEDREF_HPP
#define BLIB_ASSETS_TYPEDREF_HPP

#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Assets/Ref.hpp>
#include <type_traits>

namespace bl
{
namespace as
{
template<typename T>
class TypedRef : public Ref {
    static_assert(std::is_base_of<Payload, T>::value, "T must be an Payload type");

public:
    T& get();

    const T& get() const;

private:
    //
};

} // namespace as
} // namespace bl

#endif
