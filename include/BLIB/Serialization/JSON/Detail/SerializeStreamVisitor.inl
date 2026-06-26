#ifndef BLIB_SERIALIZATION_JSON_DETAIL_SERIALIZESTREAMVISITOR_INL
#define BLIB_SERIALIZATION_JSON_DETAIL_SERIALIZESTREAMVISITOR_INL

namespace detail
{
template<typename T>
bool serializeStreamVisitor(stream::OutputStream& stream, const T& value, unsigned int tabSize,
                            unsigned int currentIndent) {
    stream << "{";
    if (tabSize > 0) { stream << "\n"; }

    constexpr std::size_t memberCount = refl::memberCount<T>;
    const unsigned int sc             = currentIndent + tabSize;
    unsigned int i                    = 0;
    refl::visit(value,
                [&stream, &i, &sc, &tabSize](const auto& reflMember, const auto& memberValue) {
                    util::StreamUtil::writeRepeated(stream, ' ', sc);
                    stream << '"' << reflMember.getName() << "\": ";
                    json::Serializer<std::decay_t<decltype(memberValue)>>::serializeStream(
                        stream, memberValue, tabSize, sc);
                    if (i < memberCount - 1) { stream << ","; }
                    ++i;
                    if (tabSize > 0) { stream << "\n"; }
                });

    util::StreamUtil::writeRepeated(stream, ' ', currentIndent);
    stream << "}";

    return stream.isValid();
}

} // namespace detail

#endif
