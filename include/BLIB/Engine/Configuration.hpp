#ifndef BLIB_ENGINE_CONFIGURATION_HPP
#define BLIB_ENGINE_CONFIGURATION_HPP

#include <BLIB/Logging.hpp>

#include <any>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

namespace bl
{
namespace engine
{
class Configuration {
public:
    template<typename T>
    static const T& get(const std::string& key, const T& defaultValue = T());

    template<typename T>
    static void set(const std::string& key, const T& value);

private:
    std::unordered_map<std::type_index, std::unordered_map<std::string, std::any>> config;

    Configuration() = default;

    static Configuration& get();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
const T& Configuration::get(const std::string& key, const T& d) {
    auto& config    = get().config;
    const auto tkey = std::type_index(typeid(T));
    const auto it   = config.find(tkey);
    if (it != config.end()) {
        const auto jit = it->second.find(key);
        if (jit != it->second.end()) { return *std::any_cast<T>(&jit->second); }
    }

    static std::unordered_set<std::string> warned;
    if (warned.find(key) == warned.end()) {
        warned.insert(key);
        BL_LOG_WARN << "Using default value for unset config: " << key << " value: " << d;
    }
    return d;
}

template<typename T>
void Configuration::set(const std::string& key, const T& value) {
    auto& config    = get().config;
    const auto tkey = std::type_index(typeid(T));

    auto tit = config.find(tkey);
    if (tit == config.end()) { tit = config.try_emplace(tkey).first; }

    auto it = tit->second.find(key);
    if (it == tit->second.end()) {
        BL_LOG_INFO << "Configuration set: " << key << "=" << value;
        tit->second.emplace(key, value);
    }
    else {
        BL_LOG_WARN << "Tried to overwrite configuration (" << key << "="
                    << std::any_cast<T>(it->second) << ") to " << value;
    }
}

inline Configuration& Configuration::get() {
    static Configuration configuration;
    return configuration;
}

} // namespace engine
} // namespace bl

#endif
