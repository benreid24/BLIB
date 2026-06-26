#ifndef BLIB_SERIALIZATION_JSON_DETAIL_SERIALIZEVISITOR_INL
#define BLIB_SERIALIZATION_JSON_DETAIL_SERIALIZEVISITOR_INL

namespace detail
{
template<typename T>
Group serializeVisitor(const T& value) {
    Group result;
    refl::visit(value, [&result](const auto& reflMember, const auto& memberValue) {
        using MemberType = std::decay_t<decltype(memberValue)>;
        result.addField(std::string(reflMember.getName()),
                        Serializer<MemberType>::serialize(memberValue));
    });
    return result;
}
} // namespace detail

#endif
