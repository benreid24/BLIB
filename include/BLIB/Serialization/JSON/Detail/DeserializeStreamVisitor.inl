#ifndef BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZESTREAMVISITOR_INL
#define BLIB_SERIALIZATION_JSON_DETAIL_DESERIALIZESTREAMVISITOR_INL

namespace detail
{
constexpr std::size_t MaxFieldCount = 128;

template<typename T>
void deserializeStreamVisitor(::bl::stream::InputStream& stream, T& value) {
    try {
        char foundFields[MaxFieldCount];
        std::memset(foundFields, 0, MaxFieldCount);

        ::bl::stream::StreamUtil::skipWhitespace(stream);
        if (stream.peek() != '{') {
            throw SerializationError("Unexpected character: ", stream.peek());
        }
        stream.get();

        while (stream.isValid()) {
            ::bl::stream::StreamUtil::skipWhitespace(stream);
            if (stream.peek() != '"') {
                throw SerializationError("Unexpected character: ", stream.peek());
            }
            stream.get();

            std::string name;
            ::bl::stream::StreamUtil::getline(stream, name, '"');
            ::bl::stream::StreamUtil::skipUntil(stream, ':');
            stream.get();

            bool foundField = false;
            refl::visit(value,
                        [&stream, &name, &foundFields, &foundField](const auto& reflMember,
                                                                    auto& memberValue) {
                            using MemberType = std::decay_t<decltype(memberValue)>;
                            if (name == reflMember.getName()) {
                                foundField = true;
                                try {
                                    Serializer<MemberType>::deserializeStream(stream, memberValue);
                                    foundFields[reflMember.getId()] = 1; // allow optionals to fail
                                } catch (const SerializationError& e) {
                                    throw SerializationError(e, "Failed to parse field: " + name);
                                }
                            }
                        });

            // field not found, skip data
            if (!foundField) {
                json::Loader loader(stream);
                json::Value trash(false);
                if (!loader.loadValue(trash)) {
                    throw SerializationError("Failed to parse ignored field: " + name);
                }
            }

            ::bl::stream::StreamUtil::skipWhitespace(stream);
            const char c = stream.get();
            if (c == '}') {
                // check fields not found for required
                refl::visit(value, [&foundFields](const auto& reflMember, auto& memberValue) {
                    if (foundFields[reflMember.getId()] == 0) {
                        if constexpr (reflMember.template hasAttribute<Trait::Optional>()) {
                            using DefaultTrait =
                                refl::attr::DefaultValue<std::decay_t<decltype(memberValue)>>;
                            if constexpr (reflMember.template hasAttribute<DefaultTrait>()) {
                                const auto* defaultAttr =
                                    reflMember.template getAttribute<DefaultTrait>();
                                if (defaultAttr) { memberValue = defaultAttr->value; }
                            }
                        }
                        else {
                            std::string message = "Missing required field: ";
                            const auto fieldName = reflMember.getName();
                            message.append(fieldName.data(), fieldName.size());
                            throw SerializationError(std::move(message));
                        }
                    }
                });
                return;
            }
            else if (c != ',') { throw SerializationError("Unexpected character: ", c); }
        }

        // shouldnt get here unless EOF
        throw SerializationError("Unexpected end of stream");
    } catch (const ::bl::stream::StreamError& e) { throw SerializationError(e); }
}

} // namespace detail

#endif
