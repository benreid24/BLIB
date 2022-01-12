#include <Scripts/ScriptImpl.hpp>

#include <BLIB/Logging.hpp>
#include <Scripts/Parser.hpp>
#include <cmath>

namespace bl
{
namespace script
{
using Symbol = const parser::Node::Ptr&;
using G      = Parser::Grammar;
namespace
{
struct Ops {
    static PrimitiveValue Or(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue And(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue Neg(const PrimitiveValue& val);
    static PrimitiveValue Eq(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue Ne(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue Gt(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue Ge(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue Lt(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue Le(const PrimitiveValue& lhs, const PrimitiveValue& rhs);
    static PrimitiveValue Add(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node);
    static PrimitiveValue Sub(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node);
    static PrimitiveValue Mult(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node);
    static PrimitiveValue Div(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node);
    static PrimitiveValue Exp(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node);
};

void evalList(Symbol node, SymbolTable& table, std::vector<Value>& result);
std::optional<Value> runElifChain(Symbol node, SymbolTable& table, bool& ran);

PrimitiveValue evalOrGrp(Symbol node, SymbolTable& table);
PrimitiveValue evalAndGrp(Symbol node, SymbolTable& table);
PrimitiveValue evalNegGrp(Symbol node, SymbolTable& table);
PrimitiveValue evalCmp(Symbol node, SymbolTable& table);
PrimitiveValue evalSum(Symbol node, SymbolTable& table);
PrimitiveValue evalProd(Symbol node, SymbolTable& table);
PrimitiveValue evalExp(Symbol node, SymbolTable& table);
Value evalTVal(Symbol node, SymbolTable& table);
ReferenceValue evalRVal(Symbol node, SymbolTable& table, bool create = false);

} // namespace

PrimitiveValue ScriptImpl::computeValue(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Exit();

    if (node->type != G::Value)
        throw Error("Internal error: computeValue called on non Value: " +
                        std::to_string(node->type),
                    node);
    if (node->children.size() != 1)
        throw Error("Internal error: Value node has invalid child", node);
    if (node->children[0]->type != G::OrGrp)
        throw Error("Internal error: Value node has invalid child", node);
    return evalOrGrp(node->children[0], table);
}

Value ScriptImpl::runFunction(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Exit();

    if (node->type != G::Call) throw Error("Internal error: runFunction called on non Call", node);
    if (node->children.size() != 2 && node->children.size() != 3)
        throw Error("Internal error: Invalid Call children", node);

    std::vector<Value> params;
    ReferenceValue ret = evalRVal(node->children[0], table);
    const Value& func  = ret.deref();
    if (func.value().getType() != PrimitiveValue::TFunction)
        throw Error("Cannot call non-function type", node->children[0]);
    if (node->children.size() == 2) {
        const auto& c = node->children[1];
        if (c->children.size() != 3) throw Error("Internal error: Invalid ArgList", c);
        evalList(c->children[1], table, params);
    }

    return func.value().getAsFunction()(table, params);
}

std::optional<Value> ScriptImpl::runStatement(Symbol n, SymbolTable& table) {
    if (table.killed()) throw Exit();
    if (n->children.empty()) throw Error("Internal error: Invalid Statement", n);

    const auto& node = n->children[0];
    switch (node->type) {
    case G::Ret:
        if (node->children.size() == 2)
            return Value();
        else if (node->children.size() == 3)
            return computeValue(node->children[1], table);
        throw Error("Internal error: Invalid Ret children", node);

    case G::Call:
        runFunction(node, table);
        return {};

    case G::Conditional:
        return runConditional(node, table);

    case G::Loop:
        return runLoop(node, table);

    case G::ForLoop:
        return runForLoop(node, table);

    case G::Assignment: {
        if (node->children.size() != 4)
            throw Error("Internal error: Invalid Assignment children", node);
        ReferenceValue rv = evalRVal(node->children[0], table, true);
        if (node->children[2]->type == G::Ref) {
            if (node->children[2]->children.size() != 2)
                throw Error("Internal error: Invalid Ref children", node->children[2]);
            rv.deref().value() = evalRVal(node->children[2]->children[1], table);
        }
        else if (node->children[2]->type == G::Value) {
            rv.deref() = computeValue(node, table);
        }
        else
            throw Error("Internal error: Invalid Assignment children", node);
        break;
    }

    case G::FDef: {
        if (node->children.size() != 2) throw Error("Internal error: Invalid FDef children", node);
        Symbol fhead = node->children[0];
        if (fhead->children.size() < 3)
            throw Error("Internal error: Invalid FHead children", fhead);
        Symbol fname = fhead->children[0];
        if (fname->children.size() != 2)
            throw Error("Internal error: Invalid FName children", fname);
        if (fname->children[1]->type != G::Id)
            throw Error("Internal error: Invalid FName children", fname->children[1]);
        table.set(fname->children[1]->data, Value(Function(node)));
        return {};
    }

    default:
        throw Error("Internal error: Invalid Statement children", node);
    }

    return {};
}

std::optional<Value> ScriptImpl::runStatementList(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Exit();

    class ScopeGuard {
    public:
        ScopeGuard(Symbol node, SymbolTable& table)
        : table(table)
        , pop(node->type == G::StmtBlock) {
            if (pop) table.pushFrame();
        }

        ~ScopeGuard() {
            if (pop) table.popFrame();
        }

    private:
        SymbolTable& table;
        const bool pop;
    } guard(node, table);

    try {
        switch (node->type) {
        case G::StmtBlock:
            if (node->children.size() != 3)
                throw Error("Internal error: Invalid StmtBlock children", node);
            return runStatementList(node->children[1], table);

        case G::Statement:
            return runStatement(node, table);

        case G::StmtList:
            if (node->children.size() == 2) {
                const std::optional<Value> r = runStatementList(node->children[0], table);
                if (r.has_value()) return r;
                return runStatement(node->children[1], table);
            }
            else if (node->children.size() == 1)
                return runStatement(node->children[0], table);
            throw Error("Internal error: Invalid StmtList children", node);

        default:
            throw Error("Internal error: Expected Statement, StmtBlock, or StmtList", node);
        }
    } catch (const Error& err) { throw Error("Called from here", node, err); }
}

std::optional<Value> ScriptImpl::runLoop(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Exit();
    if (node->type != G::Loop) throw Error("Internal error: Expected Loop", node);
    if (node->children.size() != 2) throw Error("Internal error: Invalid Loop children", node);

    Symbol head = node->children[0];
    if (head->type != G::LoopHead) throw Error("Internal error: Expected LoopHead", head);
    if (head->children.size() != 2) throw Error("Internal error: Invalid LoopHead children", head);
    Symbol pgroup = head->children[1];

    while (evaluateCond(pgroup, table)) {
        if (table.killed()) throw Exit();
        const std::optional<Value> r = runStatementList(node->children[1], table);
        if (r.has_value()) return r;
    }
    return {};
}

std::optional<Value> ScriptImpl::runForLoop(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Exit();
    if (node->type != G::ForLoop) throw Error("Internal error: Expected ForLoop", node);
    if (node->children.size() != 2) throw Error("Internal error: Invalid ForLoop children", node);

    Symbol head = node->children[0];
    if (head->type != G::ForHead) throw Error("Internal error: Invalid ForLoop child", head);
    if (head->children.size() != 6) throw Error("Invalid ForHead children", head);
    if (head->children[2]->type != G::Id) throw Error("Invalid ForHead child", head->children[2]);

    const Value arr = computeValue(head->children[4], table); // TODO - consider switching to RValue
    const std::string& iter = head->children[2]->data;
    if (arr.value().getType() != PrimitiveValue::TArray)
        throw Error("For loop can only iterate over Array type", head->children[4]);
    for (const Value& v : arr.value().getAsArray()) {
        if (table.killed()) throw Exit();
        table.set(iter, v, true);
        std::optional<Value> r = runStatementList(node->children[1], table);
        if (r.has_value()) return r;
    }
    return {};
}

std::optional<Value> ScriptImpl::runConditional(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Exit();
    if (node->type != G::Conditional) throw Error("Internal error: Expected Conditional", node);
    if (node->children.size() != 1)
        throw Error("Internal error: Invalid Conditional children", node);

    bool ran = false;
    std::optional<Value> r;
    switch (node->children[0]->type) {
    case G::ElifChain:
        return runElifChain(node->children[0], table, ran);
    case G::ElseCond: {
        Symbol ec = node->children[0];
        if (ec->children.size() != 2) throw Error("Internal error: Invalid ElseCond children", ec);
        r = runElifChain(ec->children[0], table, ran);
        if (ran) return r;
        if (ec->children[1]->type != G::ElseBlock)
            throw Error("Internal error: Invalid ElseCond children", node);
        Symbol el = node->children[1];
        if (el->children.size() != 2) throw Error("Internal error: Invalid ElseBlock children", el);
        return runStatementList(el->children[1], table);
    }
    default:
        throw Error("Internal error: Invalid Conditional children", node->children[0]);
    }
}

bool ScriptImpl::equals(const Value& left, const Value& right) {
    return Ops::Eq(left.value(), right.value()).getAsBool();
}

bool ScriptImpl::evaluateCond(Symbol node, SymbolTable& table) {
    if (node->type != G::PGroup) throw Error("Internal error: Expected PGroup", node);
    if (node->children.size() != 3) throw Error("Internal error: Invalid PGroup children", node);
    return ScriptImpl::computeValue(node->children[1], table).getAsBool();
}

namespace
{
std::optional<Value> runElifChain(Symbol node, SymbolTable& table, bool& ran) {
    if (node->type != G::ElifChain) throw Error("Internal error: Expected ElifChain", node);

    constexpr auto getCond = [](Symbol block) -> Symbol {
        if (block->children.size() != 2)
            throw Error("Internal error: Invalid If/Elif Block children", block);
        Symbol b = block->children[0];
        if (b->children.size() != 2)
            throw Error("Internal error: Invalid If/Elif Head children", b);
        return b->children[1];
    };

    if (node->children.size() == 1) {
        Symbol n = node->children[0];
        if (n->type != G::IfBlock) throw Error("Internal error: Invalid ElifChain child", n);
        if (ScriptImpl::evaluateCond(getCond(node), table)) {
            ran = true;
            if (n->children.size() != 2) throw Error("Internal error: Invalid IfBlock children", n);
            return ScriptImpl::runStatementList(n->children[1], table);
        }
        return {};
    }
    else if (node->children.size() == 2) {
        Symbol chain                 = node->children[0];
        Symbol elif                  = node->children[1];
        const std::optional<Value> r = runElifChain(chain, table, ran);
        if (ran) return r;
        if (ScriptImpl::evaluateCond(getCond(elif), table)) {
            ran = true;
            if (elif->children.size() != 2)
                throw Error("Internal error: Invalid ElifBlock children", elif);
            return ScriptImpl::runStatementList(elif->children[1], table);
        }
        return {};
    }
    throw Error("Internal error: Invalid ElifChain children ", node);
}

PrimitiveValue evalOrGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::OrGrp) throw Error("Internal error: evalOrGrp called on non OrGrp", node);
    if (node->children.size() == 1)
        return evalAndGrp(node->children[0], table);
    else if (node->children.size() == 3)
        return Ops::Or(evalOrGrp(node->children[0], table), evalAndGrp(node->children[2], table));
    throw Error("Internal error: OrGrp node has invalid child", node);
}

PrimitiveValue evalAndGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::AndGrp)
        throw Error("Internal error: evalAndGrp called on non AndGrp", node);
    if (node->children.size() == 1)
        return evalNegGrp(node->children[0], table);
    else if (node->children.size() == 3)
        return Ops::And(evalAndGrp(node->children[0], table), evalNegGrp(node->children[2], table));
    throw Error("Internal error: AndGrp node has invalid child", node);
}

PrimitiveValue evalNegGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::Negation)
        throw Error("Internal error: evalNegGrp called on non Neg", node);
    if (node->children.size() == 1)
        return evalCmp(node->children[0], table);
    else if (node->children.size() == 2)
        return Ops::Neg(evalNegGrp(node->children[1], table));
    throw Error("Internal error: Negation node has invalid child", node);
}

PrimitiveValue evalCmp(Symbol node, SymbolTable& table) {
    if (node->type != G::Cmp) throw Error("Internal error: evalCmp called on non Cmp", node);
    if (node->children.size() == 1)
        return evalSum(node->children[0], table);
    else if (node->children.size() == 3) {
        const PrimitiveValue lhs = evalSum(node->children[0], table);
        const PrimitiveValue rhs = evalSum(node->children[2], table);
        switch (node->children[1]->type) {
        case G::Eq:
            return Ops::Eq(lhs, rhs);
        case G::Ne:
            return Ops::Ne(lhs, rhs);
        case G::Gt:
            return Ops::Gt(lhs, rhs);
        case G::Ge:
            return Ops::Ge(lhs, rhs);
        case G::Lt:
            return Ops::Lt(lhs, rhs);
        case G::Le:
            return Ops::Le(lhs, rhs);
        default:
            throw Error("Internal error: Invalid operation in Cmp", node);
        }
    }
    throw Error("Internal error: OrGrp node has invalid child", node);
}

PrimitiveValue evalSum(Symbol node, SymbolTable& table) {
    if (node->type != G::Sum) throw Error("Internal error: evalSum called on non Sum", node);
    if (node->children.size() == 1)
        return evalProd(node->children[0], table);
    else if (node->children.size() == 3) {
        const PrimitiveValue lhs = evalSum(node->children[0], table);
        const PrimitiveValue rhs = evalProd(node->children[2], table);
        switch (node->children[1]->type) {
        case G::Plus:
            return Ops::Add(lhs, rhs, node);
        case G::Minus:
            return Ops::Sub(lhs, rhs, node);
        default:
            throw Error("Internal error: Invalid operation in Sum");
        }
    }
    throw Error("Internal error: Sum node has invalid child", node);
}

PrimitiveValue evalProd(Symbol node, SymbolTable& table) {
    if (node->type != G::Product)
        throw Error("Internal error: evalProd called on non Product", node);
    if (node->children.size() == 1)
        return evalExp(node->children[0], table);
    else if (node->children.size() == 3) {
        const PrimitiveValue lhs = evalProd(node->children[0], table);
        const PrimitiveValue rhs = evalExp(node->children[2], table);
        switch (node->children[1]->type) {
        case G::Mult:
            return Ops::Mult(lhs, rhs, node);
        case G::Div:
            return Ops::Div(lhs, rhs, node);
        default:
            throw Error("Internal error: Invalid operation in Product");
        }
    }
    throw Error("Internal error: Product node has invalid child", node);
}

PrimitiveValue evalExp(Symbol node, SymbolTable& table) {
    if (node->type != G::Exp) throw Error("Internal error: evalExp called on non Exp", node);
    if (node->children.size() == 1)
        return evalTVal(node->children[0], table).value();
    else if (node->children.size() == 3) {
        const PrimitiveValue lhs = evalExp(node->children[0], table);
        const PrimitiveValue rhs = evalTVal(node->children[2], table).value();
        return Ops::Exp(lhs, rhs, node);
    }
    throw Error("Internal error: Exp node has invalid child", node);
}

Value evalTVal(Symbol node, SymbolTable& table) {
    if (node->type != G::TValue) throw Error("Internal error: evalTVal called on non TValue");
    if (node->children.size() != 1) throw Error("Internal error: TValue has invalid children");

    Symbol n = node->children[0];
    switch (n->type) {
    case G::RValue:
        return evalRVal(n, table).deref();
    case G::PGroup:
        if (n->children.size() != 3) throw Error("Internal error: PGroup has invalid children", n);
        return ScriptImpl::computeValue(n->children[1], table);
    case G::NumLit:
        return PrimitiveValue(std::atof(n->data.c_str()));
    case G::StringLit:
        return PrimitiveValue(n->data);
    case G::Call:
        return ScriptImpl::runFunction(n, table);
    case G::ArrayDef: {
        if (n->children.size() != 3) throw Error("Invalid ArrayDef children", n);
        Value ret(ArrayValue{});
        evalList(n->children[1], table, ret.value().getAsArray());
        return ret;
    }
    case G::True: {
        return Value(true);
    }
    case G::False: {
        return Value(false);
    }
    case G::UNeg: {
        if (n->children.size() != 2) throw Error("Internal error: Invalid UNeg children", n);
        Value v = evalTVal(n->children[1], table);
        if (v.value().getType() != PrimitiveValue::TNumeric)
            throw Error("Right operand of unary '-' must be Numeric", n->children[1]);
        v.value() = -v.value().getAsNum();
        return v;
    }
    default:
        throw Error("Internal error: Invalid TValue child");
    }
}

ReferenceValue evalRVal(Symbol node, SymbolTable& table, bool create) {
    if (node->type != G::RValue) throw Error("Internal error: evalRVal called on non RValue");
    if (node->children.size() != 1) throw Error("Internal error: RValue has invalid children");

    Symbol n = node->children[0];
    switch (n->type) {
    case G::Id:
        return table.get(n->data, create);

    case G::Property: {
        if (n->children.size() != 3) throw Error("Internal error: Invalid Property children", n);
        ReferenceValue v = evalRVal(n->children[0], table, create);
        return v.deref().getProperty(n->children[2]->data, create);
    }

    case G::ArrayAcc: {
        if (n->children.size() != 4) throw Error("Internal error: Invalid ArrayAcc children", n);
        ReferenceValue v = evalRVal(n->children[0], table, create);
        PrimitiveValue i = ScriptImpl::computeValue(n->children[2], table);
        if (v.deref().value().getType() != PrimitiveValue::TArray)
            throw Error("Array access on non-array type", n->children[0]);
        if (i.getType() != PrimitiveValue::TNumeric)
            throw Error("Array indices must be Numeric", n->children[2]);
        unsigned int j = static_cast<unsigned int>(i.getAsNum());
        if (i.getAsNum() != j || std::floor(i.getAsNum()) != i.getAsNum())
            throw Error("Array indices must be positive integers", n->children[2]);
        if (j >= v.deref().value().getAsArray().size())
            throw Error("Array index " + std::to_string(j) + " out of bounds", n->children[2]);
        return v.deref().value().getAsArray()[j].getRef();
    }

    default:
        throw Error("Internal error: Invalid RValue child", n);
    }
}

unsigned int findListSize(Symbol list, parser::Node** bottom) {
    parser::Node* c = list.get();
    unsigned int s  = 1;
    while (c->children.size() > 1) {
        s += 1;
        c = c->children.front().get();
    }
    *bottom = c;
    return s;
}

void evalList(Symbol node, SymbolTable& table, std::vector<Value>& result) {
    if (node->type != G::ValueList) throw Error("Internal error: evalList called on non ValueList");

    parser::Node* c;
    const unsigned int n = findListSize(node, &c);
    result.reserve(n);
    while (c != node.get()) {
        result.emplace_back(ScriptImpl::computeValue(
            c->children.size() > 1 ? c->children[2] : c->children[0], table));
        c = c->parent;
    }
    result.emplace_back(node->children.size() > 1 ? node->children[2] : node->children[0]);
}

PrimitiveValue Ops::Or(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    return {lhs.deref().getAsBool() || rhs.deref().getAsBool()};
}

PrimitiveValue Ops::And(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    return {lhs.deref().getAsBool() && rhs.deref().getAsBool()};
}

PrimitiveValue Ops::Neg(const PrimitiveValue& val) { return {!val.deref().getAsBool()}; }

PrimitiveValue Ops::Eq(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != lh.getType()) { return PrimitiveValue(false); }
    bool eq = true;
    switch (lh.getType()) {
    case PrimitiveValue::TBool:
        eq = lh.getAsBool() == rh.getAsBool();
        break;
    case PrimitiveValue::TNumeric:
        eq = lh.getAsNum() == rh.getAsNum();
        break;
    case PrimitiveValue::TString:
        eq = lh.getAsString() == rh.getAsString();
        break;
    case PrimitiveValue::TArray:
        if (rh.getAsArray().size() == lh.getAsArray().size()) {
            for (unsigned int i = 0; i < rh.getAsArray().size(); ++i) {
                if (!Ops::Eq(rh.getAsArray()[i].value(), lh.getAsArray()[i].value()).getAsBool()) {
                    eq = false;
                    break;
                }
            }
        }
        else
            eq = false;
        break;
    case PrimitiveValue::TFunction:
        eq = rh.getAsFunction() == lh.getAsFunction();
        break;
    default:
        eq = false;
    }
    return PrimitiveValue(eq);
}

PrimitiveValue Ops::Ne(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    return PrimitiveValue(!Ops::Eq(rhs, lhs).getAsBool());
}

PrimitiveValue Ops::Gt(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != lh.getType()) { return PrimitiveValue(false); }
    bool gt = true;
    switch (lh.getType()) {
    case PrimitiveValue::TBool:
        gt = lh.getAsBool() > rh.getAsBool();
        break;
    case PrimitiveValue::TNumeric:
        gt = lh.getAsNum() > rh.getAsNum();
        break;
    case PrimitiveValue::TString:
        gt = !(lh.getAsString() < rh.getAsString()) && lh.getAsString() != rh.getAsString();
        break;
    case PrimitiveValue::TArray:
        gt = lh.getAsArray().size() > rh.getAsArray().size();
        break;
    default:
        gt = false;
    }
    return PrimitiveValue(gt);
}

PrimitiveValue Ops::Ge(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != lh.getType()) { return PrimitiveValue(false); }
    bool ge = true;
    switch (lh.getType()) {
    case PrimitiveValue::TBool:
        ge = lh.getAsBool() >= rh.getAsBool();
        break;
    case PrimitiveValue::TNumeric:
        ge = lh.getAsNum() >= rh.getAsNum();
        break;
    case PrimitiveValue::TString:
        ge = !(lh.getAsString() < rh.getAsString());
        break;
    case PrimitiveValue::TArray:
        ge = lh.getAsArray().size() >= rh.getAsArray().size();
        break;
    default:
        ge = false;
    }
    return PrimitiveValue(ge);
}

PrimitiveValue Ops::Lt(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != lh.getType()) { return PrimitiveValue(false); }
    bool lt = true;
    switch (lh.getType()) {
    case PrimitiveValue::TBool:
        lt = lh.getAsBool() < rh.getAsBool();
        break;
    case PrimitiveValue::TNumeric:
        lt = lh.getAsNum() < rh.getAsNum();
        break;
    case PrimitiveValue::TString:
        lt = lh.getAsString() < rh.getAsString();
        break;
    case PrimitiveValue::TArray:
        lt = lh.getAsArray().size() < rh.getAsArray().size();
        break;
    default:
        lt = false;
    }
    return PrimitiveValue(lt);
}

PrimitiveValue Ops::Le(const PrimitiveValue& lhs, const PrimitiveValue& rhs) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != lh.getType()) { return PrimitiveValue(false); }
    bool gt = true;
    switch (lh.getType()) {
    case PrimitiveValue::TBool:
        gt = lh.getAsBool() <= rh.getAsBool();
        break;
    case PrimitiveValue::TNumeric:
        gt = lh.getAsNum() <= rh.getAsNum();
        break;
    case PrimitiveValue::TString:
        gt = lh.getAsString() < rh.getAsString() || lh.getAsString() != rh.getAsString();
        break;
    case PrimitiveValue::TArray:
        gt = lh.getAsArray().size() <= rh.getAsArray().size();
        break;
    default:
        gt = false;
    }
    return PrimitiveValue(gt);
}

PrimitiveValue Ops::Add(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();

    switch (lh.getType()) {
    case PrimitiveValue::TNumeric:
        if (rh.getType() == PrimitiveValue::TNumeric) return lh.getAsNum() + rh.getAsNum();
        throw Error("Only a Numeric type may be added to a Numeric type", node);
    case PrimitiveValue::TString:
        if (rh.getType() == PrimitiveValue::TString) return lh.getAsString() + rh.getAsString();
        if (rh.getType() == PrimitiveValue::TNumeric) {
            std::string str;
            if (std::floor(rh.getAsNum()) == rh.getAsNum())
                str = std::to_string(static_cast<int>(rh.getAsNum()));
            else
                std::to_string(rh.getAsNum());
            return lh.getAsString() + str;
        }
        throw Error("Only Numeric and String types may be added to Strings", node);
    case PrimitiveValue::TArray: {
        auto& a = const_cast<PrimitiveValue&>(lh).getAsArray();
        a.emplace_back(rh);
        return {std::move(a)};
    }
    default:
        throw Error("Valid left operand types for '+' are Array, String, and Numeric only", node);
    }
}

PrimitiveValue Ops::Sub(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != PrimitiveValue::TNumeric || lh.getType() != PrimitiveValue::TNumeric)
        throw Error("Subtraction may only be done between Numeric types", node);
    return lhs.getAsNum() - rhs.getAsNum();
}

PrimitiveValue Ops::Mult(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();

    switch (lh.getType()) {
    case PrimitiveValue::TNumeric:
        if (rh.getType() == PrimitiveValue::TNumeric) return lh.getAsNum() * rh.getAsNum();
        throw Error("A Numeric type can only be multiplied by a Numeric type", node);
    case PrimitiveValue::TString:
        if (rh.getType() == PrimitiveValue::TNumeric) {
            const unsigned int n = static_cast<unsigned int>(rh.getAsNum());
            if (n != rh.getAsNum()) throw Error("Multiplier must be a positive integer or 0", node);
            std::string r;
            for (unsigned int i = 0; i < n; ++i) { r += lh.getAsString(); }
            return r;
        }
        throw Error("A String type can only be multiplied by a Numeric type", node);
    case PrimitiveValue::TArray:
        if (rh.getType() == PrimitiveValue::TNumeric) {
            const unsigned int n = static_cast<unsigned int>(rh.getAsNum());
            if (n != rh.getAsNum()) throw Error("Multiplier must be a positive integer or 0", node);
            ArrayValue a;
            a.reserve(a.size() * n);
            for (unsigned int i = 0; i < n; ++i) {
                for (auto v : lh.getAsArray()) a.push_back(v);
            }
            return {std::move(a)};
        }
        throw Error("An Array type can only be multiplied by a Numeric type", node);
    default:
        throw Error("Valid left operand types for '*' are Array, String, and Numeric only", node);
    }
}

PrimitiveValue Ops::Div(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != PrimitiveValue::TNumeric || lh.getType() != PrimitiveValue::TNumeric)
        throw Error("Division may only be done between Numeric types", node);
    return lhs.getAsNum() / rhs.getAsNum();
}

PrimitiveValue Ops::Exp(const PrimitiveValue& lhs, const PrimitiveValue& rhs, Symbol node) {
    const PrimitiveValue& rh = rhs.deref();
    const PrimitiveValue& lh = lhs.deref();
    if (rh.getType() != PrimitiveValue::TNumeric || lh.getType() != PrimitiveValue::TNumeric)
        throw Error("Exponentiation may only be done between Numeric types", node);
    return std::pow(lhs.getAsNum(), rhs.getAsNum());
}

} // namespace

} // namespace script
} // namespace bl
