#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEPACKEDVISITOR_INL
#define BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEPACKEDVISITOR_INL

#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#error "This file should not be included directly. Include Serializer.hpp instead"
#endif

namespace detail
{
template<typename T>
void deserializePackedVisitor(stream::InputStream& stream, T& value) {
    refl::visit(value, [&stream](const auto&, auto& member) {
        using MemberType = std::decay_t<decltype(member)>;
        Serializer<MemberType>::deserialize(stream, member);
    });
}

} // namespace detail

#endif
