#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_SIZEVISITOR_INL
#define BLIB_SERIALIZATION_BINARY_DETAIL_SIZEVISITOR_INL

namespace detail
{
template<typename T>
std::uint32_t sizeVisitor(const T& value) {
    std::uint32_t result = sizeof(std::uint16_t);
    refl::visit(value, [&result](const auto&, const auto& memberValue) {
        using MemberType = std::decay_t<decltype(memberValue)>;
        result += sizeof(std::uint16_t) + sizeof(std::uint32_t); // member id and size
        result += Serializer<MemberType>::size(memberValue);
    });
    return result;
}

} // namespace detail

#endif
