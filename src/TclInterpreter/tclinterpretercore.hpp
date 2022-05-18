#ifndef TCLINTERPRETERCORE_HPP
#define TCLINTERPRETERCORE_HPP

#include<QtGlobal>
#include<QString>

namespace Tcl::Interpreter::Core {
    using TclCommand = QString;
    using OutputCommand = QString;
    using OutputCommands = QStringList;
    enum class Error : bool{Error, NoError};

    // Stat flags:
    // - Parent - parent can be saved in SavedStats stack
    // - Child - child can be saved in another SavedStat
    // - Processing - owns method for interpretation of event for that stat
    // - Prodromic - expects the occurance of specific next event, can be saved in SavedStats stack
    // - Pending - stat which is keept to perform all processing operations caused by events, stat has start and end events.
    enum class Stat : uint{
        None,
        Word,             // {Child, Proccessing} //Old Name: String // For definition, Check Tcl concept in Github References
        //Const,              // SAVEABLE   // Check const or string is only in special cases - const and string distinction do not change interpreter behaviour
        //Operator,           // SAVEABLE // In Development
        //UnknownString, Word state substitute that state
        CommandSubbingStart,   // {Processing} [
        CommandSubbingEnd,  // {Processing} ],
        Braces,  // } {Processing}   // SAVEABLE (For complete list)
        Whitespace, // " " , "\t" {Processing}
        //VariableSubbingStart, // $ {Processing} // SAVEABLE
        Namespace,   // :: {Processing} // SAVEABLE
        Semicolon, // ; {Processing}
        Comment,    // # {Processing} Save to Pending Stats (if tclCommand ends with \)   // check   add EndOfString Stat to processing stats
        EndOfString,    // {Processing} Deinitialize saved stats
        //EndOfCodeBlock,  // SAVEABLE
        //Snprintf,   //  SAVEABLE For case of procedure call for speech mark processing -> procedure call [string concat ...]
        //PendingString,  // SAVEABLE
        //PendingSnprintf,    // SAVEABLE
        //StringInQuotes, // SAVEABLE
        //EndOfExpression, // SAVEABLE
        //MainScript,  // {Parent, Pending} SAVEABLE // Previous name: MainCodeBlock
        //Ignore, // {Parent, Prodromic, Pending} SAVEABLE
        CommandSubbing,       // {Parent, Child, Pending} //  Old name - FunctionCall
        //Expression, // {Parent, Child} SAVEABLE
        //CommandExpression, // {Parent, Child} SAVEABLE
        BracesStart,   // {    /// {Parent, Child, Processing, Pending} Everything is a string // SAVEABLE
        DoubleQuotes, // "  {Parent, Child, Processing, Pending}  // SAVEABLE
        ComplexWord, // {Parent, Child, Pending} If VariableSubbing,BackslashSubbing or CommandSubbing occured with anther string (without whitespace)
        Script,  // {Parent, Child, Pending} SAVEABLE // Old Name: CodeBLock
        //ExprCommand, // {Parent, Child} SAVEABLE
        Ignore, // {Parent, Prodromic, Pending} SAVEABLE
        VariableSubbing,           // $ {Child, Processing} // Specific behaviour
        BackslashSubbing, // {Parent, Child, Processing} // Old Name: SpecialSign
        Size,
        Common // - Used to define one command processing procedure in Tcl Command Controller
    };

    static constexpr std::underlying_type_t<Stat> numbOfSpecialCommandCalls()
    {
        return std::underlying_type_t<Stat>(Stat::Size) - std::underlying_type_t<Stat>(Stat::CommandSubbing);
    }
    static constexpr std::underlying_type_t<Stat> numbOfSpecialCommandCallsAndSafeguard()
    {
        // Safeguard is special interpreter method which throws error in case of wrong stat passed to interpret method
        // Safeguard is always at the end of interpret method list
        return numbOfSpecialCommandCalls() + 1;
    }

    using Stats = QVector<Stat>;
}

#endif // TCLINTERPRETERCORE_HPP
