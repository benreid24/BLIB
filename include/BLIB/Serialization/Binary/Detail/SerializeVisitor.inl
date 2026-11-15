#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_SERIALIZEVISITOR_INL
#define BLIB_SERIALIZATION_BINARY_DETAIL_SERIALIZEVISITOR_INL

#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#error "This file should not be included directly. Include Serializer.hpp instead"
#endif

namespace detail
{
template<typename T>
void serializeVisitor(stream::OutputStream& stream, const T& value, bool packed) {
    using Reflected = refl::ReflectedObject<T>;

    OutputStreamWrapper wrapper(stream);
    if (!packed) { wrapper.write<std::uint16_t>(Reflected::spec.memberCount); }

    refl::visit(value,
                [&stream, &wrapper, packed](const auto& reflMember, const auto& memberValue) {
                    using MemberType               = std::decay_t<decltype(memberValue)>;
                    const std::uint16_t memberId   = reflMember.getId();
                    const std::uint32_t memberSize = Serializer<MemberType>::size(memberValue);

                    if (!packed) {
                        wrapper.write<std::uint16_t>(memberId);
                        wrapper.write<std::uint32_t>(memberSize);
                    }
                    Serializer<MemberType>::serialize(stream, memberValue);
                });
}

} // namespace detail

#endif
