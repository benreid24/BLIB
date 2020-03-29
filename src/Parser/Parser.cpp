#include <BLIB/Parser/Parser.hpp>

#include <Scripts/Parser.hpp>
#include <iostream>
#include <numeric>
#include <stack>

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
    if (!nodes.empty()) s << nodes[0];
    for (unsigned int i = 1; i < nodes.size(); ++i) { s << ", " << nodes[i]; }
    s << "]";
    return s;
}

Node::Type getFree(const Grammar& g) {
    Node::Type t = rand();
    while (g.terminal(t) || g.nonterminal(t)) { t = rand(); }
    return t;
}

int nonterminalCount(const Grammar& g, const Grammar::Production& prod) {
    int c = 0;
    for (Node::Type t : prod.set) {
        if (g.nonterminal(t)) ++c;
    }
    return c;
}

int terminalCount(const Grammar& g, const Grammar::Production& prod) {
    int c = 0;
    for (Node::Type t : prod.set) {
        if (g.terminal(t)) ++c;
    }
    return c;
}

bool doReduction(const Grammar& g, std::vector<Node::Ptr>& nonterminals,
                 std::vector<Node::Ptr>& terminals, std::stack<unsigned int>& states,
                 unsigned int& state, const Grammar::Production& prod) {
    Node::Ptr nt(new Node());
    nt->type = prod.result;
    nt->children.resize(prod.set.size());

    const int c = terminalCount(g, prod) + nonterminalCount(g, prod);
    for (int i = 0; i < c; ++i) {
        if (!states.empty())
            states.pop();
        else {
            error(terminals.back())
                << "Internal parser error, not enough states for reduction" << std::endl;
            return false;
        }
    }
    if (!states.empty()) state = states.top();

    for (unsigned int i = 0; i < prod.set.size(); ++i) {
        const int pi = prod.set.size() - i - 1;
        const int si = nt->children.size() - i - 1;
        if (g.terminal(prod.set[pi])) {
            nt->children[si] = terminals.back();
            terminals.pop_back();
        }
        else if (g.nonterminal(prod.set[pi])) {
            nt->children[si] = nonterminals.back();
            nonterminals.pop_back();
        }
        else {
            error(terminals.back())
                << "Internal parser error, grammar production uses undefined symbol"
                << std::endl;
            return false;
        }
        nt->children[si]->parent = nt;
        if (nt->children[si]->type != prod.set[pi]) {
            error(nt->children[si])
                << "Internal parser error, table specified reduction that does "
                   "not match the given production"
                << std::endl;
            return false;
        }
    }

    nt->sourceLine   = nt->children.front()->sourceLine;
    nt->sourceColumn = nt->children.front()->sourceColumn;
    nonterminals.push_back(nt);
    return true;
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

    Node::Ptr eoi(new Node(*tokens.back()));
    eoi->type = Node::EOI;
    eoi->data = "<EOF>";
    tokens.push_back(eoi);

    unsigned int state = 0;
    std::stack<unsigned int> stateHist;
    stateHist.push(state);

    std::vector<Node::Ptr> terminals;
    std::vector<Node::Ptr> nonterminals;

    const auto shift = [&terminals, &tokens]() {
        terminals.push_back(tokens.front());
        tokens.pop_front();
    };
    const auto reduce = [this, &terminals, &nonterminals, &state, &stateHist](
                            const Grammar::Production& p) -> bool {
        return doReduction(grammar, nonterminals, terminals, stateHist, state, p);
    };

    while (!stateHist.empty()) {
        Node::Ptr lookahead = tokens.front();
        if (table[state].actions.find(lookahead->type) != table[state].actions.end()) {
            const Action& action = table[state].actions.at(lookahead->type);
            if (action.type == Action::Shift) {
                shift();
                if (table[state].gotos.find(lookahead->type) != table[state].gotos.end()) {
                    state = table[state].gotos.at(lookahead->type);
                    stateHist.push(state);
                }
                else {
                    error(lookahead) << "Internal parser error, no goto in table" << std::endl;
                    return nullptr;
                }
            }
            else if (action.type == Action::Reduce) {
                if (!action.reduction.has_value()) {
                    error(lookahead)
                        << "Internal parser error, no production for reduce action"
                        << std::endl;
                    return nullptr;
                }
                if (!reduce(action.reduction.value())) { return nullptr; }
                if (stateHist.empty() || nonterminals.back()->type == Start) break;
                Node::Type t = nonterminals.back()->type;
                if (table[state].gotos.find(t) != table[state].gotos.end()) {
                    state = table[state].gotos.at(t);
                    stateHist.push(state);
                }
                else {
                    error(lookahead) << "Internal parser error, no goto in table" << std::endl;
                    return nullptr;
                }
            }
            else {
                error(lookahead) << "Internal parser error, invalid action "
                                 << table[state].actions.at(lookahead->type).type << std::endl;
                return nullptr;
            }
        }
        else {
            error(lookahead) << "Unexpected token '" << lookahead->data << "'" << std::endl;
            return nullptr;
        }
    }

    if (nonterminals.empty()) {
        if (!terminals.empty()) error(terminals.back());
        std::cerr << "Unable to perform reduction" << std::endl;
        return nullptr;
    }
    if (!terminals.empty()) {
        error(terminals.back()) << "Unable to process remaining input" << std::endl;
        return nullptr;
    }
    if (nonterminals.size() != 1) {
        error(nonterminals[0]) << "Unable to reduce parse tree" << std::endl;
        return nullptr;
    }
    if (nonterminals.back()->type != Start) {
        error(nonterminals.back()) << "Invalid root symbol" << std::endl;
        return nullptr;
    }
    if (nonterminals.back()->children[0]->type != grammar.getStart()) {
        error(nonterminals.back()) << "Did not reach grammar start symbol" << std::endl;
        return nullptr;
    }

    Node::Ptr result = nonterminals.back()->children[0];
    result->parent   = nullptr;
    return result;
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
                            std::cerr << "Shift-Reduce error\n";
                            table.clear();
                            return false;
                        }
                        else {
                            std::cerr << "Reduce-Reduce error\n";
                            table.clear();
                            return false;
                        }
                    }
                    table[i].actions[t] = Action::reduce(item);
                }
            }
        }
    }

    return true;
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

} // namespace bl