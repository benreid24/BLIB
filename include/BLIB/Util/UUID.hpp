#ifndef BLIB_UTIL_UUID_HPP
#define BLIB_UTIL_UUID_HPP

#include <BLIB/Serialization/Binary/Serializer.hpp>
#include <BLIB/Serialization/JSON/Serializer.hpp>
#include <BLIB/Util/HashCombine.hpp>
#include <cstdint>
#include <string>
#include <string_view>

namespace bl
{
namespace util
{
/**
 * @brief UUID Generator
 *
 * @ingroup Util
 */
class UUID {
public:
    /// The fixed length of UUID strings
    static constexpr std::size_t StringLength = 36;

    /**
     * @brief Generates a new UUID string
     *
     * @return A new UUID string
     */
    static UUID generate();

    /**
     * @brief Creates a 0 UUID
     */
    UUID();

    /**
     * @brief Copies the UUID
     *
     * @param copy The UUID to copy
     */
    UUID(const UUID& copy) = default;

    /**
     * @brief Constructs the UUID from a string
     *
     * @param str The string to parse and construct from
     */
    UUID(std::string_view str);

    /**
     * @brief Constructs the UUID from a string
     *
     * @param str The string to parse and construct from
     */
    UUID(const std::string& str);

    /**
     * @brief Constructs the UUID from a string
     *
     * @param str The string to parse and construct from
     */
    UUID(const char* str);

    /**
     * @brief Constructs the UUID from its two 64 bit parts
     *
     * @param part1 The first 64 bits of the UUID
     * @param part2 The second 64 bits of the UUID
     */
    UUID(std::uint64_t part1, std::uint64_t part2);

    /**
     * @brief Copies the UUID
     *
     * @param copy The UUID to copy
     * @return A reference to this object
     */
    UUID& operator=(const UUID& copy) = default;

    /**
     * @brief Compare two UUIDs for equality
     *
     * @param other The UUID to compare to
     * @return True if the UUIDs are equal, false otherwise
     */
    bool operator==(const UUID& other) const {
        return part1 == other.part1 && part2 == other.part2;
    }

    /**
     * @brief Converts the UUID to a string
     */
    std::string toString() const;

    /**
     * @brief Converts the UUID to a string
     */
    operator std::string() const { return toString(); }

    /**
     * @brief Returns the first part of the UUID
     */
    std::uint64_t getPart1() const { return part1; }

    /**
     * @brief Returns the second part of the UUID
     */
    std::uint64_t getPart2() const { return part2; }

private:
    std::uint64_t part1;
    std::uint64_t part2;
};

} // namespace util

namespace serial
{
namespace binary
{
template<>
struct Serializer<util::UUID> {
    static bool serialize(OutputStream& output, const util::UUID& uuid) {
        if (!output.write<std::uint64_t>(uuid.getPart1())) return false;
        return output.write<std::uint64_t>(uuid.getPart2());
    }
    static bool deserialize(InputStream& input, util::UUID& uuid) {
        std::uint64_t part1, part2;
        if (!input.read<std::uint64_t>(part1)) return false;
        if (!input.read<std::uint64_t>(part2)) return false;
        uuid = util::UUID(part1, part2);
        return true;
    }
    static std::uint32_t size(const util::UUID&) { return sizeof(std::uint64_t) * 2; }
};
} // namespace binary

namespace json
{
template<>
struct Serializer<util::UUID> {
    static bool deserialize(util::UUID& result, const Value& v) {
        const std::string* r = v.getAsString();
        if (r != nullptr) {
            result = util::UUID(*r);
            return true;
        }
        return false;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, util::UUID& result) {
        return priv::Serializer<util::UUID>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const util::UUID& value) { return Value(value.toString()); }

    static void serializeInto(Group& result, const std::string& name, const util::UUID& value) {
        priv::Serializer<util::UUID>::serializeInto(result, name, value, &serialize);
    }

    static bool deserializeStream(std::istream& stream, util::UUID& result) {
        json::Loader loader(stream);
        std::string str;
        if (!loader.loadString(str)) { return false; }
        result = util::UUID(str);
        return true;
    }

    static bool serializeStream(std::ostream& stream, const util::UUID& value, unsigned int,
                                unsigned int) {
        return Serializer<std::string>::serializeStream(stream, value.toString(), 0, 0);
    }
};
} // namespace json
} // namespace serial
} // namespace bl

namespace std
{
template<>
struct hash<bl::util::UUID> {
    std::size_t operator()(const bl::util::UUID& uuid) const noexcept {
        std::size_t h1 = std::hash<std::uint64_t>{}(uuid.getPart1());
        std::size_t h2 = std::hash<std::uint64_t>{}(uuid.getPart2());
        return bl::util::hashCombine(h1, h2);
    }
};
} // namespace std

inline std::ostream& operator<<(std::ostream& os, const bl::util::UUID& uuid) {
    os << uuid.toString();
    return os;
}

#endif
