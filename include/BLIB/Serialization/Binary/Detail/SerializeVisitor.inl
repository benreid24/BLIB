#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_SERIALIZEVISITOR_INL
#define BLIB_SERIALIZATION_BINARY_DETAIL_SERIALIZEVISITOR_INL

#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#error "This file should not be included directly. Include Serializer.hpp instead"
#endif

namespace detail
{
template<typename T>
bool serializeVisitor(stream::OutputStream& stream, const T& value, bool packed) {
    using Reflected = refl::ReflectedObject<T>;

    bool result = true;
    OutputStreamWrapper wrapper(stream);
    if (!packed && !wrapper.write<std::uint16_t>(Reflected::spec.memberCount)) { return false; }

    refl::visit(
        value,
        [&stream, &wrapper, &result, packed](const auto& reflMember, const auto& memberValue) {
            if (!result) { return; }

            using MemberType               = std::decay_t<decltype(memberValue)>;
            const std::uint16_t memberId   = reflMember.getId();
            const std::uint32_t memberSize = Serializer<MemberType>::size(memberValue);

            if (!packed) {
                if (!wrapper.write<std::uint16_t>(memberId)) {
                    result = false;
                    return;
                }
                if (!wrapper.write<std::uint32_t>(memberSize)) {
                    result = false;
                    return;
                }
            }
            if (!Serializer<MemberType>::serialize(stream, memberValue)) {
                result = false;
                return;
            }
        });

    return result;
}

} // namespace detail

#endif
