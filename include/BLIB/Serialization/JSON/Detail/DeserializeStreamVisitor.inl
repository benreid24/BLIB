#ifndef BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZESTREAMVISITOR_INL
#define BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZESTREAMVISITOR_INL

namespace detail
{
constexpr std::size_t MaxFieldCount = 128;

template<typename T>
bool deserializeStreamVisitor(stream::InputStream& stream, T& value) {
    char foundFields[MaxFieldCount];
    std::memset(foundFields, 0, MaxFieldCount);

    util::StreamUtil::skipWhitespace(stream);
    if (stream.peek() != '{') { return false; }
    stream.get();

    while (stream.isValid()) {
        util::StreamUtil::skipWhitespace(stream);
        if (stream.peek() != '"') { return false; }
        stream.get();

        std::string name;
        util::StreamUtil::getline(stream, name, '"');
        if (!stream.isValid()) { return false; }
        if (!util::StreamUtil::skipUntil(stream, ':')) { return false; }
        stream.get();

        bool parsedField = false;
        refl::visit(value,
                    [&stream, &name, &parsedField, &foundFields](const auto& reflMember,
                                                                 auto& memberValue) {
                        if (parsedField) { return; }
                        using MemberType = std::decay_t<decltype(memberValue)>;
                        if (name == reflMember.getName()) {
                            parsedField                     = true;
                            foundFields[reflMember.getId()] = 1;
                            if (!Serializer<MemberType>::deserializeStream(stream, memberValue)) {
                                parsedField = false;
                            }
                        }
                    });

        // field not found, skip data
        if (!parsedField) {
            json::Loader loader(stream);
            json::Value trash(false);
            if (!loader.loadValue(trash)) { return false; }
        }

        util::StreamUtil::skipWhitespace(stream);
        const char c = stream.get();
        if (c == '}') {
            // check fields not found for required
            bool result = true;
            refl::visit(value, [&foundFields, &result](const auto& reflMember, auto& memberValue) {
                if (!result) { return; }
                if (foundFields[reflMember.getId()] == 0) {
                    if constexpr (reflMember.template hasAttribute<Trait::Optional>()) {
                        using DefaultTrait =
                            refl::attr::DefaultValue<std::decay_t<decltype(memberValue)>>;
                        if constexpr (reflMember.template hasAttribute<DefaultTrait>()) {
                            const auto* defaultAttr = reflMember.template getAttribute<DefaultTrait>();
                            if (defaultAttr) { memberValue = defaultAttr->value; }
                        }
                    }
                    else { result = false; }
                }
            });
            return true;
        }
        else if (c != ',') { return false; }
    }

    // shouldnt get here unless EOF
    return false;
}

} // namespace detail

#endif
