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

Value evalOrGrp(Symbol node, SymbolTable& table);
Value evalAndGrp(Symbol node, SymbolTable& table);
Value evalNegGrp(Symbol node, SymbolTable& table);
Value evalCmp(Symbol node, SymbolTable& table);
Value evalSum(Symbol node, SymbolTable& table);
Value evalProd(Symbol node, SymbolTable& table);
Value evalExp(Symbol node, SymbolTable& table);
Value evalTVal(Symbol node, SymbolTable& table);
Value evalRVal(Symbol node, SymbolTable& table);

} // namespace

Value ScriptImpl::computeValue(Symbol node, SymbolTable& table) {
    if (node->type != G::Value)
        throw Error("Internal error: computeValue called on non Value", node);
    if (node->children.size() != 1)
        throw Error("Internal error: Value node has invalid child", node);
    if (node->children[0]->type != G::OrGrp)
        throw Error("Internal error: Value node has invalid child", node);
    return evalOrGrp(node->children[0], table);
}

namespace
{
Value evalOrGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::OrGrp)
        throw Error("Internal error: evalOrGrp called on non OrGrp", node);
    if (node->children.size() == 1)
        return evalAndGrp(node->children[0], table);
    else if (node->children.size() == 3)
        return Ops::Or(evalOrGrp(node->children[0], table),
                       evalOrGrp(node->children[2], table));
    throw Error("Internal error: OrGrp node has invalid child", node);
}

Value evalAndGrp(Symbol node, SymbolTable& table) {
    if (node->type != G::AndGrp)
        throw Error("Internal error: evalAndGrp called on non AndGrp", node);
    if (node->children.size() == 1)
        return evalNegGrp(node->children[0], table);
    else if (node->children.size() == 3)
        return Ops::And(evalAndGrp(node->children[0], table),
                        evalAndGrp(node->children[2], table));
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
        const Value lhs = evalCmp(node->children[0], table);
        const Value rhs = evalCmp(node->children[2], table);
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
        const Value rhs = evalSum(node->children[2], table);
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
        const Value rhs = evalProd(node->children[2], table);
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
        const Value rhs = evalExp(node->children[2], table);
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
        return evalRVal(node, table);
    case G::PGroup:
        if (node->children.size() != 3)
            throw Error("Internal error: PGroup has invalid children", node);
        return ScriptImpl::computeValue(node->children[1], table);
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
    case G::ArrayDef: {
        Value::Array r;
        if (node->children.size() > 2) {
            r.reserve(node->children.size() - 2);
            for (unsigned int i = 1; i < node->children.size() - 1; ++i) {
                r.push_back(ScriptImpl::computeValue(node->children[i], table));
            }
        }
        return r;
    }
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

Value evalRVal(Symbol node, SymbolTable& table) {
    if (node->type != G::RValue) throw Error("Internal error: evalRVal called on non RValue");
    if (node->children.size() != 1) throw Error("Internal error: RValue has invalid children");

    node = node->children[0];
    switch (node->type) {
    case G::Id: {
        Value::Ptr v = table.get(node->data);
        if (!v) throw Error("Use of undefined symbol '" + node->data + "'", node);
        return *v;
    }
    case G::Property: {
        if (node->children.size() != 3)
            throw Error("Internal error: Invalid Property children", node);
        Value v = evalRVal(node->children[0], table);
        v       = v.getProperty(node->children[2]->data);
        if (v.getType() == Value::TVoid)
            throw Error("Undefined property '" + node->children[2]->data + "'",
                        node->children[2]);
        return v;
    }
    case G::ArrayAcc: {
        if (node->children.size() != 4)
            throw Error("Internal error: Invalid ArrayAcc children", node);
        Value v = evalRVal(node->children[0], table);
        Value i = ScriptImpl::computeValue(node->children[2], table);
        if (v.getType() != Value::TArray)
            throw Error("Array access on non-array type", node->children[0]);
        if (i.getType() != Value::TNumeric)
            throw Error("Array indices must be Numeric", node->children[2]);
        unsigned int j = static_cast<unsigned int>(i.getAsNum());
        if (i.getAsNum() != j)
            throw Error("Array indices must be positive integers", node->children[2]);
        if (j >= v.getAsArray().size())
            throw Error("Array index " + std::to_string(j) + " out of bounds",
                        node->children[2]);
        return v.getAsArray()[j];
    }
    default:
        throw Error("Internal error: Invalid RValue child", node);
    }
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
                if (!Ops::Eq(rh.getAsArray()[i], lh.getAsArray()[i]).getAsBool()) {
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
        if (rh.getType() == Value::TNumeric)
            return lh.getAsString() + std::to_string(rh.getAsNum());
        throw Error("Only Numeric and String types may be added to Strings", node);
    case Value::TArray: {
        auto a = lh.getAsArray();
        a.push_back(rhs);
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
        std::cout << "Warning: Referenced Value expired";
        return Value();
    }
    return v;
}

} // namespace

} // namespace scripts
} // namespace bl