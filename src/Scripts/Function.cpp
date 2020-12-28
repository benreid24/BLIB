#include <BENG/Scripts/Function.hpp>

#include <BENG/Scripts/Error.hpp>
#include <BENG/Scripts/SymbolTable.hpp>
#include <BENG/Scripts/Value.hpp>
#include <Scripts/Parser.hpp>
#include <Scripts/ScriptImpl.hpp>

namespace bg
{
namespace scripts
{
namespace
{
std::vector<std::string> parseParams(parser::Node::Ptr fhead);
} // namespace

Function::Function()
: data(CustomCB(nullptr)) {}

Function::Function(parser::Node::Ptr tree)
: data(tree) {
    using G                       = Parser::Grammar;
    const parser::Node::Ptr& root = *std::get_if<parser::Node::Ptr>(&data);
    if (root->children.size() == 2) {
        if (root->children[0]->type == G::FHead)
            params = parseParams(root->children[0]);
        else
            throw Error("Internal error: Invalid FHead in FDef", root);
    }
    else
        throw Error("Internal error: Invalid FDef children");
}

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
        using G                       = Parser::Grammar;
        const parser::Node::Ptr& root = *std::get_if<parser::Node::Ptr>(&data);
        if (root->type != G::FDef) throw Error("Internal error: Expected FDef", root);
        if (root->children.size() != 2)
            throw Error("Internal error: Invalid FDef children", root);
        if (!params.has_value()) throw Error("Internal error: Null param list", root);
        const std::vector<std::string>& plist = params.value();

        if (plist.size() != args.size()) {
            throw Error("Function expects " + std::to_string(plist.size()) + " arguments, " +
                            std::to_string(args.size()) + " passed",
                        root);
        }
        table.pushFrame();
        for (unsigned int i = 0; i < args.size(); ++i) { table.set(plist[i], args[i], true); }
        const std::optional<Value> ret =
            ScriptImpl::runStatementList(root->children[1], table);
        table.popFrame();
        return ret.value_or(Value());
    }
}

namespace
{
std::vector<std::string> recurseParams(parser::Node::Ptr plist) {
    using G = Parser::Grammar;
    if (plist->type != G::ParamList) throw Error("Internal error: Expected ParamList", plist);
    if (plist->children.size() != 3)
        throw Error("Internal error: Invalid ParamList children", plist);

    if (plist->children[0]->type == G::ParamList) {
        if (plist->children[2]->type != G::Id)
            throw Error("Internal error: Expected Id in ParamList", plist->children[2]);
        std::vector<std::string> params = recurseParams(plist->children[0]);
        params.push_back(plist->children[2]->data);
        return params;
    }
    else if (plist->children[0]->type == G::Id && plist->children[2]->type == G::Id)
        return {plist->children[0]->data, plist->children[2]->data};
    throw Error("Internal error: Invalid ParamList children types", plist);
}

std::vector<std::string> parseParams(parser::Node::Ptr fhead) {
    using G = Parser::Grammar;
    if (fhead->children.size() == 3) return {};
    if (fhead->children.size() != 4)
        throw Error("Internal error: Invalid FHead children", fhead);

    if (fhead->children[2]->type == G::Id)
        return {fhead->children[2]->data};
    else if (fhead->children[2]->type == G::ParamList) {
        return recurseParams(fhead->children[2]);
    }
    throw Error("Internal error: Invalid FFead paramlist", fhead->children[2]);
}
} // namespace

} // namespace scripts
} // namespace bg