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
/**
 * @brief Global typeless configuration store for engine settings. Applications may add their own
 *        configuration settings to the store as well. Some examples of configuration are the path
 *        prefixes for songs in Playlists and spritesheets in Animations. Reading the configuration
 *        is threadsafe, however all calls to set() should be performed before any concurrent
 *        access is attempted
 *
 * @ingroup Engine
 *
 */
class Configuration {
public:
    /**
     * @brief Returns the stored value for the given configuration type and key. If the key does not
     *        exist or is a different type then the default value is returned
     *
     * @tparam T The type of the item to be retrieved
     * @param key The key of the item to be returned
     * @param defaultValue The default value to return if not found
     * @return T The stored configuration value
     */
    template<typename T>
    static T getOrDefault(const std::string& key, const T& defaultValue = {});

    /**
     * @brief Returns the stored value for the given configuration type and key. If the key does not
     *        exist or is a different type then the default constructed value is returned
     *
     * @tparam T The type of the item to be retrieved
     * @param key The key of the item to be returned
     * @return const T& The stored configuration value
     */
    template<typename T>
    static const T& get(const std::string& key);

    /**
     * @brief Sets the value for the given type and key in the config store. Keys may be duplicated
     *        across types. Values may be overwritten by default
     *
     * @tparam T The type of item to set
     * @param key The key to set the value for
     * @param value The value to set the config to
     * @param overwrite Set to true to overwrite existing value
     */
    template<typename T>
    static void set(const std::string& key, const T& value, bool overwrite = true);

    /**
     * @brief Loads the configuration from the given file. Default values should be set for all
     *        expected keys first, then this should be called. Existing keys are maintained but
     *        overwritten if a value is specified in the file
     *
     * @details
     * The configuration file is a plain text file in the format
     *
     *     <type><key>=<value>
     *     <type><key>=<value>
     *     ...
     *
     * Where <type> is one of:
     *     b - boolean value. "true" and "false" are the expected values
     *     i - integer value. Loaded in with int as the type key
     *     u - unsigned integer. Loaded in with unsigned int as the type key
     *     f - floating point number. Loaded in with float as the type key
     *     s - String value
     *
     * <key> is any string not containing the '=' character
     * <value> is any the text between '=' and '\n' and will be parsed as the specified type
     *
     * @param file The file to load from
     * @return True if loaded without error, false if any error. Default values still present
     */
    static bool load(const std::string& file);

    /**
     * @brief Saves the configuration to the given file. See load() for the file format
     * @see load
     *
     * @param file The file to save to
     * @return True on successful write, false for any error
     */
    static bool save(const std::string& file);

    /**
     * @brief Logs the configuration via the included logging utility
     *
     */
    static void log();

    /**
     * @brief Clears out all stored config data. Useful for doing a fresh reload
     *
     */
    static void clear();

private:
    std::unordered_map<std::type_index, std::unordered_map<std::string, std::any>> config;

    Configuration() = default;

    static Configuration& get();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
T Configuration::getOrDefault(const std::string& key, const T& d) {
    auto& config    = get().config;
    const auto tkey = std::type_index(typeid(T));
    const auto it   = config.find(tkey);
    if (it != config.end()) {
        const auto jit = it->second.find(key);
        if (jit != it->second.end()) { return *std::any_cast<T>(&jit->second); }
    }

    return d;
}

template<typename T>
const T& Configuration::get(const std::string& key) {
    auto& config    = get().config;
    const auto tkey = std::type_index(typeid(T));
    const auto it   = config.find(tkey);
    if (it != config.end()) {
        const auto jit = it->second.find(key);
        if (jit != it->second.end()) { return *std::any_cast<T>(&jit->second); }
    }

    static std::unordered_set<std::string> warned;
    static const T d = {};
    if (warned.find(key) == warned.end()) {
        warned.insert(key);
        BL_LOG_WARN << "Using default value for unset config: " << key << " value: " << d;
    }
    return d;
}

template<typename T>
void Configuration::set(const std::string& key, const T& value, bool ov) {
    auto& config    = get().config;
    const auto tkey = std::type_index(typeid(T));

    auto tit = config.find(tkey);
    if (tit == config.end()) { tit = config.try_emplace(tkey).first; }

    auto it = tit->second.find(key);
    if (it == tit->second.end() || ov) { tit->second[key] = std::make_any<T>(value); }
    else {
        BL_LOG_WARN << "Tried to overwrite configuration (" << key << "="
                    << std::any_cast<T>(it->second) << ") to " << value;
    }
}

} // namespace engine
} // namespace bl

#endif
