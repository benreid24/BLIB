#include <BLIB/Parser/Parser.hpp>

#include <BLIB/Logging.hpp>
#include <Scripts/Parser.hpp>
#include <numeric>
#include <stack>

#define PARSER_ERROR(node) \
    BL_LOG_ERROR << "Line " << node->sourceLine << " column " << node->sourceColumn << ": "

namespace bl
{
using namespace parser;
namespace
{
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
            PARSER_ERROR(terminals.back())
                << "Internal parser error, not enough states for reduction";
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
            PARSER_ERROR(terminals.back())
                << "Internal parser error, grammar production uses undefined symbol";
            return false;
        }
        nt->children[si]->parent = nt;
        if (nt->children[si]->type != prod.set[pi]) {
            PARSER_ERROR(nt->children[si])
                << "Internal parser error, table specified reduction that does "
                   "not match the given production";
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
                    PARSER_ERROR(lookahead) << "Internal parser error, no goto in table";
                    return nullptr;
                }
            }
            else if (action.type == Action::Reduce) {
                if (!action.reduction.has_value()) {
                    PARSER_ERROR(lookahead)
                        << "Internal parser error, no production for reduce action";
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
                    PARSER_ERROR(lookahead) << "Internal parser error, no goto in table";
                    return nullptr;
                }
            }
            else {
                PARSER_ERROR(lookahead) << "Internal parser error, invalid action "
                                        << table[state].actions.at(lookahead->type).type;
                return nullptr;
            }
        }
        else {
            PARSER_ERROR(lookahead) << "Unexpected token '" << lookahead->data << "'";
            return nullptr;
        }
    }

    if (nonterminals.empty()) {
        if (!terminals.empty())
            PARSER_ERROR(terminals.back()) << "Unable to perform reduction";
        else
            BL_LOG_ERROR << "Unable to perform reduction";
        return nullptr;
    }
    if (!terminals.empty()) {
        PARSER_ERROR(terminals.back()) << "Unable to process remaining input";
        return nullptr;
    }
    if (nonterminals.size() != 1) {
        PARSER_ERROR(nonterminals[0]) << "Unable to reduce parse tree";
        return nullptr;
    }
    if (nonterminals.back()->type != Start) {
        PARSER_ERROR(nonterminals.back()) << "Invalid root symbol";
        return nullptr;
    }
    if (nonterminals.back()->children[0]->type != grammar.getStart()) {
        PARSER_ERROR(nonterminals.back()) << "Did not reach grammar start symbol";
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
                            BL_LOG_ERROR << "Shift-Reduce error";
                            table.clear();
                            return false;
                        }
                        else {
                            BL_LOG_ERROR << "Reduce-Reduce error";
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