#ifndef RESOURCES_UTILS_HPP
#define RESOURCES_UTILS_HPP

#include <sstream>

inline std::string formatSize(unsigned int byteCount) {
    constexpr unsigned int KB = 1000;
    constexpr unsigned int MB = KB * 1000;
    constexpr unsigned int GB = MB * 1000;

    std::stringstream ss;
    if (byteCount > GB) {
        const float gb = static_cast<float>(byteCount) / static_cast<float>(GB);
        ss << std::fixed << gb << " GB";
    }
    else if (byteCount > MB) {
        const float mb = static_cast<float>(byteCount) / static_cast<float>(MB);
        ss << std::fixed << mb << " MB";
    }
    else if (byteCount > KB) {
        const float kb = static_cast<float>(byteCount) / static_cast<float>(KB);
        ss << std::fixed << kb << " KB";
    }
    else { ss << byteCount << " bytes"; }
    return ss.str();
}

#endif
