#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/Value.hpp>

namespace bl
{
namespace scripts
{
Function::Function(parser::Node::Ptr tree)
: data(tree) {}

Function::Function(CustomCB cb)
: data(cb) {}

Value Function::operator()(SymbolTable& table, const std::vector<Value>& args) const {
    const CustomCB* cbp = std::get_if<CustomCB>(&data);
    if (cbp) {
        const CustomCB& cb = *cbp;
        return cb(table, args);
    }
    else {
        const parser::Node::Ptr& root = *std::get_if<parser::Node::Ptr>(&data);
        // TODO - execute nodes
        return Value();
    }
}

} // namespace scripts
} // namespace bl