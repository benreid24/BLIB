#include <BLIB/Util/UUID.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Random.hpp>
#include <algorithm>
#include <cstdint>
#include <numeric>

namespace bl
{
namespace util
{
namespace
{
std::uint64_t byteSwap64(std::uint64_t value) {
    return ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) |
           ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8) |
           ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
           ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56);
}
} // namespace

UUID UUID::generate() {
    std::uint64_t part1 = Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max());
    std::uint64_t part2 = Random::get<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max());
    return UUID(part1, part2);
}

UUID::UUID()
: part1(0)
, part2(0) {}

UUID::UUID(const std::string& str)
: UUID(std::string_view(str)) {}

UUID::UUID(const char* str)
: UUID(std::string_view(str)) {}

UUID::UUID(std::string_view str)
: UUID() {
    part1 = 0;
    part2 = 0;

    if (str.length() != StringLength) {
        BL_LOG_ERROR << "Invalid UUID string: " << str;
        return;
    }

    // Validate hyphen positions: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-') {
        BL_LOG_ERROR << "Invalid UUID string: " << str;
        return;
    }

    auto isHexDigit = [](char c) -> bool {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    };

    auto parseHex = [](char c) -> std::uint64_t {
        if (c >= '0' && c <= '9') { return c - '0'; }
        if (c >= 'a' && c <= 'f') { return c - 'a' + 10; }
        if (c >= 'A' && c <= 'F') { return c - 'A' + 10; }
        return 0;
    };

    for (std::size_t i = 0; i < StringLength; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) { continue; }
        if (!isHexDigit(str[i])) {
            BL_LOG_ERROR << "Invalid UUID string: " << str;
            return;
        }
    }

    std::uint64_t temp1 = 0;
    std::uint64_t temp2 = 0;
    std::size_t idx     = 0;
    for (std::size_t i = 0; i < 16; ++i) {
        if (idx == 8 || idx == 13 || idx == 18 || idx == 23) { ++idx; }

        std::uint64_t byte = (parseHex(str[idx]) << 4) | parseHex(str[idx + 1]);
        idx += 2;

        if (i < 8) { temp1 = (temp1 << 8) | byte; }
        else { temp2 = (temp2 << 8) | byte; }
    }

    if (util::FileUtil::isBigEndian()) {
        part1 = byteSwap64(temp1);
        part2 = byteSwap64(temp2);
    }
    else {
        part1 = temp1;
        part2 = temp2;
    }
}

UUID::UUID(std::uint64_t part1, std::uint64_t part2)
: part1(part1)
, part2(part2) {}

std::string UUID::toString() const {
    const char* hexChars = "0123456789abcdef";
    std::string result;
    result.reserve(StringLength);

    auto appendByte = [&](std::uint8_t byte) {
        result += hexChars[(byte >> 4) & 0xF];
        result += hexChars[byte & 0xF];
    };

    std::uint64_t temp1 = part1;
    std::uint64_t temp2 = part2;

    if (util::FileUtil::isBigEndian()) {
        temp1 = byteSwap64(part1);
        temp2 = byteSwap64(part2);
    }

    // Format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    // part1: 8 bytes (first 8, then 4, then 4 hex chars with hyphens)
    // Byte indices: 7 6 5 4 - 3 2 - 1 0
    // Char groups:  8 chars - 4   - 4

    // First 4 bytes (8 hex chars)
    for (int i = 7; i >= 4; --i) { appendByte((temp1 >> (i * 8)) & 0xFF); }
    result += '-';

    // Next 2 bytes (4 hex chars)
    for (int i = 3; i >= 2; --i) { appendByte((temp1 >> (i * 8)) & 0xFF); }
    result += '-';

    // Last 2 bytes of part1 (4 hex chars)
    for (int i = 1; i >= 0; --i) { appendByte((temp1 >> (i * 8)) & 0xFF); }
    result += '-';

    // part2: 8 bytes (first 4, then 12 hex chars with hyphen)
    // First 2 bytes (4 hex chars)
    for (int i = 7; i >= 6; --i) { appendByte((temp2 >> (i * 8)) & 0xFF); }
    result += '-';

    // Last 6 bytes (12 hex chars)
    for (int i = 5; i >= 0; --i) { appendByte((temp2 >> (i * 8)) & 0xFF); }

    return result;
}

bool UUID::parse(std::string_view str) {
    UUID temp(str);
    if (temp == UUID()) { return false; }
    *this = temp;
    return true;
}

} // namespace util
} // namespace bl
