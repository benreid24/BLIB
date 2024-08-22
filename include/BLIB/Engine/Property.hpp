#ifndef BLIB_ENGINE_PROPERTY_HPP
#define BLIB_ENGINE_PROPERTY_HPP

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <string_view>

namespace bl
{
namespace engine
{
/// Private implementation details
namespace priv
{
class PropertyBase : private util::NonCopyable {
public:
    PropertyBase();
    ~PropertyBase();
    virtual void syncFromConfig() = 0;
    virtual void syncToConfig()   = 0;
};
} // namespace priv

/**
 * @brief Helper class to manage application properties stored in engine Configuration
 *
 * @tparam T The type of the property
 * @tparam Path The URI of the property in Configuration
 * @ingroup Engine
 */
template<typename T>
class Property : public priv::PropertyBase {
public:
    /**
     * @brief Creates the property
     *
     * @param defaultValue The initial and default value for the property
     * @param path The configuration path for the property
     */
    Property(const T& defaultValue, std::string_view path);

    /**
     * @brief Destroys the property
     */
    ~Property() = default;

    /**
     * @brief Returns the default value of the property
     */
    const T& defaultValue() const { return initialValue; }

    /**
     * @brief Returns the URI of the property in Configuration
     */
    const char* configPath() const { return path.data(); }

    /**
     * @brief Returns the current value of the property
     */
    const T& get() const { return currentValue; }

    /**
     * @brief Sets the value of the property
     *
     * @param newValue The new value of the property
     * @param writeThrough Whether to also set the entry in Configuration
     */
    void set(const T& newValue, bool writeThrough = true);

    /**
     * @brief Reads the value of the property from Configuration. Keeps current value if missing
     */
    virtual void syncFromConfig() override;

    /**
     * @brief Writes the current value of the property to Configuration
     */
    virtual void syncToConfig() override;

private:
    std::string_view path;
    T initialValue;
    T currentValue;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
inline Property<T>::Property(const T& dv, std::string_view path)
: path(path)
, initialValue(dv)
, currentValue(dv) {}

template<typename T>
void Property<T>::set(const T& newValue, bool wt) {
    currentValue = newValue;
    if (wt) { syncToConfig(); }
}

template<typename T>
void Property<T>::syncFromConfig() {
    currentValue = Configuration::getOrDefault<T>(path.data(), currentValue);
}

template<typename T>
void Property<T>::syncToConfig() {
    Configuration::set<T>(path.data(), currentValue);
}

#define BLIB_PROPERTY(Name, Type, Path, DefaultValue) \
    bl::engine::Property<Type> Name = bl::engine::Property<Type>(DefaultValue, Path);

} // namespace engine
} // namespace bl

#endif
