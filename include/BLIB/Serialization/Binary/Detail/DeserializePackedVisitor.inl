#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEPACKEDVISITOR_INL
#define BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEPACKEDVISITOR_INL

#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#error "This file should not be included directly. Include Serializer.hpp instead"
#endif

namespace detail
{
template<typename T>
bool deserializePackedVisitor(stream::InputStream& stream, T& value) {
    bool result = true;
    refl::visit(value, [&stream, &result](const auto& reflMember, auto& member) {
        using MemberType = std::decay_t<decltype(member)>;

        if (!result) { return; }
        if (!Serializer<MemberType>::deserialize(stream, member)) { result = false; }
    });

    return result;
}

} // namespace detail

#endif
