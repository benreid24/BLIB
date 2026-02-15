#ifndef BLIB_UTIL_UUID_HPP
#define BLIB_UTIL_UUID_HPP

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

#endif
