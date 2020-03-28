#ifndef BLIB_SCRIPTS_VALUE_HPP
#define BLIB_SCRIPTS_VALUE_HPP

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace bl
{
namespace scripts
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

    enum Type { TVoid, TBool, TString, TNumeric, TArray, TFunction, TRef };

    typedef std::vector<Value> Array;
    typedef std::weak_ptr<Value> Ref;
    typedef std::weak_ptr<const Value> CRef;

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
     * @brief Returns the given Property. TVoid if none
     *
     * @param name Name of the property to access
     */
    Value getProperty(const std::string& name) const;

    /**
     * @brief Assigns a Value to the given property name
     *
     * @param name Name of the property to modify
     * @param value The value to assign it to
     * @return True if the property could be set, false otherwise
     */
    bool setProperty(const std::string& name, const Value& value);

private:
    // TODO - change array and proprties to use Ptr for Ref to work. figure out length. Maybe
    // have properties remember their parent Value
    typedef std::variant<bool, float, std::string, Array, Ref, Function> TData;

    Type type;
    std::unique_ptr<TData> value;
    std::map<std::string, Value> properties;

    void resetProps();
};

} // namespace scripts
} // namespace bl

#endif