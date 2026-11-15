#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEVISITOR_INL
#define BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEVISITOR_INL

#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#error "This file should not be included directly. Include Serializer.hpp instead"
#endif

namespace detail
{
constexpr unsigned int MaxFieldCount = 128;

template<typename T>
void deserializeVisitor(::bl::stream::InputStream& stream, T& value) {
    InputStreamWrapper wrapper(stream);
    char foundFields[MaxFieldCount];
    std::memset(foundFields, 0, MaxFieldCount);

    std::uint16_t fieldCount;
    wrapper.read<std::uint16_t>(fieldCount);

    for (std::uint16_t i = 0; i < fieldCount; ++i) {
        std::uint16_t memberId;
        std::uint32_t memberSize;
        wrapper.read<std::uint16_t>(memberId);
        wrapper.read<std::uint32_t>(memberSize);
        if (memberId >= MaxFieldCount) {
            throw SerializationError("Member ID exceeds maximum field count");
        }
        foundFields[memberId] = 1;

        bool parsedField = false;
        refl::visit(value,
                    [&stream, &parsedField, memberId](const auto& reflMember, auto& memberValue) {
                        if (parsedField || reflMember.getId() != memberId) { return; }

                        using MemberType = std::decay_t<decltype(memberValue)>;
                        parsedField      = true;
                        Serializer<MemberType>::deserialize(stream, memberValue);
                    });
        if (!parsedField) { wrapper.skip(memberSize); }
    }

    // default fields not found and fail if any are required
    refl::visit(value, [&foundFields](const auto& reflMember, auto& member) {
        using MemberType   = std::decay_t<decltype(member)>;
        using DefaultTrait = refl::attr::DefaultValue<MemberType>;

        if constexpr (!reflMember.template hasAttribute<Trait::Optional>()) {
            if (foundFields[reflMember.getId()] == 0) {
                std::string message = "Missing required field: ";
                const auto fieldName = reflMember.getName();
                message.append(fieldName.data(), fieldName.size());
                throw SerializationError(std::move(message));
            }
        }
        else if constexpr (reflMember.template hasAttribute<DefaultTrait>()) {
            if (foundFields[reflMember.getId()] == 0) {
                member = reflMember.template getAttribute<DefaultTrait>()->value;
            }
        }
    });
}

} // namespace detail

#endif
