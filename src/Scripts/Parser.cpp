#include <Scripts/Parser.hpp>
#include <cassert>

namespace bl
{
namespace scripts
{
namespace
{
parser::ISkipper::Ptr getskipper() { return parser::CommentSkipper::create("//", "/*", "*/"); }

parser::Tokenizer buildTokenizer() {
    using G = Parser::Grammar;
    parser::Tokenizer tokenizer(getskipper());

    tokenizer.addSkipperToggleChar('"');
    tokenizer.addEscapeSequence("\\n", '\n');

    tokenizer.addTokenType(G::NumLit, "([0-9]+(\\.[0-9]+)?)");
    tokenizer.addTokenType(G::StringLit, "\"([^\"]*)\"");
    tokenizer.addTokenType(G::LParen, "\\(");
    tokenizer.addTokenType(G::RParen, "\\)");
    tokenizer.addTokenType(G::LBrkt, "\\[");
    tokenizer.addTokenType(G::RBrkt, "\\]");
    tokenizer.addTokenType(G::LBrc, "\\{");
    tokenizer.addTokenType(G::RBrc, "\\}");
    tokenizer.addTokenType(G::Assign, "=");
    tokenizer.addTokenType(G::Eq, "==");
    tokenizer.addTokenType(G::Ne, "!=");
    tokenizer.addTokenType(G::Gt, ">");
    tokenizer.addTokenType(G::Ge, ">=");
    tokenizer.addTokenType(G::Lt, "<");
    tokenizer.addTokenType(G::Le, "<=");
    tokenizer.addTokenType(G::Amp, "&");
    tokenizer.addTokenType(G::Dot, "\\.");
    tokenizer.addTokenType(G::Plus, "\\+");
    tokenizer.addTokenType(G::Minus, "\\-");
    tokenizer.addTokenType(G::Mult, "\\*");
    tokenizer.addTokenType(G::Div, "/");
    tokenizer.addTokenType(G::Hat, "\\^");
    tokenizer.addTokenType(G::Comma, ",");
    tokenizer.addTokenType(G::Term, ";");
    tokenizer.addTokenType(G::Id, "[a-zA-Z]+[a-zA-Z0-9]*");
    tokenizer.addKeyword(G::Id, G::Def, "def");
    tokenizer.addKeyword(G::Id, G::If, "if");
    tokenizer.addKeyword(G::Id, G::While, "while");
    tokenizer.addKeyword(G::Id, G::Return, "return");
    tokenizer.addKeyword(G::Id, G::And, "and");
    tokenizer.addKeyword(G::Id, G::Or, "or");
    tokenizer.addKeyword(G::Id, G::Not, "not");

    return tokenizer;
}

parser::Grammar buildGrammar() {
    using G = Parser::Grammar;

    parser::Grammar grammar;

    // Writeables
    grammar.addRule(G::ArrayDef, {G::LBrc, G::RBrc});
    grammar.addRule(G::ArrayDef, {G::LBrc, G::ValueList, G::RBrc});
    grammar.addRule(G::ArrayAcc, {G::RValue, G::LBrkt, G::Value, G::RBrkt});
    grammar.addRule(G::Property, {G::Id, G::Dot, G::Id});
    grammar.addRule(G::RValue, G::Id);
    grammar.addRule(G::RValue, G::ArrayAcc);
    grammar.addRule(G::RValue, G::Property);

    // Arithmetic
    grammar.addRule(G::PGroup, {G::LParen, G::Value, G::RParen});
    grammar.addRule(G::TValue, G::PGroup);
    grammar.addRule(G::TValue, G::RValue);
    grammar.addRule(G::TValue, G::NumLit);
    grammar.addRule(G::TValue, G::StringLit);
    grammar.addRule(G::TValue, G::Call);
    grammar.addRule(G::TValue, G::ArrayDef);
    grammar.addRule(G::Exp, G::TValue);
    grammar.addRule(G::Exp, {G::Exp, G::Hat, G::Exp});
    grammar.addRule(G::Product, G::Exp);
    grammar.addRule(G::Product, {G::Product, G::Mult, G::Product});
    grammar.addRule(G::Product, {G::Product, G::Div, G::Product});
    grammar.addRule(G::Sum, G::Product);
    grammar.addRule(G::Sum, {G::Sum, G::Plus, G::Sum});
    grammar.addRule(G::Sum, {G::Sum, G::Minus, G::Sum});

    // Comparisons
    grammar.addRule(G::Cmp, G::Sum);
    grammar.addRule(G::Cmp, {G::Cmp, G::Eq, G::Cmp});
    grammar.addRule(G::Cmp, {G::Cmp, G::Ne, G::Cmp});
    grammar.addRule(G::Cmp, {G::Cmp, G::Gt, G::Cmp});
    grammar.addRule(G::Cmp, {G::Cmp, G::Ge, G::Cmp});
    grammar.addRule(G::Cmp, {G::Cmp, G::Lt, G::Cmp});
    grammar.addRule(G::Cmp, {G::Cmp, G::Le, G::Cmp});

    // Boolean Logic
    grammar.addRule(G::Negation, G::Cmp);
    grammar.addRule(G::Negation, {G::Not, G::Negation});
    grammar.addRule(G::AndGrp, G::Negation);
    grammar.addRule(G::AndGrp, {G::AndGrp, G::And, G::AndGrp});
    grammar.addRule(G::OrGrp, G::AndGrp);
    grammar.addRule(G::OrGrp, {G::OrGrp, G::Or, G::OrGrp});

    // Value
    grammar.addRule(G::Value, G::OrGrp);

    // Assignment
    grammar.addRule(G::Ref, {G::Amp, G::RValue});
    grammar.addRule(G::Assignment, {G::RValue, G::Assign, G::Value});
    grammar.addRule(G::Assignment, {G::RValue, G::Assign, G::Ref});

    // Function call
    grammar.addRule(G::ValueList, G::Value);
    grammar.addRule(G::ValueList, {G::ValueList, G::Comma, G::Value});
    grammar.addRule(G::Call, {G::Id, G::LParen, G::ValueList, G::RParen});
    grammar.addRule(G::Call, {G::Id, G::LParen, G::RParen});

    // Conditional and Loop
    grammar.addRule(G::CondHead, {G::If, G::PGroup});
    grammar.addRule(G::Conditional, {G::CondHead, G::Statement});
    grammar.addRule(G::Conditional, {G::CondHead, G::StmtBlock});
    grammar.addRule(G::LoopHead, {G::While, G::PGroup});
    grammar.addRule(G::Loop, {G::LoopHead, G::Statement});
    grammar.addRule(G::Loop, {G::LoopHead, G::StmtBlock});

    // Statements
    grammar.addRule(G::Statement, {G::Return, G::Value, G::Term});
    grammar.addRule(G::Statement, {G::Call, G::Term});
    grammar.addRule(G::Statement, {G::Assignment, G::Term});
    grammar.addRule(G::Statement, G::Conditional);
    grammar.addRule(G::Statement, G::Loop);
    grammar.addRule(G::Statement, G::Fdef);
    grammar.addRule(G::StmtList, G::Statement);
    grammar.addRule(G::StmtList, {G::StmtList, G::Statement});
    grammar.addRule(G::StmtBlock, {G::LBrc, G::StmtList, G::RBrc});

    // Function definition
    grammar.addRule(G::Param, {G::Comma, G::Id});
    grammar.addRule(G::ParamList, {G::Id, G::Param});
    grammar.addRule(G::ParamList, {G::ParamList, G::Param});
    grammar.addRule(G::FName, {G::Def, G::Id});
    grammar.addRule(G::FHead, {G::FName, G::LParen, G::RParen});
    grammar.addRule(G::FHead, {G::FName, G::LParen, G::Id, G::RParen});
    grammar.addRule(G::FHead, {G::FName, G::LParen, G::ParamList, G::RParen});
    grammar.addRule(G::Fdef, {G::FHead, G::StmtBlock});

    // Program
    grammar.addRule(G::Program, G::StmtList);

    return grammar;
}

bl::Parser buildParser() {
    parser::Grammar grammar = buildGrammar();
    grammar.setStart(Parser::Grammar::Program);
    assert(grammar.compile());
    return bl::Parser(grammar, Parser::getTokenizer());
}

} // namespace

const parser::Tokenizer& Parser::getTokenizer() {
    static const parser::Tokenizer tokenizer = buildTokenizer();
    return tokenizer;
}

parser::Grammar Parser::getGrammar() {
    static const parser::Grammar grammar = buildGrammar();
    return grammar;
}

parser::Node::Ptr Parser::parse(const std::string& input) {
    static const bl::Parser parser = buildParser();
    return parser.parse(input);
}

} // namespace scripts
} // namespace bl