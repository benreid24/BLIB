#ifndef BLIB_SCRIPTS_PARSER_HPP
#define BLIB_SCRIPTS_PARSER_HPP

#include <BLIB/Parser/Node.hpp>

namespace bl
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
    parser::Node::Ptr parse(const std::string& input);

    enum Grammar : parser::Node::Type {
        // Terminals
        NumLit,    // -123.5
        StringLit, // "String"
        Def,       // def
        If,        // if
        While,     // while
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
        Asign,     // =
        Eq,        // ==
        Ne,        // !=
        Gt,        // >
        Ge,        // >=
        Lt,        // <
        Le,        // >=
        Amp,       // &
        Plus,      // +
        Minus,     // -
        Mult,      // *
        Div,       // /
        Hat,       // ^
        Comma,     // ,
        Term,      // ;
        Id,        // varname

        // Non-Terminals

        // Arithmetic and Boolean Logic
        RValue,   // Id. Id LBrkt Value RBrkt
        TValue,   // RValue. NumLit. StringLit. Call
        Exp,      // TValue. TValue Hat Value
        Product,  // Exp. Product Mult Product
        Sum,      // Product. Sum Plus Sum
        Negation, // Sum. Not Negation
        AndGrp,   // Negation. AndGrp And AndGrp
        OrGrp,    // AndGrp. OrGrp Or OrGrp
        Value,    // OrGrp. LParen Value RParen

        // Asignment
        Ref,       // Amp RValue
        Asignment, // RValue Asign Value. RValue Asign Ref

        // Function Call
        ValueList, // Value. ValueList Comma Value
        Call,      // Id LParen ValueList RParen. Id LParen RParen

        // Conditional and Loop
        Cond,        // LParen Value RParen
        CondHead,    // If Cond
        Conditional, // CondHead Statement. CondHead StmtBlock
        LoopHead,    // While Cond
        Loop,        // LoopHead Statement. LoopHead StmtBlock

        // Statements
        Statement, // Call. Conditional. Loop. Asignment Term. FDef
        StmtList,  // Statement. StmtList Statement
        StmtBlock, // LBrc StmtList RBrc

        // Function Definition
        Param,     // Comma Id
        ParamList, // Id Param. ParamList Param
        FHead,     // Id LParen ParamList RParen. Id LParen Id RParen. Id LParen RParen
        Fdef,      // FHead StmtList

        // Program
        Program // StmtList
    };
};

} // namespace scripts
} // namespace bl

#endif