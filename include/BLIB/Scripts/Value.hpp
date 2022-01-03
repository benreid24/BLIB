#ifndef BLIB_SCRIPTS_VALUE_HPP
#define BLIB_SCRIPTS_VALUE_HPP

#include <BLIB/Scripts/Function.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace bl
{
namespace script
{
class Function;
class SymbolTable;

/**
 * @brief Represents a Value in a script. Can be a temp value or from a SymbolTable
 * @ingroup Scripts
 *
 */
class Value {
public:
    typedef Value (Value::*Builtin)(SymbolTable&, const std::vector<Value>&);
    using Array = std::vector<Value>;

    static const std::unordered_map<std::string, Builtin> Builtins;

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
     * @brief Represents a reference to a value
     *
     * @ingroup Scripts
     *
     */
    struct Ref {
        /// The value pointed to. May be nullptr or invalid
        Value* value;

        /// The stack depth of the referred value. If lower than current depth the Ref is invalid
        int depth;

        /**
         * @brief Construct a new Ref object
         *
         */
        Ref()
        : value(nullptr)
        , depth(0) {}

        /**
         * @brief Construct a new Ref object
         *
         * @param v Value to refer to
         * @param d Current table depth
         */
        Ref(Value* v, int d)
        : value(v)
        , depth(d) {}
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
    Value();

    /**
     * @brief Copy constructor
     *
     */
    Value(const Value&) = default;

    /**
     * @brief Move constructor
     *
     */
    Value(Value&&) = default;

    /**
     * @brief Copy operator
     *
     */
    Value& operator=(const Value&) = default;

    /**
     * @brief Move operator
     *
     */
    Value& operator=(Value&&) = default;

    /**
     * @brief Makes integer or bool type based on parameter type
     *
     * @tparam T The type
     * @param intOrBool The int or bool value
     */
    template<typename T, class = std::enable_if_t<std::is_integral_v<T>>>
    Value& operator=(T intOrBool) {
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
    Value(T intOrBool)
    : depth(-1) {
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
    Value& operator=(float num);

    /**
     * @brief Makes Numeric type
     *
     */
    Value(float num);

    /**
     * @brief Makes String type
     *
     */
    Value& operator=(const std::string& str);

    /**
     * @brief Makes String type
     *
     */
    Value(const std::string& str);

    /**
     * @brief Makes Array type
     *
     */
    Value& operator=(const Array& array);

    /**
     * @brief Makes Array type
     *
     */
    Value(const Array& array);

    /**
     * @brief Makes Ref type
     *
     */
    Value& operator=(const Ref& ref);

    /**
     * @brief Makes Ref type
     *
     */
    Value(const Ref& ref);

    /**
     * @brief Make Function type
     *
     */
    Value& operator=(const Function& func);

    /**
     * @brief Make Function type
     *
     */
    Value(const Function& func);

    /**
     * @brief Returns the current type of this Value
     *
     */
    Type getType() const;

    /**
     * @brief Get the as Bool type. Performs implicit conversion
     *
     * @param depth The current stack depth. Used to validate references
     * @return The value as a boolean
     */
    bool getAsBool(int depth = 0) const;

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
     * @brief Get the as Array value
     *
     * @return Array An Array of Values or empty
     */
    const Array& getAsArray() const;

    /**
     * @brief Get the as Array value
     *
     * @return Array An Array of Values or empty
     */
    Array& getAsArray();

    /**
     * @brief Get the as Ref value
     *
     * @return Ref A reference to the Value this references, or nullptr
     */
    const Ref& getAsRef() const;

    /**
     * @brief Dereferences the value and returns the pointed to value. Recursively follows
     *        references
     *
     * @param depth The current stack depth. Used to validate references
     * @return Value& The referenced value, or this value if not a reference
     */
    Value& deref(int depth);

    /**
     * @brief Dereferences the value and returns the pointed to value. Recursively follows
     *        references. Const version
     *
     * @param depth The current stack depth. Used to validate references
     * @return Value& The referenced value, or this value if not a reference
     */
    const Value& deref(int depth) const;

    /**
     * @brief Get the as Function type
     *
     * @return Function The Function value
     */
    const Function& getAsFunction() const;

    /**
     * @brief Helper function to validate the number and type of arguments passed into functions
     *
     * @tparam Types The order and number of argument types expected
     * @param func The name of the function for error reporting
     * @param args The arguments passed in
     * @param depth The current stack depth. Used to validate references
     */
    template<Value::Type... Types>
    static void validateArgs(const std::string& func, const std::vector<Value>& args, int depth);

private:
    struct Empty {};
    using TData = std::variant<Empty, bool, float, std::string, Array, Ref, Function>;

    Type type;
    TData value;
    int depth;

    // built-ins for arrays
    Value clear(SymbolTable& table, const std::vector<Value>& args);
    Value append(SymbolTable& table, const std::vector<Value>& args);
    Value resize(SymbolTable& table, const std::vector<Value>& args);
    Value insert(SymbolTable& table, const std::vector<Value>& args);
    Value erase(SymbolTable& table, const std::vector<Value>& args);
    Value find(SymbolTable& table, const std::vector<Value>& args);

    // built-ins for properties
    Value keys(SymbolTable& table, const std::vector<Value>& args);
    Value at(SymbolTable& table, const std::vector<Value>& args);

    friend class SymbolTable;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<Value::Type... Types>
void Value::validateArgs(const std::string& func, const std::vector<Value>& args, int d) {
    const Value::Type types[] = {Types...};
    const unsigned int nargs  = sizeof...(Types);

    if (args.size() != nargs)
        throw Error(func + "() takes " + std::to_string(nargs) + " arguments");
    for (unsigned int i = 0; i < nargs; ++i) {
        const Value& v = args[i].deref(d);
        if ((v.getType() & types[i]) == 0) {
            throw Error(func + "() argument " + std::to_string(i) + " must be a " +
                        Value::typeToString(types[i]) + " but " + Value::typeToString(v.getType()) +
                        " was passed");
        }
    }
}

} // namespace script
} // namespace bl

#endif
