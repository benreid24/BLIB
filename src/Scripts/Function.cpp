#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/Value.hpp>

namespace bl
{
namespace scripts
{
Function::Function()
: data(CustomCB(nullptr)) {}

Function::Function(parser::Node::Ptr tree)
: data(tree) {}

Function::Function(CustomCB cb)
: data(cb) {}

bool Function::operator==(const Function& f) const {
    if (data.index() != f.data.index()) return false;
    if (data.index() == 0) {
        const CustomCB* c1 = std::get_if<CustomCB>(&data);
        const CustomCB* c2 = std::get_if<CustomCB>(&f.data);
        return c1 == c2;
    }
    else {
        const parser::Node::Ptr& r1 = *std::get_if<parser::Node::Ptr>(&data);
        const parser::Node::Ptr& r2 = *std::get_if<parser::Node::Ptr>(&f.data);
        return r1.get() == r2.get();
    }
}

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