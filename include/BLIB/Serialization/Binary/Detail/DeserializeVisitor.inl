#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEVISITOR_INL
#define BLIB_SERIALIZATION_BINARY_DETAIL_DESERIALIZEVISITOR_INL

#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#error "This file should not be included directly. Include Serializer.hpp instead"
#endif

namespace detail
{
constexpr unsigned int MaxFieldCount = 128;

template<typename T>
bool deserializeVisitor(stream::InputStream& stream, T& value) {
    using Reflected = refl::ReflectedObject<T>;

    bool result = true;
    InputStreamWrapper wrapper(stream);
    char foundFields[MaxFieldCount];
    std::memset(foundFields, 0, MaxFieldCount);

    std::uint16_t fieldCount;
    if (!wrapper.read<std::uint16_t>(fieldCount)) { return false; }

    for (std::uint16_t i = 0; i < fieldCount; ++i) {
        std::uint16_t memberId;
        std::uint32_t memberSize;
        if (!wrapper.read<std::uint16_t>(memberId)) { return false; }
        if (!wrapper.read<std::uint32_t>(memberSize)) { return false; }
        if (memberId >= MaxFieldCount) { return false; }
        foundFields[memberId] = 1;

        bool parsedField = false;
        refl::visit(
            value,
            [&stream, &result, &parsedField, memberId](const auto& reflMember, auto& memberValue) {
                if (parsedField || reflMember.getId() != memberId) { return; }

                using MemberType = std::decay_t<decltype(memberValue)>;
                parsedField      = true;
                if (!Serializer<MemberType>::deserialize(stream, memberValue)) { result = false; }
            });
        if (!result) { return false; }
        if (!parsedField) {
            if (!wrapper.skip(memberSize)) { return false; }
        }
    }

    // default fields not found and fail if any are required
    refl::visit(value, [&foundFields, &result](const auto& reflMember, auto& member) {
        using MemberType   = std::decay_t<decltype(member)>;
        using DefaultTrait = refl::attr::DefaultValue<MemberType>;

        if constexpr (!reflMember.hasAttribute<Trait::Optional>()) {
            if (foundFields[reflMember.getId()] == 0) { result = false; }
        }
        else if constexpr (reflMember.hasAttribute<DefaultTrait>()) {
            if (foundFields[reflMember.getId()] == 0) {
                member = reflMember.getAttribute<DefaultTrait>()->value;
            }
        }
    });

    return result;
}

} // namespace detail

#endif
