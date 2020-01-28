#ifndef BLIB_SCRIPTS_VALUE_HPP
#define BLIB_SCRIPTS_VALUE_HPP

#include <BLIB/Parser/Node.hpp>
#include <BLIB/Scripts/Function.hpp>

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace bl
{
namespace scripts
{
/**
 * @brief Represents a Value in a script. Can be a temp value or from a SymbolTable
 * @ingroup Scripts
 *
 */
class Value {
public:
    typedef std::shared_ptr<Value> Ptr;

    enum Type { TVoid, TString, TNumeric, TArray, TFunction, TRef };

    typedef std::vector<Value> Array;
    typedef std::weak_ptr<Value> Ref;

    /**
     * @brief Makes Void type
     *
     */
    Value();

    /**
     * @brief Makes Numeric type
     *
     */
    Value& operator=(float num);

    /**
     * @brief Makes String type
     *
     */
    Value& operator=(const std::string& str);

    /**
     * @brief Makes Array type
     *
     */
    Value& operator=(const Array& array);

    /**
     * @brief Makes Ref type
     *
     */
    Value& operator=(Ref ref);

    /**
     * @brief Make Function type
     *
     */
    Value& operator=(const Function& func);

    /**
     * @brief Returns the current type of this Value
     *
     */
    Type type() const;

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

    Array getAsArray() const;

    Ref getAsRef() const;

private:
    Type type;
    std::variant<float, std::string, Array, Ref, Function> value;
};

} // namespace scripts
} // namespace bl

#endif