#include <BLIB/Util/Base64.hpp>

namespace bl
{
namespace util
{
namespace
{
constexpr const char* Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}
std::string Base64::encode(std::span<const char> data) {
    std::string result;
    result.reserve(((data.size() + 2) / 3) * 4);
    for (std::size_t i = 0; i < data.size(); i += 3) {
        const unsigned char b1 = static_cast<unsigned char>(data[i]);
        const unsigned char b2 = i + 1 < data.size() ? static_cast<unsigned char>(data[i + 1]) : 0;
        const unsigned char b3 = i + 2 < data.size() ? static_cast<unsigned char>(data[i + 2]) : 0;
        result += Table[b1 >> 2];
        result += Table[((b1 & 0x03) << 4) | (b2 >> 4)];
        if (i + 1 < data.size()) { result += Table[((b2 & 0x0F) << 2) | (b3 >> 6)]; }
        else { result += '='; }
        if (i + 2 < data.size()) { result += Table[b3 & 0x3F]; }
        else { result += '='; }
    }
    return result;
}

bool Base64::decode(const std::string& encoded, std::vector<char>& result) {
    if (encoded.size() % 4 != 0) return false;
    result.clear();
    result.reserve((encoded.size() / 4) * 3);
    for (std::size_t i = 0; i < encoded.size(); i += 4) {
        const char c1 = encoded[i];
        const char c2 = encoded[i + 1];
        const char c3 = encoded[i + 2];
        const char c4 = encoded[i + 3];
        if (c1 == '=' || c2 == '=') return false;
        const auto b1 = static_cast<unsigned char>(strchr(Table, c1) - Table);
        const auto b2 = static_cast<unsigned char>(strchr(Table, c2) - Table);
        result.push_back(static_cast<char>((b1 << 2) | (b2 >> 4)));
        if (c3 != '=') {
            const auto b3 = static_cast<unsigned char>(strchr(Table, c3) - Table);
            result.push_back(static_cast<char>(((b2 & 0x0F) << 4) | (b3 >> 2)));
            if (c4 != '=') {
                const auto b4 = static_cast<unsigned char>(strchr(Table, c4) - Table);
                result.push_back(static_cast<char>(((b3 & 0x03) << 6) | b4));
            }
        }
    }
    return true;
}

} // namespace util
} // namespace bl
