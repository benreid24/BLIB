#ifndef BLIB_SERIALIZATION_JSON_DETAIL_SERIALIZESTREAMVISITOR_INL
#define BLIB_SERIALIZATION_JSON_DETAIL_SERIALIZESTREAMVISITOR_INL

namespace detail
{
template<typename T>
void serializeStreamVisitor(::bl::stream::OutputStream& stream, const T& value,
                            unsigned int tabSize, unsigned int currentIndent) {
    try {
        stream << "{";
        if (tabSize > 0) { stream << "\n"; }

        constexpr std::size_t memberCount = refl::memberCount<T>;
        const unsigned int sc             = currentIndent + tabSize;
        unsigned int i                    = 0;
        refl::visit(value,
                    [&stream, &i, &sc, &tabSize](const auto& reflMember, const auto& memberValue) {
                        ::bl::stream::StreamUtil::writeRepeated(stream, ' ', sc);
                        stream << '"' << reflMember.getName() << "\": ";
                        ::bl::serial::json::Serializer<std::decay_t<decltype(memberValue)>>::
                            serializeStream(stream, memberValue, tabSize, sc);
                        if (i < memberCount - 1) { stream << ","; }
                        ++i;
                        if (tabSize > 0) { stream << "\n"; }
                    });

        ::bl::stream::StreamUtil::writeRepeated(stream, ' ', currentIndent);
        stream << "}";
    } catch (const ::bl::stream::StreamError& e) { throw SerializationError(e); }
}

} // namespace detail

#endif
