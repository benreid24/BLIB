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
template<typename T, const char* Path>
class Property : public priv::PropertyBase {
public:
    /**
     * @brief Creates the property
     *
     * @param defaultValue The initial and default value for the property
     */
    Property(T&& defaultValue);

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
    const char* configPath() const { return Path; }

    /**
     * @brief Returns the current value of the property
     */
    const T& get() const { return currentValue; }

    /**
     * @brief Sets the value of the property
     *
     * @param newValue The new value of the property
     */
    void set(T&& newValue);

    /**
     * @brief Reads the value of the property from Configuration. Keeps current value if missing
     */
    virtual void syncFromConfig() override;

    /**
     * @brief Writes the current value of the property to Configuration
     */
    virtual void syncToConfig() override;

private:
    T initialValue;
    T currentValue;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, const char* Path>
inline Property<T, Path>::Property(T&& dv)
: initialValue(std::forward<T>(dv)) {
    currentValue = initialValue;
}

template<typename T, const char* Path>
void Property<T, Path>::set(T&& newValue) {
    currentValue = std::forward<T>(newValue);
}

template<typename T, const char* Path>
void Property<T, Path>::syncFromConfig() {
    currentValue = Configuration::getOrDefault<T>(Path, currentValue);
}

template<typename T, const char* Path>
void Property<T, Path>::syncToConfig() {
    Configuration::set<T>(Path, currentValue);
}

#define BLIB_PROPERTY(Name, Type, Path, DefaultValue) \
    static constexpr char _##Name[]          = Path;  \
    bl::engine::Property<Type, _##Name> Name = bl::engine::Property<Type, _##Name>(DefaultValue);

} // namespace engine
} // namespace bl

#endif
