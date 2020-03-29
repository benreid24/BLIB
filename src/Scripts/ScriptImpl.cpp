#include <Scripts/ScriptImpl.hpp>

#include <Scripts/Parser.hpp>
#include <cmath>
#include <iostream>

namespace bl
{
namespace scripts
{
using Symbol = parser::Node::Ptr;
using G      = Parser::Grammar;
namespace
{
struct Ops {
    static Value Or(const Value& lhs, const Value& rhs);
    static Value And(const Value& lhs, const Value& rhs);
    static Value Neg(const Value& val);
    static Value Eq(const Value& lhs, const Value& rhs);
    static Value Ne(const Value& lhs, const Value& rhs);
    static Value Gt(const Value& lhs, const Value& rhs);
    static Value Ge(const Value& lhs, const Value& rhs);
    static Value Lt(const Value& lhs, const Value& rhs);
    static Value Le(const Value& lhs, const Value& rhs);
    static Value Add(const Value& lhs, const Value& rhs, Symbol node);
    static Value Sub(const Value& lhs, const Value& rhs, Symbol node);
    static Value Mult(const Value& lhs, const Value& rhs, Symbol node);
    static Value Div(const Value& lhs, const Value& rhs, Symbol node);
    static Value Exp(const Value& lhs, const Value& rhs, Symbol node);
};

Value deref(const Value& ref);
Value::Ptr deref(Value::Ptr val);
std::vector<Value> evalList(Symbol node, SymbolTable& table);
std::optional<Value> runElifChain(Symbol node, SymbolTable& table, bool& ran);

Value evalOrGrp(Symbol node, SymbolTable& table);
Value evalAndGrp(Symbol node, SymbolTable& table);
Value evalNegGrp(Symbol node, SymbolTable& table);
Value evalCmp(Symbol node, SymbolTable& table);
Value evalSum(Symbol node, SymbolTable& table);
Value evalProd(Symbol node, SymbolTable& table);
Value evalExp(Symbol node, SymbolTable& table);
Value evalTVal(Symbol node, SymbolTable& table);
Value::Ptr evalRVal(Symbol node, SymbolTable& table, bool create = false);

} // namespace

Value ScriptImpl::computeValue(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Error("Script killed");

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
    if (table.killed()) throw Error("Script killed");

    if (node->type != G::Call)
        throw Error("Internal error: runFunction called on non Call", node);
    if (node->children.size() != 2 && node->children.size() != 3)
        throw Error("Internal error: Invalid Call children", node);

    std::vector<Value> params;
    Value func = *evalRVal(node->children[0], table);
    if (func.getType() != Value::TFunction)
        throw Error("Cannot call non-function type", node->children[0]);
    if (node->children.size() == 2) {
        node = node->children[1];
        if (node->children.size() != 3) throw Error("Internal error: Invalid ArgList", node);
        params = evalList(node->children[1], table);
    }

    return func.getAsFunction()(table, params);
}

std::optional<Value> ScriptImpl::runStatement(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Error("Script killed");
    if (node->children.empty()) throw Error("Internal error: Invalid Statement", node);

    switch (node->children[0]->type) {
    case G::Ret:
        node = node->children[0];
        if (node->children.size() == 2)
            return Value();
        else if (node->children.size() == 3)
            return computeValue(node->children[1], table);
        throw Error("Internal error: Invalid Ret children", node);

    case G::Call:
        runFunction(node->children[0], table);
        return {};

    case G::Conditional:
        return runConditional(node->children[0], table);

    case G::Loop:
        return runLoop(node->children[0], table);

    case G::ForLoop:
        return runForLoop(node->children[0], table);

    case G::Assignment: {
        node = node->children[0];
        if (node->children.size() != 4)
            throw Error("Internal error: Invalid Assignment children", node);
        Value::Ptr rv = evalRVal(node->children[0], table, true);
        node          = node->children[2];
        if (node->type == G::Ref) {
            if (node->children.size() != 2)
                throw Error("Internal error: Invalid Ref children", node);
            Value::Ptr v = evalRVal(node->children[1], table);
            *rv          = Value::Ref(v);
        }
        else if (node->type == G::Value) {
            rv  = deref(rv);
            *rv = computeValue(node, table);
        }
        else
            throw Error("Internal error: Invalid Assignment children", node);
        break;
    }

    case G::FDef: {
        node = node->children[0];
        if (node->children.size() != 2)
            throw Error("Internal error: Invalid FDef children", node);
        Symbol c = node->children[0];
        if (c->children.size() < 3) throw Error("Internal error: Invalid FHead children", c);
        c = c->children[0];
        if (c->children.size() != 2) throw Error("Internal error: Invalid FName children", c);
        if (c->children[1]->type != G::Id)
            throw Error("Internal error: Invalid FName children", c->children[1]);
        table.set(c->children[1]->data, Value(Function(node)));
        return {};
    }

    default:
        throw Error("Internal error: Invalid Statement children", node);
    }

    return {};
}

std::optional<Value> ScriptImpl::runStatementList(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Error("Script killed");

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
    if (table.killed()) throw Error("Script killed");
    if (node->type != G::Loop) throw Error("Internal error: Expected Loop", node);
    if (node->children.size() != 2) throw Error("Internal error: Invalid Loop children", node);

    Symbol head = node->children[0];
    if (head->type != G::LoopHead) throw Error("Internal error: Expected LoopHead", head);
    if (head->children.size() != 2)
        throw Error("Internal error: Invalid LoopHead children", head);
    head = head->children[1]; // PGroup

    while (evaluateCond(head, table)) {
        if (table.killed()) throw Error("Script killed");
        const std::optional<Value> r = runStatementList(node->children[1], table);
        if (r.has_value()) return r;
    }
    return {};
}

std::optional<Value> ScriptImpl::runForLoop(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Error("Script killed");
    if (node->type != G::ForLoop) throw Error("Internal error: Expected ForLoop", node);
    if (node->children.size() != 2)
        throw Error("Internal error: Invalid ForLoop children", node);

    Symbol head = node->children[0];
    if (head->type != G::ForHead) throw Error("Internal error: Invalid ForLoop child", head);
    if (head->children.size() != 6) throw Error("Invalid ForHead children", head);
    if (head->children[2]->type != G::Id)
        throw Error("Invalid ForHead child", head->children[2]);

    const Value arr        = computeValue(head->children[4], table);
    const std::string iter = head->children[2]->data;
    if (arr.getType() != Value::TArray)
        throw Error("For loop can only iterate over Array type", head->children[4]);
    for (const Value::Ptr v : arr.getAsArray()) {
        if (table.killed()) throw Error("Script killed");
        table.pushFrame();
        table.set(iter, *v, true);
        std::optional<Value> r = runStatementList(node->children[1], table);
        table.popFrame();
        if (r.has_value()) return r;
    }
    return {};
}

std::optional<Value> ScriptImpl::runConditional(Symbol node, SymbolTable& table) {
    if (table.killed()) throw Error("Script killed");
    if (node->type != G::Conditional)
        throw Error("Internal error: Expected Conditional", node);
    if (node->children.size() != 1)
        throw Error("Internal error: Invalid Conditional children", node);

    bool ran = false;
    std::optional<Value> r;
    switch (node->children[0]->type) {
    case G::ElifChain:
        return runElifChain(node->children[0], table, ran);
    case G::ElseCond:
        node = node->children[0];
        if (node->children.size() != 2)
            throw Error("Internal error: Invalid ElseCond children", node);
        r = runElifChain(node->children[0], table, ran);
        if (ran) return r;
        if (node->children[1]->type != G::ElseBlock)
            throw Error("Internal error: Invalid ElseCond children", node);
        node = node->children[1];
        if (node->children.size() != 2)
            throw Error("Internal error: Invalid ElseBlock children", node);
        return runStatementList(node->children[1], table);
    default:
        throw Error("Internal error: Invalid Conditional children", node->children[0]);
    }
}

bool ScriptImpl::evaluateCond(Symbol node, SymbolTable& table) {
    if (node->type != G::PGroup) throw Error("Internal error: Expected PGroup", node);
    if (node->children.size() != 3)
        throw Error("Internal error: Invalid PGroup children", node);
    return ScriptImpl::computeValue(node->children[1], table).getAsBool();
}

namespace
{
std::optional<Value> runElifChain(Symbol node, SymbolTable& table, bool& ran) {
    if (node->type != G::ElifChain) throw Error("Internal error: Expected ElifChain", node);

    auto getCond = [](Symbol block) -> Symbol {
        if (block->children.size() != 2)
            throw Error("Internal error: Invalid If/Elif Block children", block);
        block = block->children[0];
        if (block->children.size() != 2)
            throw Error("Internal error: Invalid If/Elif Head children", block);
        return block->children[1];
    };

    if (node->children.size() == 1) {
        node = node->children[0];
        if (node->type != G::IfBlock)
            throw Error("Internal error: Invalid ElifChain child", node);
        if (ScriptImpl::evaluateCond(getCond(node), table)) {
            ran = true;
            if (node->children.size() != 2)
                throw Error("Internal error: Invalid IfBlock children", node);
            return ScriptImpl::runStatementList(node->children[1], table);
        }
        return {};
    }
    else if (node->children.size() == 2) {
        Symbol chain                 = node->children[0];
        node                         = node->children[1]; // ElifBlock
        const std::optional<Value> r = runElifChain(chain, table, ran);
        if (ran) return r;
        if (ScriptImpl::evaluateCond(getCond(node), table)) {
            ran = true;
            if (node->children.size() != 2)
                throw Error("Internal error: Invalid ElifBlock children", node);
            return ScriptImpl::runStatementList(node->children[1], table);
        }
        return {};
    }
    throw Error("Internal error: Invalid ElifChain children ", node);
}

Value evalOrGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::OrGrp)
        throw Error("Internal error: evalOrGrp called on non OrGrp", node);
    if (node->children.size() == 1)
        return evalAndGrp(node->children[0], table);
    else if (node->children.size() == 3)
        return Ops::Or(evalOrGrp(node->children[0], table),
                       evalAndGrp(node->children[2], table));
    throw Error("Internal error: OrGrp node has invalid child", node);
}

Value evalAndGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::AndGrp)
        throw Error("Internal error: evalAndGrp called on non AndGrp", node);
    if (node->children.size() == 1)
        return evalNegGrp(node->children[0], table);
    else if (node->children.size() == 3)
        return Ops::And(evalAndGrp(node->children[0], table),
                        evalNegGrp(node->children[2], table));
    throw Error("Internal error: AndGrp node has invalid child", node);
}

Value evalNegGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::Negation)
        throw Error("Internal error: evalNegGrp called on non Neg", node);
    if (node->children.size() == 1)
        return evalCmp(node->children[0], table);
    else if (node->children.size() == 2)
        return Ops::Neg(evalNegGrp(node->children[1], table));
    throw Error("Internal error: Negation node has invalid child", node);
}

Value evalCmp(Symbol node, SymbolTable& table) {
    if (node->type != G::Cmp) throw Error("Internal error: evalCmp called on non Cmp", node);
    if (node->children.size() == 1)
        return evalSum(node->children[0], table);
    else if (node->children.size() == 3) {
        const Value lhs = evalSum(node->children[0], table);
        const Value rhs = evalSum(node->children[2], table);
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

Value evalSum(Symbol node, SymbolTable& table) {
    if (node->type != G::Sum) throw Error("Internal error: evalSum called on non Sum", node);
    if (node->children.size() == 1)
        return evalProd(node->children[0], table);
    else if (node->children.size() == 3) {
        const Value lhs = evalSum(node->children[0], table);
        const Value rhs = evalProd(node->children[2], table);
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

Value evalProd(Symbol node, SymbolTable& table) {
    if (node->type != G::Product)
        throw Error("Internal error: evalProd called on non Product", node);
    if (node->children.size() == 1)
        return evalExp(node->children[0], table);
    else if (node->children.size() == 3) {
        const Value lhs = evalProd(node->children[0], table);
        const Value rhs = evalExp(node->children[2], table);
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

Value evalExp(Symbol node, SymbolTable& table) {
    if (node->type != G::Exp) throw Error("Internal error: evalExp called on non Exp", node);
    if (node->children.size() == 1)
        return evalTVal(node->children[0], table);
    else if (node->children.size() == 3) {
        const Value lhs = evalExp(node->children[0], table);
        const Value rhs = evalTVal(node->children[2], table);
        return Ops::Exp(lhs, rhs, node);
    }
    throw Error("Internal error: Exp node has invalid child", node);
}

Value evalTVal(Symbol node, SymbolTable& table) {
    if (node->type != G::TValue) throw Error("Internal error: evalTVal called on non TValue");
    if (node->children.size() != 1) throw Error("Internal error: TValue has invalid children");

    node = node->children[0];
    switch (node->type) {
    case G::RValue:
        return deref(*evalRVal(node, table));
    case G::PGroup:
        if (node->children.size() != 3)
            throw Error("Internal error: PGroup has invalid children", node);
        return deref(ScriptImpl::computeValue(node->children[1], table));
    case G::NumLit: {
        std::stringstream ss(node->data);
        float f;
        ss >> f;
        return Value(f);
    }
    case G::StringLit:
        return Value(node->data);
    case G::Call:
        return ScriptImpl::runFunction(node, table);
    case G::ArrayDef:
        if (node->children.size() != 3) throw Error("Invalid ArrayDef children", node);
        return Value(evalList(node->children[1], table));
    case G::True: {
        Value r;
        r.makeBool(true);
        return r;
    }
    case G::False: {
        Value r;
        r.makeBool(false);
        return r;
    }
    default:
        throw Error("Internal error: Invalid TValue child");
    }
}

Value::Ptr evalRVal(Symbol node, SymbolTable& table, bool create) {
    if (node->type != G::RValue) throw Error("Internal error: evalRVal called on non RValue");
    if (node->children.size() != 1) throw Error("Internal error: RValue has invalid children");

    node = node->children[0];
    switch (node->type) {
    case G::Id: {
        Value::Ptr v = table.get(node->data, create);
        if (!v) throw Error("Use of undefined symbol '" + node->data + "'", node);
        return v;
    }
    case G::Property: {
        if (node->children.size() != 3)
            throw Error("Internal error: Invalid Property children", node);
        Value::Ptr v = evalRVal(node->children[0], table, create);
        v            = v->getProperty(node->children[2]->data, create);
        if (!v)
            throw Error("Undefined property '" + node->children[2]->data + "'",
                        node->children[2]);
        return v;
    }
    case G::ArrayAcc: {
        if (node->children.size() != 4)
            throw Error("Internal error: Invalid ArrayAcc children", node);
        Value::Ptr v = evalRVal(node->children[0], table, create);
        Value i      = ScriptImpl::computeValue(node->children[2], table);
        if (v->getType() != Value::TArray)
            throw Error("Array access on non-array type", node->children[0]);
        if (i.getType() != Value::TNumeric)
            throw Error("Array indices must be Numeric", node->children[2]);
        unsigned int j = static_cast<unsigned int>(i.getAsNum());
        if (i.getAsNum() != j)
            throw Error("Array indices must be positive integers", node->children[2]);
        if (j >= v->getAsArray().size())
            throw Error("Array index " + std::to_string(j) + " out of bounds",
                        node->children[2]);
        return v->getAsArray()[j];
    }
    default:
        throw Error("Internal error: Invalid RValue child", node);
    }
}

std::vector<Value> evalList(Symbol node, SymbolTable& table) {
    if (node->type != G::ValueList)
        throw Error("Internal error: evalList called on non ValueList");

    if (node->children.size() == 1)
        return {ScriptImpl::computeValue(node->children[0], table)};
    else if (node->children.size() == 3) {
        std::vector<Value> r = evalList(node->children[0], table);
        r.push_back(ScriptImpl::computeValue(node->children[2], table));
        return r;
    }
    throw Error("Internal error: Invalid ValueList children", node);
}

Value Ops::Or(const Value& lhs, const Value& rhs) {
    Value r;
    r.makeBool(deref(lhs).getAsBool() || deref(rhs).getAsBool());
    return r;
}

Value Ops::And(const Value& lhs, const Value& rhs) {
    Value r;
    r.makeBool(deref(lhs).getAsBool() && deref(rhs).getAsBool());
    return r;
}

Value Ops::Neg(const Value& val) {
    Value r;
    r.makeBool(!deref(val).getAsBool());
    return r;
}

Value Ops::Eq(const Value& lhs, const Value& rhs) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != lh.getType()) {
        Value r;
        r.makeBool(false);
        return r;
    }
    bool eq = true;
    switch (lh.getType()) {
    case Value::TBool:
        eq = lh.getAsBool() == rh.getAsBool();
        break;
    case Value::TNumeric:
        eq = lh.getAsNum() == rh.getAsNum();
        break;
    case Value::TString:
        eq = lh.getAsString() == rh.getAsString();
        break;
    case Value::TArray:
        if (rh.getAsArray().size() == lh.getAsArray().size()) {
            for (unsigned int i = 0; i < rh.getAsArray().size(); ++i) {
                if (!Ops::Eq(*rh.getAsArray()[i], *lh.getAsArray()[i]).getAsBool()) {
                    eq = false;
                    break;
                }
            }
        }
        else
            eq = false;
        break;
    case Value::TFunction:
        eq = rh.getAsFunction() == lh.getAsFunction();
        break;
    default:
        eq = false;
    }
    Value r;
    r.makeBool(eq);
    return r;
}

Value Ops::Ne(const Value& lhs, const Value& rhs) {
    Value r;
    r.makeBool(!Ops::Eq(rhs, lhs).getAsBool());
    return r;
}

Value Ops::Gt(const Value& lhs, const Value& rhs) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != lh.getType()) {
        Value r;
        r.makeBool(false);
        return r;
    }
    bool gt = true;
    switch (lh.getType()) {
    case Value::TBool:
        gt = lh.getAsBool() > rh.getAsBool();
        break;
    case Value::TNumeric:
        gt = lh.getAsNum() > rh.getAsNum();
        break;
    case Value::TString:
        gt = !(lh.getAsString() < rh.getAsString()) && lh.getAsString() != rh.getAsString();
        break;
    case Value::TArray:
        gt = lh.getAsArray().size() > rh.getAsArray().size();
        break;
    default:
        gt = false;
    }
    Value r;
    r.makeBool(gt);
    return r;
}

Value Ops::Ge(const Value& lhs, const Value& rhs) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != lh.getType()) {
        Value r;
        r.makeBool(false);
        return r;
    }
    bool ge = true;
    switch (lh.getType()) {
    case Value::TBool:
        ge = lh.getAsBool() >= rh.getAsBool();
        break;
    case Value::TNumeric:
        ge = lh.getAsNum() >= rh.getAsNum();
        break;
    case Value::TString:
        ge = !(lh.getAsString() < rh.getAsString());
        break;
    case Value::TArray:
        ge = lh.getAsArray().size() >= rh.getAsArray().size();
        break;
    default:
        ge = false;
    }
    Value r;
    r.makeBool(ge);
    return r;
}

Value Ops::Lt(const Value& lhs, const Value& rhs) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != lh.getType()) {
        Value r;
        r.makeBool(false);
        return r;
    }
    bool lt = true;
    switch (lh.getType()) {
    case Value::TBool:
        lt = lh.getAsBool() < rh.getAsBool();
        break;
    case Value::TNumeric:
        lt = lh.getAsNum() < rh.getAsNum();
        break;
    case Value::TString:
        lt = lh.getAsString() < rh.getAsString();
        break;
    case Value::TArray:
        lt = lh.getAsArray().size() < rh.getAsArray().size();
        break;
    default:
        lt = false;
    }
    Value r;
    r.makeBool(lt);
    return r;
}

Value Ops::Le(const Value& lhs, const Value& rhs) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != lh.getType()) {
        Value r;
        r.makeBool(false);
        return r;
    }
    bool gt = true;
    switch (lh.getType()) {
    case Value::TBool:
        gt = lh.getAsBool() <= rh.getAsBool();
        break;
    case Value::TNumeric:
        gt = lh.getAsNum() <= rh.getAsNum();
        break;
    case Value::TString:
        gt = lh.getAsString() < rh.getAsString() || lh.getAsString() != rh.getAsString();
        break;
    case Value::TArray:
        gt = lh.getAsArray().size() <= rh.getAsArray().size();
        break;
    default:
        gt = false;
    }
    Value r;
    r.makeBool(gt);
    return r;
}

Value Ops::Add(const Value& lhs, const Value& rhs, Symbol node) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);

    switch (lh.getType()) {
    case Value::TNumeric:
        if (rh.getType() == Value::TNumeric) return lh.getAsNum() + rh.getAsNum();
        throw Error("Only a Numeric type may be added to a Numeric type", node);
    case Value::TString:
        if (rh.getType() == Value::TString) return lh.getAsString() + rh.getAsString();
        if (rh.getType() == Value::TNumeric) {
            std::string str = std::to_string(rh.getAsNum());
            if (std::floor(rh.getAsNum()) == rh.getAsNum())
                str = std::to_string(static_cast<int>(rh.getAsNum()));
            return lh.getAsString() + str;
        }
        throw Error("Only Numeric and String types may be added to Strings", node);
    case Value::TArray: {
        auto a = lh.getAsArray();
        a.push_back(Value::Ptr(new Value(rhs)));
        return a;
    }
    default:
        throw Error("Valid left operand types for '+' are Array, String, and Numeric only",
                    node);
    }
}

Value Ops::Sub(const Value& lhs, const Value& rhs, Symbol node) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != Value::TNumeric || lh.getType() != Value::TNumeric)
        throw Error("Subtraction may only be done between Numeric types", node);
    return lhs.getAsNum() - rhs.getAsNum();
}

Value Ops::Mult(const Value& lhs, const Value& rhs, Symbol node) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);

    switch (lh.getType()) {
    case Value::TNumeric:
        if (rh.getType() == Value::TNumeric) return lh.getAsNum() * rh.getAsNum();
        throw Error("A Numeric type can only be multiplied by a Numeric type", node);
    case Value::TString:
        if (rh.getType() == Value::TNumeric) {
            const unsigned int n = static_cast<unsigned int>(rh.getAsNum());
            if (n != rh.getAsNum())
                throw Error("Multiplier must be a positive integer or 0", node);
            std::string r;
            for (unsigned int i = 0; i < n; ++i) { r += lh.getAsString(); }
            return r;
        }
        throw Error("A String type can only be multiplied by a Numeric type", node);
    case Value::TArray:
        if (rh.getType() == Value::TNumeric) {
            const unsigned int n = static_cast<unsigned int>(rh.getAsNum());
            if (n != rh.getAsNum())
                throw Error("Multiplier must be a positive integer or 0", node);
            Value::Array a;
            a.reserve(a.size() * n);
            for (unsigned int i = 0; i < n; ++i) {
                for (auto v : lh.getAsArray()) a.push_back(v);
            }
            return a;
        }
        throw Error("An Array type can only be multiplied by a Numeric type", node);
    default:
        throw Error("Valid left operand types for '*' are Array, String, and Numeric only",
                    node);
    }
}

Value Ops::Div(const Value& lhs, const Value& rhs, Symbol node) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != Value::TNumeric || lh.getType() != Value::TNumeric)
        throw Error("Division may only be done between Numeric types", node);
    return lhs.getAsNum() / rhs.getAsNum();
}

Value Ops::Exp(const Value& lhs, const Value& rhs, Symbol node) {
    Value rh = deref(rhs);
    Value lh = deref(lhs);
    if (rh.getType() != Value::TNumeric || lh.getType() != Value::TNumeric)
        throw Error("Exponentiation may only be done between Numeric types", node);
    return std::pow(lhs.getAsNum(), rhs.getAsNum());
}

Value deref(const Value& v) {
    if (v.getType() == Value::TRef) {
        Value::CPtr l = v.getAsRef().lock();
        if (l) return deref(*l);
        std::cerr << "Warning: Referenced Value expired";
        return Value();
    }
    return v;
}

Value::Ptr deref(Value::Ptr val) {
    if (val->getType() == Value::TRef) {
        Value::Ptr r = val->getAsRef().lock();
        if (!r) throw Error("Expired reference");
        return deref(r);
    }
    return val;
}

} // namespace

} // namespace scripts
} // namespace bl