#ifndef BLIB_SCRIPTS_VALUE_HPP
#define BLIB_SCRIPTS_VALUE_HPP

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

/**
 * @brief Represents a Value in a script. Can be a temp value or from a SymbolTable
 * @ingroup Scripts
 *
 */
class Value {
public:
    typedef std::shared_ptr<Value> Ptr;
    typedef std::shared_ptr<const Value> CPtr;

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

    typedef std::vector<Ptr> Array;
    typedef std::weak_ptr<Value> Ref;
    typedef std::weak_ptr<const Value> CRef;

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

    Value(const Value& cpy);
    Value& operator=(const Value&);

    /**
     * @brief Makes Bool type
     *
     */
    void makeBool(bool val);

    /**
     * @brief Makes Numeric type
     *
     */
    Value& operator=(float num);
    Value(float num);

    /**
     * @brief Makes String type
     *
     */
    Value& operator=(const std::string& str);
    Value(const std::string& str);

    /**
     * @brief Makes Array type
     *
     */
    Value& operator=(const Array& array);
    Value(const Array& array);
    Value& operator=(const std::vector<Value>& array);
    Value(const std::vector<Value>& array);

    /**
     * @brief Makes Ref type
     *
     */
    Value& operator=(Ref ref);
    Value(Ref ref);

    /**
     * @brief Make Function type
     *
     */
    Value& operator=(const Function& func);
    Value(const Function& func);

    /**
     * @brief Returns the current type of this Value
     *
     */
    Type getType() const;

    /**
     * @brief Get the As Bool type
     *
     * @return The value as a boolean
     */
    bool getAsBool() const;

    /**
     * @brief Get the As Numeric value
     *
     * @return float The value or 0 if not a String
     */
    float getAsNum() const;

    /**
     * @brief Get the As String value
     *
     * @return std::string The value or "ERROR" if not String type
     */
    std::string getAsString() const;

    /**
     * @brief Get the As Array value
     *
     * @return Array An Array of Values or empty
     */
    Array getAsArray() const;

    /**
     * @brief Get the As Ref value
     *
     * @return Ref A reference to the Value this references, or nullptr
     */
    Ref getAsRef();

    /**
     * @brief Dereferences the value and returns the pointed to value. Recursively follows
     *        references
     *
     * @return Value& The referenced value, or this value if not a reference
     */
    Value& deref();

    /**
     * @brief Dereferences the value and returns the pointed to value. Recursively follows
     *        references. Const version
     *
     * @return Value& The referenced value, or this value if not a reference
     */
    const Value& deref() const;

    /**
     * @brief Get the As CRef value
     *
     * @return Ref A reference to the Value this references, or nullptr
     */
    CRef getAsRef() const;

    /**
     * @brief Get the As Function type
     *
     * @return Function The Function value
     */
    Function getAsFunction() const;

    /**
     * @brief Returns the given Property. nullptr if none
     *
     * @param name Name of the property to access
     * @param create If true, creates the property if it doesn't exist
     */
    Ptr getProperty(const std::string& name, bool create = false);

    /**
     * @brief Const accessor for properties
     *
     * @param name Name of the property to access
     */
    CPtr getProperty(const std::string& name) const;

    /**
     * @brief Assigns a Value to the given property name
     *
     * @param name Name of the property to modify
     * @param value The value to assign it to
     * @return True if the property could be set, false otherwise
     */
    bool setProperty(const std::string& name, const Value& value);

    /**
     * @brief Returns all properties on this value
     *
     */
    const std::unordered_map<std::string, Ptr>& allProperties() const;

private:
    typedef std::variant<bool, float, std::string, Array, Ref, Function> TData;
    typedef Value (Value::*Builtin)(const std::vector<Value>&);
    static const std::unordered_map<std::string, Builtin> builtins;

    Type type;
    std::unique_ptr<TData> value;
    std::unordered_map<std::string, Ptr> properties;

    void resetProps();

    // built-ins for arrays
    Value clear(const std::vector<Value>& args);
    Value append(const std::vector<Value>& args);
    Value resize(const std::vector<Value>& args);
    Value insert(const std::vector<Value>& args);
    Value erase(const std::vector<Value>& args);
    Value find(const std::vector<Value>& args);

    // built-ins for properties
    Value keys(const std::vector<Value>& args);
    Value at(const std::vector<Value>& args);
};

} // namespace script
} // namespace bl

#endif
