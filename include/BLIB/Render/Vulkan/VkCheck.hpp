#ifndef BLIB_RENDER_VULKAN_VKCHECK_HPP
#define BLIB_RENDER_VULKAN_VKCHECK_HPP

#include <BLIB/Logging.hpp>

#ifdef BLIB_DEBUG
#define vkCheck(expr)                                                           \
    do {                                                                        \
        const auto result = expr;                                               \
        if (result != VK_SUCCESS) {                                             \
            BL_LOG_WARN << "Vulkan call returned " << result << " | " << #expr; \
        }                                                                       \
    } while (false);
#else
#define vkCheck(expr) expr
#endif

#endif
