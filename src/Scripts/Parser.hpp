#ifndef BLIB_SCRIPTS_PARSER_HPP
#define BLIB_SCRIPTS_PARSER_HPP

#include <BENG/Parser.hpp>

namespace bg
{
namespace scripts
{
/**
 * @brief Implementation of a Parser for the scripting language
 * @ingroup Scripts
 *
 */
struct Parser {
    /**
     * @brief Parses the input script into a parse tree
     *
     * @param input The script to parse
     * @return parser::Node::Ptr The root node, or nullptr on error
     */
    static parser::Node::Ptr parse(const std::string& input);

    /**
     * @brief Builds and returns the tokenizer for scripts
     *
     * @return parser::Tokenizer
     */
    static const parser::Tokenizer& getTokenizer();

    /**
     * @brief Builds and returns the Grammar for scripts
     *
     * @return parser::Grammar An uncompiled grammar w/o root node set
     */
    static parser::Grammar getGrammar();

    enum Grammar : parser::Node::Type {
        // Terminals
        NumLit,    // -123.5
        StringLit, // "String"
        True,      // true
        False,     // false
        Def,       // def
        If,        // if
        Elif,      // elif
        Else,      // Else
        While,     // while
        For,       // for
        In,        // In
        Return,    // return
        And,       // and
        Or,        // or
        Not,       // not
        LParen,    // (
        RParen,    // )
        LBrkt,     // [
        RBrkt,     // ]
        LBrc,      // {
        RBrc,      // }
        Assign,    // =
        Eq,        // ==
        Ne,        // !=
        Gt,        // >
        Ge,        // >=
        Lt,        // <
        Le,        // >=
        Amp,       // &
        Dot,       // .
        Plus,      // +
        Minus,     // -
        Mult,      // *
        Div,       // /
        Hat,       // ^
        Comma,     // ,
        Term,      // ;
        Colon,     // :
        Id,        // varname

        // Non-Terminals

        // Writeables
        ArrayDef, // LBrkt RBrkt. LBrkt ValueList RBrkt
        Property, // RValue Dot Id
        ArrayAcc, // RValue LBrkt Value RBrkt
        RValue,   // Id. ArrayAcc. Property

        // Arithmetic
        UNeg,    // Minus TValue
        TValue,  // RValue. NumLit. StringLit. Call. PGroup. ArrayDef. True. False. UNeg
        Exp,     // TValue. Exp Hat TValue
        Product, // Exp. Product Mult Exp. Product Div Exp
        Sum,     // Product. Sum Plus Product. Sum Minus Product

        // Comparisons
        Cmp, // Sum. Sum [Eq, Ne, Gt, Ge, Le, Lt] Sum

        // Boolean Logic and top Value
        Negation, // Cmp. Not Negation
        AndGrp,   // Negation. AndGrp And Negation
        OrGrp,    // AndGrp. OrGrp Or AndGrp
        PGroup,   // LParen Value RParen
        Value,    // OrGrp

        // Assignment
        Ref,        // Amp RValue
        Assignment, // RValue Assign Value Term. RValue Assign Ref Term

        // Function Call
        ValueList, // Value. ValueList Comma Value
        ArgList,   // LParen ValueList RParen
        Call,      // RValue ArgList. RValue LParen RParen

        // Conditional
        IfHead,      // If PGroup
        ElifHead,    // Elif PGroup
        IfBlock,     // IfHead Statement. IfHead StmtBlock
        ElifBlock,   // ElifHead Statement. ElifHead StmtBlock
        ElseBlock,   // Else Statement. Else StmtBlock
        ElifChain,   // IfBlock. ElifChain ElifBlock
        ElseCond,    // ElifChain ElseBlock
        Conditional, // ElifChain. ElseCond

        // Loop
        LoopHead, // While PGroup
        Loop,     // LoopHead Statement. LoopHead StmtBlock
        ForHead,  // For LParen Id In Value RParen
        ForLoop,  // ForHead Statement. ForHead StmtBlock

        // Statements
        Ret,       // Return Term. Return Value Term
        Statement, // Ret. Call Term. Conditional. Loop. ForLoop. Assignment. FDef
        StmtList,  // Statement. StmtList Statement
        StmtBlock, // LBrc StmtList RBrc

        // Function Definition
        ParamList, // Id Comma Id. ParamList Comma Id
        FName,     // Def Id
        FHead, // FName LParen ParamList RParen. FName LParen Id RParen. FName LParen RParen
        FDef,  // FHead StmtBlock

        // Program
        Program // StmtList
    };
};

} // namespace scripts
} // namespace bg

#endif