#ifndef BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZEVISITOR_INL
#define BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZEVISITOR_INL

namespace detail
{
template<typename T>
void deserializeVisitor(const ::bl::serial::json::Group& group, T& value) {
    refl::visit(value, [&group](const auto& reflMember, auto& memberValue) {
        using MemberType = std::decay_t<decltype(memberValue)>;
        const std::string memberName(reflMember.getName());
        const ::bl::serial::json::Value* memberJson = group.getField(memberName);

        if (!memberJson) {
            if constexpr (reflMember.template hasAttribute<Trait::Optional>()) {
                using DefaultTrait = refl::attr::DefaultValue<MemberType>;
                if constexpr (reflMember.template hasAttribute<DefaultTrait>()) {
                    const auto* defaultAttr = reflMember.template getAttribute<DefaultTrait>();
                    if (defaultAttr) { memberValue = defaultAttr->value; }
                    else { memberValue = MemberType{}; }
                }
            }
            else { throw SerializationError("Missing required field: " + memberName); }
        }
        else {
            try {
                ::bl::serial::json::Serializer<MemberType>::deserialize(memberValue, *memberJson);
            } catch (const SerializationError& e) {
                throw SerializationError(e, std::string("Failed to parse field: ") + memberName);
            }
        }
    });
}
} // namespace detail

#endif
