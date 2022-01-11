#ifndef BLIB_SCRIPTS_FUNCTION_HPP
#define BLIB_SCRIPTS_FUNCTION_HPP

#include <BLIB/Parser/Node.hpp>
#include <BLIB/Scripts/Error.hpp>

#include <functional>
#include <optional>
#include <variant>
#include <vector>

namespace bl
{
namespace script
{
class Value;
class SymbolTable;
class PrimitiveValue;

/**
 * @brief Utility class for functions defined in scripts themselves
 * @ingroup Scripts
 *
 */
class Function {
public:
    /**
     * @brief Callback type for custom functions. Can be used to provide a built in library
     *        Receives the SymbolTable and list of calling parameters. Can throw an Error
     *        object on error
     *
     */
    using CustomCB = std::function<Value(SymbolTable&, const std::vector<Value>&)>;

    /**
     * @brief Creates an empty, uncallable function
     *
     */
    Function();

    /**
     * @brief Copy constructor
     *
     */
    Function(const Function&) = default;

    /**
     * @brief Copy operator
     *
     */
    Function& operator=(const Function&) = default;

    /**
     * @brief Construct a new Function from a subset of a valid parse tree
     *
     * @param tree The root node of type FDef
     */
    Function(const parser::Node::Ptr& fdef);

    /**
     * @brief Construct a new Function from a user defined function
     *
     * @param userFunction Callback to a custom function
     */
    Function(const CustomCB& userFunction);

    /**
     * @brief Comapres against another function
     *
     */
    bool operator==(const Function& func) const;

    /**
     * @brief Calls the function. Error thrown on error
     *
     * @param table Modifiable reference to the SymbolTable
     * @param args The arguments it was called with
     * @return Value The result of the function
     */
    Value operator()(SymbolTable& table, const std::vector<Value>& args) const;

private:
    std::variant<CustomCB, parser::Node::Ptr> data;
    std::optional<std::vector<std::string>> params;
};

} // namespace script
} // namespace bl

#endif
