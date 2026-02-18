#ifndef BLIB_ASSETS_DEPENDENCY_HPP
#define BLIB_ASSETS_DEPENDENCY_HPP

#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Serialization/SerializableField.hpp>
#include <BLIB/Serialization/SerializableObject.hpp>
#include <string_view>
#include <type_traits>

namespace bl
{
namespace as
{

template<typename T = Payload>
class Dependency : public detail::DependencyChain {
public:
    static_assert(std::is_base_of<Payload, T>::value, "T must be a Payload type");

    Dependency(Repository& repo, Asset& owner, std::string_view tag);

    Dependency(const Dependency&) = delete;

    Dependency(Dependency&&);

    ~Dependency() = default;

    Dependency& operator=(Dependency&&);

    Dependency& operator=(const Dependency&) = delete;

    T& get();

    const T& get() const;
};

} // namespace as

namespace serial
{
template<typename T>
struct SerializableObject<as::Dependency<T>> : public SerializableObjectBase {
    SerializableField<1, as::Dependency<T>, util::UUID> uuid;

    SerializableObject()
    : SerializableObjectBase("AssetDependency")
    , uuid("uuid", *this, &as::Dependency<T>::uuid, SerializableFieldBase::Required{}) {}
};

} // namespace serial

} // namespace bl

#endif
