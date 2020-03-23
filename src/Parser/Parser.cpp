#include <BLIB/Parser/Parser.hpp>

#include <Scripts/Parser.hpp>
#include <iostream>
#include <numeric>

std::string toStr(bl::parser::Node::Type t) {
    using T = bl::scripts::Parser::Grammar;
    switch (t) {
    case T::NumLit:
        return "NumLit";
    case T::TValue:
        return "TValue";
    case T::RValue:
        return "RValue";
    case T::Exp:
        return "Exp";
    case T::Hat:
        return "Hat";
    case T::Plus:
        return "Plus";
    case T::Minus:
        return "Minus";
    case T::Mult:
        return "Mult";
    case T::Div:
        return "Div";
    case T::LParen:
        return "LParen";
    case T::RParen:
        return "RParen";
    case T::Id:
        return "Id";
    case T::Product:
        return "Product";
    case T::Sum:
        return "Sum";
    case T::Cmp:
        return "Cmp";
    case T::Negation:
        return "Negation";
    case T::AndGrp:
        return "AndGrp";
    case T::OrGrp:
        return "OrGrp";
    case T::PGroup:
        return "PGroup";
    case T::Value:
        return "Value";
    case T::ValueList:
        return "ValueList";
    case T::ArgList:
        return "ArgList";
    case T::Call:
        return "Call";
    case T::Comma:
        return "Comma";
    default:
        return std::to_string(t);
    }
}

namespace bl
{
using namespace parser;
namespace
{
std::ostream& error(Node::Ptr node) {
    std::cerr << "Error: Line " << node->sourceLine << " column " << node->sourceColumn
              << ": ";
    return std::cerr;
}

std::ostream& operator<<(std::ostream& s, const Node::Sequence& nodes) {
    s << "[";
    if (!nodes.empty()) s << toStr(nodes[0]);
    for (unsigned int i = 1; i < nodes.size(); ++i) { s << ", " << toStr(nodes[i]); }
    s << "]";
    return s;
}

void doReduction(std::vector<Node::Ptr>& stack, unsigned int i, unsigned int len,
                 Node::Type type) {
    Node::Ptr node(new Node());
    node->type = type;
    node->children.reserve(len);
    for (unsigned int j = i; j < i + len; ++j) {
        node->children.push_back(stack[j]);
        node->data += stack[j]->data;
        stack[j]->parent = node;
    }
    node->sourceLine   = stack[i]->sourceLine;
    node->sourceColumn = stack[i]->sourceColumn;
    stack.erase(stack.begin() + i, stack.begin() + i + len);
    stack.insert(stack.begin() + i, node);
}

Node::Type getFree(const Grammar& g) {
    Node::Type t = rand();
    while (g.terminal(t) || g.nonterminal(t)) { t = rand(); }
    return t;
}

} // namespace

Parser::Parser(const Grammar& grammar, const Tokenizer& tokenizer)
: grammar(grammar)
, tokenizer(tokenizer)
, Start(getFree(grammar)) {
    isValid = generateTables();
}

bool Parser::valid() const { return isValid; }

Node::Ptr Parser::parse(const std::string& input) const {
    Stream stream(input);
    return parse(stream);
}

Node::Ptr Parser::parse(Stream& input) const {
    std::vector<Node::Ptr> parsed = tokenizer.tokenize(input);
    if (parsed.empty()) return nullptr;
    std::list<Node::Ptr> tokens(parsed.begin(), parsed.end());

    std::vector<Node::Ptr> stack;
    const auto shift = [&stack, &tokens]() {
        stack.push_back(tokens.front());
        tokens.pop_front();
    };
}

bool Parser::generateTables() {
    table.clear();

    Grammar::Production s0 = {Start, Node::Sequence({grammar.getStart()})};
    Grammar::Item i0(s0);
    getState(grammar.closure(i0), true);

    for (unsigned int i = 0; i < table.size(); ++i) {
        Grammar::ItemSet state = table[i].state;
        for (auto j = state.items().begin(); j != state.items().end(); ++j) {
            const Grammar::Item& item = *j;
            // Enter goto/shift for items not at end
            if (!item.final()) {
                Node::Type t = item.production.set[item.cursor];
                Grammar::ItemSet gotoSet;
                gotoSet.add(item.next());
                for (const Grammar::Item& ot : table[i].state.items()) {
                    if (!ot.final() && ot.production.set[ot.cursor] == t)
                        gotoSet.add(ot.next());
                }
                gotoSet           = grammar.closure(gotoSet);
                table[i].gotos[t] = getState(gotoSet, true);
                // Shift if terminal
                if (grammar.terminal(t)) table[i].actions[t] = Action::shift();
            }
            // Enter reduce for items at end
            else {
                Node::Sequence follow = grammar.followSet(item.production.set.back());
                for (Node::Type t : follow) {
                    if (table[i].actions.find(t) != table[i].actions.end()) {
                        if (table[i].actions[t].type == Action::Shift) {
                            std::cout << "Shift-Reduce error\n";
                            table.clear();
                            return false;
                        }
                        else {
                            std::cout << "Reduce-Reduce error\n";
                            table.clear();
                            return false;
                        }
                    }
                    table[i].actions[t] = Action::reduce(item);
                }
            }
        }
    }
}

bool Parser::stateExists(unsigned int s) const { return s < table.size(); }

unsigned int Parser::getState(const Grammar::ItemSet& state, bool create) {
    for (unsigned int i = 0; i < table.size(); ++i) {
        if (table[i].state == state) return i;
    }
    if (create) {
        Row row;
        row.state = state;
        table.push_back(row);
        return table.size() - 1;
    }
    return std::numeric_limits<unsigned int>::max();
}

Parser::Action Parser::Action::shift() {
    Action a;
    a.type = Shift;
    return a;
}

Parser::Action Parser::Action::reduce(const Grammar::Item& item) {
    Action a;
    a.type      = Reduce;
    a.reduction = item.production;
    return a;
}

/*
Psuedo code:
RHS length table
    Contains the number of symbols in the RHS of each rule;
    indexed by rule number
Action look-up table
    Contains an action, either a shift/goto state, or a reduction rule;
    indexed by state number and look-ahead symbol
Goto transition table
    Contains transition (goto) state numbers;
    indexed by state numbers and nonterminal symbol
begin
    // Initialize
    stateStack = empty;
    termStack = empty;
    nontermStack = empty;
    laSym = null;
    state = 0;

    // Main parsing loop
    mainLoop:
    while stateStack not empty,
        // Insure that there is a look-ahead symbol
        if laSym is null,
            laSym = read next input symbol (token),
                (which is $end on end of input);

        // Look up the next parsing action,
        //  based on the current state and look-ahead symbol
        action = ACTION[state][laSym];

        if action is SHIFT,
            // Shift the current look-ahead terminal symbol
            push laSym onto parser termStack;
            push state 0 onto stateStack;
            laSym = null;
            // Transition (shift) to the next state
            state = action.goto;
        else if action is REDUCE,
            // Perform the parser action for the rule
            rule = action.rule;
            tLen = rule.rhsTermLength;
            nLen = rule.rhsNontermLength;
            action = rule number;
            if action is ACCEPT (r0),
                if laSym is $end,
                    // Accept
                    break mainLoop;
                else
                    // Unexpected extraneous look-ahead symbol
                    invoke parser error handler;
                end if;
            else
                invoke parser action for rule,
                    passing it the top-most tLen symbols on termStack
                    and the top-most nLen symbols on nontermStack;
                // Reduce the RHS symbols, replacing them with the LHS symbol
                pop tLen symbols from termStack;
                pop nLen symbols from nontermStack;
                push rule.LHS nonterminal symbol onto nontermStack;
                pop tLen+nLen states from stateStack;
                // Do a Goto transition on the nonterminal LHS symbol
                state = last state popped from stateStack;
                state = GOTO[state][rule.LHS];
            end if;
        else
            // No action was found for unexpected look-ahead symbol
            invoke parser error handler;
            // Begin error recovery
            push error symbol onto valueStack;
            look for a transition on 'error' in the current state;
*/

} // namespace bl