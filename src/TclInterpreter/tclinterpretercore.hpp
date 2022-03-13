#ifndef TCLINTERPRETERCORE_HPP
#define TCLINTERPRETERCORE_HPP

#include<QtGlobal>
#include<QString>

namespace TclInterpreterCore {
    using TclCommand = QString;
    using Command = QString;
    using Commands = QStringList;
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
        CommandSubbingStart,   // {Processing, Prodromic, Pending} [, SAVABLE
        CommandSubbingEnd,  // {Processing} ],
        Braces,  // } {Processing}   // SAVEABLE (For complete list)
        MainScript,  // {Parent, Pending} SAVEABLE // Previous name: MainCodeBlock
        Whitespace, // " " , "\t" {Pending, Processing}   // SAVEABLE (Special Cases)
        VariableSubbingStart, // $ {Processing, Prodromic, Pending} // SAVEABLE
        Namespace,   // :: {Processing} // SAVEABLE
        Semicolon, // ; {Processing}
        Comment,    // # {Processing} Save to Pending Stats (if tclCommand ends with \)   // check   add EndOfString Stat to processing stats
        EndOfString,    // {Processing} Deinitialize saved stats
        Script,  // {Parent, Child, Processing, Pending} SAVEABLE // Old Name: CodeBLock
        //EndOfCodeBlock,  // SAVEABLE
        //Snprintf,   //  SAVEABLE For case of procedure call for speech mark processing -> procedure call [string concat ...]
        BackslashSubbing, // {Processing, Prodromic, Pnding} // Old Name: SpecialSign
        //PendingString,  // SAVEABLE
        //PendingSnprintf,    // SAVEABLE
        //StringInQuotes, // SAVEABLE
        Ignore, // {Parent, Prodromic, Pending} SAVEABLE
        //EndOfExpression, // SAVEABLE
        CommandSubbing,       // {Parent, Child, Pending} //  Old name - FunctionCall
        BracesStart,   // {    /// {Parent, Child, Processing, Pending} Everything is a string // SAVEABLE
        DoubleQuotes, // "  {Parent, Child, Processing, Pending}  // SAVEABLE
        Expression, // {Parent, Child} SAVEABLE
        Size
    };
    using Stats = QVector<Stat>;
}

#endif // TCLINTERPRETERCORE_HPP
