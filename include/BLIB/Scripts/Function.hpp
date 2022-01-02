#ifndef BLIB_SCRIPTS_FUNCTION_HPP
#define BLIB_SCRIPTS_FUNCTION_HPP

#include <BLIB/Parser/Node.hpp>
#include <BLIB/Scripts/Error.hpp>
#include <BLIB/Scripts/Value.hpp>

#include <functional>
#include <optional>
#include <variant>
#include <vector>

namespace bl
{
namespace script
{
class SymbolTable;

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
    typedef std::function<Value(SymbolTable&, const std::vector<Value>&)> CustomCB;

    /**
     * @brief Creates an empty, uncallable function
     *
     */
    Function();

    /**
     * @brief Construct a new Function from a subset of a valid parse tree
     *
     * @param tree The root node of type FDef
     */
    Function(parser::Node::Ptr fdef);

    /**
     * @brief Construct a new Function from a user defined function
     *
     * @param userFunction Callback to a custom function
     */
    Function(CustomCB userFunction);

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

    /**
     * @brief Helper function to validate the number and type of arguments passed into functions
     *
     * @tparam Types The order and number of argument types expected
     * @param func The name of the function for error reporting
     * @param args The arguments passed in
     */
    template<Value::Type... Types>
    static void validateArgs(const std::string& func, const std::vector<Value>& args);

private:
    std::variant<CustomCB, parser::Node::Ptr> data;
    std::optional<std::vector<std::string>> params;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<Value::Type... Types>
void Function::validateArgs(const std::string& func, const std::vector<Value>& args) {
    const Value::Type types[] = {Types...};
    const unsigned int nargs  = sizeof...(Types);

    if (args.size() != nargs)
        throw Error(func + "() takes " + std::to_string(nargs) + " arguments");
    for (unsigned int i = 0; i < nargs; ++i) {
        const Value& v = args[i].deref();
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
