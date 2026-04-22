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
        const char b1 = data[i];
        const char b2 = i + 1 < data.size() ? data[i + 1] : 0;
        const char b3 = i + 2 < data.size() ? data[i + 2] : 0;
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
        const char b1 = Table[strchr(Table, c1) - Table];
        const char b2 = Table[strchr(Table, c2) - Table];
        result.push_back((b1 << 2) | (b2 >> 4));
        if (c3 != '=') {
            const char b3 = Table[strchr(Table, c3) - Table];
            result.push_back(((b2 & 0x0F) << 4) | (b3 >> 2));
            if (c4 != '=') {
                const char b4 = Table[strchr(Table, c4) - Table];
                result.push_back(((b3 & 0x03) << 6) | b4);
            }
        }
    }
    return true;
}

} // namespace util
} // namespace bl
