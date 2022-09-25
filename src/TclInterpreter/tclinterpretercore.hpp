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
    // - Pending - stat which is kept to perform all processing operations caused by events, stat has start and end events.
    enum class Stat : uint{
        None,
        Word,             // {Child, Proccessing}
        CommandSubbingStart,   // {Processing} [
        CommandSubbingEnd,  // {Processing} ],
        Braces,  // } {Processing}   // SAVEABLE (For complete list)
        Whitespace, // " " , "\t" {Processing}
        Namespace,   // :: {Processing} // SAVEABLE
        Semicolon, // ; {Processing}
        Comment,    // # {Processing}
        EndOfString,    // {Processing}
        // vvv Stats with defined interpreter procedures
        CommandSubbing,       // {Parent, Child, Pending}
        BracesStart,   // {    /// {Parent, Child, Processing, Pending}  // SAVEABLE
        DoubleQuotes, // "  {Parent, Child, Processing, Pending}  // SAVEABLE
        ComplexWord, // {Parent, Child, Pending}
        Script,  // {Parent, Child, Pending} SAVEABLE
        BracesStartExprOnly,
        DoubleQuotesExprOnly,
        ComplexWordExprOnly,
        Ignore, // {Parent, Prodromic, Pending} SAVEABLE
        VariableSubbing,           // $ {Child, Processing}
        BackslashSubbing, // {Parent, Child, Processing}
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

    static constexpr Stat statForSpecialCommandCall(std::underlying_type_t<Stat> callTableId)
    {
        // Safeguard is special interpreter method which throws error in case of wrong stat passed to interpret method
        // Safeguard is always at the end of interpret method list
        return static_cast<Stat>(static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing) + callTableId) ;
    }

    using Stats = QVector<Stat>;
}

#endif // TCLINTERPRETERCORE_HPP
