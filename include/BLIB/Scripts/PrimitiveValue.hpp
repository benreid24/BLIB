#ifndef BLIB_SCRIPTS_PRIMITIVEVALUE_HPP
#define BLIB_SCRIPTS_PRIMITIVEVALUE_HPP

#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/ReferenceValue.hpp>
#include <vector>

namespace bl
{
namespace script
{
class Value;

/// An array of values
using ArrayValue = std::vector<Value>;

/**
 * @brief Represents the lowest level values in scripts
 *
 * @ingroup Scripts
 *
 */
class PrimitiveValue {
public:
    /// The type of the primitive value
    enum Type : std::uint8_t {
        TVoid       = 0x0,
        TBool       = 0x1 << 0,
        TString     = 0x1 << 1,
        TNumeric    = 0x1 << 2,
        TArray      = 0x1 << 3,
        TFunction   = 0x1 << 4,
        TRef        = 0x1 << 5,
        _TYPE_COUNT = 6 // excludes void
    };

    /**
     * @brief Helper function to print types as strings
     *
     * @param type The type to get the string for
     * @return std::string The string representation of the type
     */
    static std::string typeToString(Type type);

    /**
     * @brief Makes Void type
     *
     */
    PrimitiveValue();

    /**
     * @brief Copy constructor
     *
     */
    PrimitiveValue(const PrimitiveValue&) = default;

    /**
     * @brief Move constructor
     *
     */
    PrimitiveValue(PrimitiveValue&&) = default;

    /**
     * @brief Copy operator
     *
     */
    PrimitiveValue& operator=(const PrimitiveValue&) = default;

    /**
     * @brief Move operator
     *
     */
    PrimitiveValue& operator=(PrimitiveValue&&) = default;

    /**
     * @brief Makes integer or bool type based on parameter type
     *
     * @tparam T The type
     * @param intOrBool The int or bool value
     */
    template<typename T, class = std::enable_if_t<std::is_integral_v<T>>>
    PrimitiveValue& operator=(T intOrBool) {
        if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            type = TBool;
            value.emplace<bool>(intOrBool);
        }
        else {
            type = TNumeric;
            value.emplace<float>(static_cast<float>(intOrBool));
        }
        return *this;
    }

    /**
     * @brief Makes integer or bool type based on parameter type
     *
     * @tparam T The type
     * @param intOrBool The int or bool value
     */
    template<typename T, class = std::enable_if_t<std::is_integral_v<T>>>
    PrimitiveValue(T intOrBool) {
        if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            type = TBool;
            value.emplace<bool>(intOrBool);
        }
        else {
            type = TNumeric;
            value.emplace<float>(static_cast<float>(intOrBool));
        }
    }

    /**
     * @brief Makes Numeric type
     *
     */
    PrimitiveValue& operator=(float num);

    /**
     * @brief Makes Numeric type
     *
     */
    PrimitiveValue(float num);

    /**
     * @brief Makes String type
     *
     */
    PrimitiveValue& operator=(const std::string& str);

    /**
     * @brief Makes String type
     *
     */
    PrimitiveValue(const std::string& str);

    /**
     * @brief Makes String type
     *
     */
    PrimitiveValue& operator=(const char* str);

    /**
     * @brief Makes String type
     *
     */
    PrimitiveValue(const char* str);

    /**
     * @brief Makes ArrayValue type
     *
     */
    PrimitiveValue& operator=(ArrayValue&& array);

    /**
     * @brief Makes ArrayValue type
     *
     */
    PrimitiveValue(ArrayValue&& array);

    /**
     * @brief Makes ArrayValue type
     *
     */
    PrimitiveValue(const ArrayValue& array);

    /**
     * @brief Makes Ref type
     *
     */
    PrimitiveValue& operator=(const ReferenceValue& ref);

    /**
     * @brief Makes Ref type
     *
     */
    PrimitiveValue(const ReferenceValue& ref);

    /**
     * @brief Make Function type
     *
     */
    PrimitiveValue& operator=(const Function& func);

    /**
     * @brief Make Function type
     *
     */
    PrimitiveValue(const Function& func);

    /**
     * @brief Returns the current type of this PrimitiveValue
     *
     */
    Type getType() const;

    /**
     * @brief Dereferences the value or returns this value if not a reference
     *
     */
    const PrimitiveValue& deref() const;

    /**
     * @brief Get the as Bool type. Performs implicit conversion
     *
     * @param depth The current stack depth. Used to validate references
     * @return The value as a boolean
     */
    bool getAsBool() const;

    /**
     * @brief Get the as Numeric value
     *
     * @return float The value or 0 if not a String
     */
    float getAsNum() const;

    /**
     * @brief Get the as String value
     *
     * @return std::string The value or "ERROR" if not String type
     */
    const std::string& getAsString() const;

    /**
     * @brief Get the as ArrayValue value
     *
     * @return ArrayValue An ArrayValue of Values or empty
     */
    const ArrayValue& getAsArray() const;

    /**
     * @brief Get the as ArrayValue value
     *
     * @return ArrayValue An ArrayValue of Values or empty
     */
    ArrayValue& getAsArray();

    /**
     * @brief Get the as ReferenceValue value
     *
     * @return ReferenceValue A reference to the PrimitiveValue this references, or nullptr
     */
    const ReferenceValue& getAsRef() const;

    /**
     * @brief Get the as ReferenceValue value
     *
     * @return ReferenceValue A reference to the PrimitiveValue this references, or nullptr
     */
    ReferenceValue& getAsRef();

    /**
     * @brief Get the as Function type
     *
     * @return Function The Function value
     */
    const Function& getAsFunction() const;

private:
    struct Empty {};
    using TData =
        std::variant<Empty, bool, float, std::string, ArrayValue, ReferenceValue, Function>;

    Type type;
    TData value;
};

} // namespace script
} // namespace bl

#endif
