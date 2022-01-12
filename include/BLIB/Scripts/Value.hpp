#ifndef BLIB_SCRIPTS_VALUE_HPP
#define BLIB_SCRIPTS_VALUE_HPP

#include <BLIB/Scripts/PrimitiveValue.hpp>
#include <BLIB/Scripts/ReferenceValue.hpp>
#include <string>
#include <unordered_map>

namespace bl
{
namespace script
{
class SymbolTable;

/**
 * @brief Main representation of a value in scripts
 *
 * @ingroup Scripts
 *
 */
class Value : public std::enable_shared_from_this<Value> {
public:
    /**
     * @brief Makes a void type value
     *
     */
    Value() = default;

    /**
     * @brief Copies a value
     *
     */
    Value(const Value& value) = default;

    /**
     * @brief Moves from a value
     *
     */
    Value(Value&& value) = default;

    /**
     * @brief Constructs a primitive value in-place
     *
     * @tparam TArgs Arguments to the PrimitiveValue
     * @param args The arguments to construct with
     */
    template<typename... TArgs>
    Value(TArgs... args);

    /**
     * @brief Create from a primitive value
     *
     */
    Value(const PrimitiveValue& value);

    /**
     * @brief Create from a primitive value
     *
     */
    Value(PrimitiveValue&& value);

    /**
     * @brief Copy the value
     *
     */
    Value& operator=(const Value&) = default;

    /**
     * @brief Copy the value
     *
     */
    Value& operator=(Value&&) = default;

    /**
     * @brief Create from a primitive value
     *
     */
    Value& operator=(const PrimitiveValue& value);

    /**
     * @brief Create from a primitive value
     *
     */
    Value& operator=(PrimitiveValue&& value);

    /**
     * @brief Assignment operator for primitive values
     *
     * @tparam TArgs The assignment argument type
     * @param arg The argument to the primitive value assignment operator
     * @return Value& This value
     */
    template<typename TArg>
    Value& operator=(TArg args);

    /**
     * @brief Access the primitive value of this value
     *
     */
    PrimitiveValue& value();

    /**
     * @brief Access the primitive value of this value
     *
     */
    const PrimitiveValue& value() const;

    /**
     * @brief Gets a properly constructed reference to this value
     *
     */
    ReferenceValue getRef() const;

    /**
     * @brief Access the value being referred to, or this one if not a reference
     *
     */
    const Value& deref() const;

    /**
     * @brief Access the value being referred to, or this one if not a reference
     *
     */
    Value& deref();

    /**
     * @brief Get a property on this value. Will throw an Error if the property does not exist
     *
     * @param name The name of the property to get
     * @return ReferenceValue A reference to the property
     */
    ReferenceValue getProperty(const std::string& name);

    /**
     * @brief Get a property on this value. Will throw an Error if the property does not exist
     *
     * @param name The name of the property to get
     * @return ReferenceValue A reference to the property
     */
    ReferenceValue getProperty(const std::string& name) const;

    /**
     * @brief Sets a property on this value. Will throw an Error if the property is not writable
     *
     * @param name The name of the property to set
     * @param value The value of the property to set
     */
    void setProperty(const std::string& name, const ReferenceValue& value);

    /**
     * @brief Helper function to validate the number and type of arguments passed into functions
     *
     * @tparam Types The order and number of argument types expected
     * @param func The name of the function for error reporting
     * @param args The arguments passed in
     */
    template<PrimitiveValue::Type... Types>
    static void validateArgs(const std::string& func, const std::vector<Value>& args);

    /**
     * @brief Returns all properties on this value
     * 
     */
    const std::unordered_map<std::string, ReferenceValue>& allProperties() const;

private:
    PrimitiveValue _value;
    std::unordered_map<std::string, ReferenceValue> properties;
    // TODO - consider using std::unique_ptr and only constructing on set

    typedef Value (Value::*Builtin)();
    static const std::unordered_map<std::string, Builtin> builtins;

    // built-ins for arrays
    Value clear(SymbolTable& table, const std::vector<Value>& args);
    Value clearValue();
    Value append(SymbolTable& table, const std::vector<Value>& args);
    Value appendValue();
    Value resize(SymbolTable& table, const std::vector<Value>& args);
    Value resizeValue();
    Value insert(SymbolTable& table, const std::vector<Value>& args);
    Value insertValue();
    Value erase(SymbolTable& table, const std::vector<Value>& args);
    Value eraseValue();
    Value find(SymbolTable& table, const std::vector<Value>& args);
    Value findValue();

    // built-ins for properties
    Value keys(SymbolTable& table, const std::vector<Value>& args);
    Value keysValue();
    Value at(SymbolTable& table, const std::vector<Value>& args);
    Value atValue();

    // built-ins for arrays
    Value lengthValue();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... TArgs>
Value::Value(TArgs... args)
: _value(args...) {}

template<typename TArg>
Value& Value::operator=(TArg arg) {
    _value = arg;
    return *this;
}

template<PrimitiveValue::Type... Types>
void Value::validateArgs(const std::string& func, const std::vector<Value>& args) {
    constexpr PrimitiveValue::Type types[] = {Types...};
    constexpr unsigned int nargs           = sizeof...(Types);

    if (args.size() != nargs)
        throw Error(func + "() takes " + std::to_string(nargs) + " arguments");
    for (unsigned int i = 0; i < nargs; ++i) {
        const Value& v = args[i].value().deref();
        if ((v.value().getType() & types[i]) == 0) {
            throw Error(func + "() argument " + std::to_string(i) + " must be a " +
                        PrimitiveValue::typeToString(types[i]) + " but " +
                        PrimitiveValue::typeToString(v.value().getType()) + " was passed");
        }
    }
}

} // namespace script
} // namespace bl

#endif
