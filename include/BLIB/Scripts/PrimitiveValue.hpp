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
        TFloat      = 0x1 << 2,
        TInteger    = 0x1 << 3,
        TArray      = 0x1 << 4,
        TFunction   = 0x1 << 5,
        TRef        = 0x1 << 6,
        TNumeric    = TInteger | TFloat,
        TAnyValue   = TBool | TString | TFloat | TInteger | TArray | TFunction | TRef,
        TAny        = TAnyValue | TVoid,
        _TYPE_COUNT = 7 // excludes void
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
    template<typename T,
             class = std::enable_if_t<std::is_integral<T>::value || std::is_enum<T>::value>>
    PrimitiveValue& operator=(T intOrBool) {
        if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            type = TBool;
            value.emplace<bool>(intOrBool);
        }
        else {
            type = TInteger;
            value.emplace<long>(static_cast<long>(intOrBool));
        }
        return *this;
    }

    /**
     * @brief Makes integer or bool type based on parameter type
     *
     * @tparam T The type
     * @param intOrBool The int or bool value
     */
    template<typename T,
             class = std::enable_if_t<std::is_integral<T>::value || std::is_enum<T>::value>>
    PrimitiveValue(T intOrBool) {
        if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            type = TBool;
            value.emplace<bool>(intOrBool);
        }
        else {
            type = TInteger;
            value.emplace<long>(static_cast<long>(intOrBool));
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
     * @brief Get the value as Bool type. Performs implicit conversion
     *
     * @param depth The current stack depth. Used to validate references
     * @return The value as a boolean
     */
    bool getAsBool() const;

    /**
     * @brief Get the value as Float value
     *
     * @return float The value or 0 if not a float
     */
    float getAsFloat() const;

    /**
     * @brief Returns the contained integer or float as a float
     *
     * @return float The contained numeric value
     */
    float getNumAsFloat() const;

    /**
     * @brief Get the value as Integer value
     *
     * @return float The value or 0 if not a float
     */
    long getAsInt() const;

    /**
     * @brief Returns the contained integer or float as an int
     *
     * @return long The contained numeric value
     */
    long getNumAsInt() const;

    /**
     * @brief Get the value as String value
     *
     * @return std::string The value or "ERROR" if not String type
     */
    const std::string& getAsString() const;

    /**
     * @brief Get the value as ArrayValue value
     *
     * @return ArrayValue An ArrayValue of Values or empty
     */
    const ArrayValue& getAsArray() const;

    /**
     * @brief Get the value as ArrayValue value
     *
     * @return ArrayValue An ArrayValue of Values or empty
     */
    ArrayValue& getAsArray();

    /**
     * @brief Get the value as ReferenceValue value
     *
     * @return ReferenceValue A reference to the PrimitiveValue this references, or nullptr
     */
    const ReferenceValue& getAsRef() const;

    /**
     * @brief Get the value as ReferenceValue value
     *
     * @return ReferenceValue A reference to the PrimitiveValue this references, or nullptr
     */
    ReferenceValue& getAsRef();

    /**
     * @brief Get the value as Function type
     *
     * @return Function The Function value
     */
    const Function& getAsFunction() const;

private:
    struct Empty {};
    using TData =
        std::variant<Empty, bool, long, float, std::string, ArrayValue, ReferenceValue, Function>;

    Type type;
    TData value;
};

} // namespace script
} // namespace bl

inline constexpr bl::script::PrimitiveValue::Type operator|(bl::script::PrimitiveValue::Type l,
                                                            bl::script::PrimitiveValue::Type r) {
    return static_cast<bl::script::PrimitiveValue::Type>(static_cast<std::uint8_t>(l) |
                                                         static_cast<std::uint8_t>(r));
}

#endif
