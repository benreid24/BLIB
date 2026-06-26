#ifndef BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZEVISITOR_INL
#define BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZEVISITOR_INL

namespace detail
{
template<typename T>
bool deserializeVisitor(const json::Group& group, T& value) {
    using Reflected = refl::ReflectedObject<T>;
    bool result     = true;
    refl::visit(value, [&group, &result](const auto& reflMember, auto& memberValue) {
        if (!result) { return; }

        using MemberType = std::decay_t<decltype(memberValue)>;
        const std::string memberName(reflMember.getName());
        const json::Value* memberJson = group.getField(memberName);

        if (!memberJson) {
            if constexpr (reflMember.hasAttribute<Trait::Optional>()) {
                using DefaultTrait = refl::attr::DefaultValue<MemberType>;
                if constexpr (reflMember.hasAttribute<DefaultTrait>()) {
                    const auto* defaultAttr = reflMember.getAttribute<DefaultTrait>();
                    if (defaultAttr) { memberValue = defaultAttr->value; }
                    else { memberValue = MemberType{}; }
                }
            }
            else {
                result = false;
                return;
            }
        }
        else {
            if (!json::Serializer<MemberType>::deserialize(memberValue, *memberJson)) {
                result = false;
            }
        }
    });
    return result;
}
} // namespace detail

#endif
