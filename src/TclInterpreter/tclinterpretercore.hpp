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
    // - Pending - stat which is keeped to perform all processing operations caused by events, stat has start and end events.
    enum class Stat : uint{
        None,
        Word,             // {Child, Proccessing} //Old Name: String // For definition, Check Tcl concept in Github References
        //Const,              // SAVEABLE   // Check const or string is only in special cases - const and string distinction do not change interpreter behaviour
        //Operator,           // SAVEABLE // In Development
        VariableSubbing,           // {Child}
        //UnknownString, Word state substitute that state
        CommandSubbingStart,   // {Processing} [
        CommandSubbingEnd,  // {Processing} ],
        Braces,  // } {Processing}   // SAVEABLE (For complete list)
        Whitespace, // " " , "\t" {Processing}   // SAVEABLE (Special Cases)
        VariableSubbingStart, // $ {Processing} // SAVEABLE
        Namespace,   // :: {Processing} // SAVEABLE
        Semicolon, // ; {Processing}
        Comment,    // # {Processing} Save to Pending Stats (if tclCommand ends with \)   // check   add EndOfString Stat to processing stats
        EndOfString,    // {Processing} Deinitialize saved stats
        //EndOfCodeBlock,  // SAVEABLE
        //Snprintf,   //  SAVEABLE For case of procedure call for speech mark processing -> procedure call [string concat ...]
        BackslashSubbing, // {Parent, Child, Processing} // Old Name: SpecialSign
        //PendingString,  // SAVEABLE
        //PendingSnprintf,    // SAVEABLE
        //StringInQuotes, // SAVEABLE
        //EndOfExpression, // SAVEABLE
        MainScript,  // {Parent, Pending} SAVEABLE // Previous name: MainCodeBlock
        Ignore, // {Parent, Prodromic, Pending} SAVEABLE
        CommandSubbing,       // {Parent, Child, Pending} //  Old name - FunctionCall
        BracesStart,   // {    /// {Parent, Child, Processing, Pending} Everything is a string // SAVEABLE
        DoubleQuotes, // "  {Parent, Child, Processing, Pending}  // SAVEABLE
        ComplexWord, // {Parent, Child, Pending} If VariableSubbing,BackslashSubbing or CommandSubbing occured with anther string (without whitespace)
        Expression, // {Parent, Child} SAVEABLE
        Script,  // {Parent, Child, Pending} SAVEABLE // Old Name: CodeBLock
        Size
    };
    using Stats = QVector<Stat>;
}

#endif // TCLINTERPRETERCORE_HPP
