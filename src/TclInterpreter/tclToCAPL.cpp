#include"tclToCAPL.hpp"
#include"tclToCAPL_errorFileConfig.hpp"
#include"Tcl2Capl/controller.hpp"
#include"Tcl2Capl/caplFunctionDefiniitions.hpp"

using TCLInterpreter = Tcl::TCLInterpreter;
using TclCommandsController = Tcl::Interpreter::Command::Controller;
namespace TclCommand_NS = Tcl::Interpreter::Command;
using KeywordsController = Tcl::Interpreter::KeywordsController;
using Result = KeywordsController::Result;
using ReadIgnoreResult = KeywordsController::ReadIgnoreResult;
using CheckingResult = KeywordsController::CheckingResult;
using KeywordsMap = KeywordsController::KeywordsMap;
//using TCLInterpreterFunctions = TCLInterpreter::InterpretFunctions;
using UserInteractionStatus = Tcl::UserInteractionStatus;
using Rule = TclCommand_NS::Definition::Rule;
using Action = TclCommand_NS::Definition::Action;
using RuleControlFlag = Rule::Control;
using Format = TclCommand_NS::Definition::Format;
using FormatRule = TclCommand_NS::Definition::Format::Rule;
using FormatTarget = TclCommand_NS::Definition::Format::Target;

using namespace Tcl::Interpreter;

TCLInterpreter::TCLInterpreter(UserInputConfig& userConfig, FunctionDefinitionsRef functionDefinitionsRef)
    : functionDefinitions(functionDefinitionsRef), commandsController(*this, userConfig), _listController(*this)
{clearError();}



KeywordsMap KeywordsController::keywordsMap ={
    {
        {"#", Stat::Comment},
        {"{", Stat::BracesStart},
        {"}", Stat::Braces},
        {"[", Stat::CommandSubbingStart},
        {"]", Stat::CommandSubbingEnd},
        {"(", Stat::Word},
        {")", Stat::Word},
        {"+", Stat::Word},
        {"-", Stat::Word},
        {"*", Stat::Word},
        {"/", Stat::Word},
        {"%", Stat::Word},
        {">", Stat::Word},
        {"<", Stat::Word},
        {"!", Stat::Word},
        {"&", Stat::Word},
        {"|", Stat::Word},
        {"^", Stat::Word},
        {"?", Stat::Word},
        {":", Stat::Word},
        {" ", Stat::Whitespace},
        {"\t", Stat::Whitespace},
        {"$", Stat::VariableSubbing},
        {"\"", Stat::DoubleQuotes},
        {";", Stat::Semicolon},
        {"\\", Stat::BackslashSubbing}

    },
    {
        {"::", Stat::Namespace},
        {"==", Stat::Word},
        {"!=", Stat::Word},
        {">=", Stat::Word},
        {"<=", Stat::Word},
        {"&&", Stat::Word},
        {"||", Stat::Word},
        {"<<", Stat::Word},
        {">>", Stat::Word},
    },
};

//TODO: Dodaj definiowanie typów zmiennych
//TODO: DONE Zmodyfikuj taby w pannelach (Ustawiono w konstruktorze)
//TODO: Może dodaj dopisywanie znacznikow i ich podpowiadanie
//TODO: DONE Lepsza kontrola kopiowanego tekstu: ten sam font, dodatkowe znaki (Wyłaczenie RichTextow)
//TODO: DONE Zrobić domyślne przekierowania do domyślnych reguł nieokreślonych dla pustych reguł DONE Jesli procedura nie ma regul onEndOfCall, to pobieram reguly nieznanych procedur

//WARNING: Zawsze dodawaj String Literal'y z toLower
//TODO: Dodać ostrzeżenia o braku EndOfCall z RulesForArgument i rulesForUnspeicifiedArgument
 const QList<QString> TclProcedureInterpreter::userInteractionMap
{
    QStringLiteral("NotRequired").toLower(),
    QStringLiteral("Required").toLower()
};

 const QList<QString> Rule::controlMap
{
    QStringLiteral("BreakRuleCheck").toLower(),
    QStringLiteral("NoBreakRuleCheck").toLower(),
    //{QStringLiteral("BreakRuleCheckDontExecOnEndActions").toLower(), RuleControlFlag::BreakRuleCheckDontExecOnEndActions}
};

const QList<QString> Action::conditionalMap
{
    QStringLiteral("CompareNumbOfArguments").toLower(),
    QStringLiteral("IsLastSavedStat").toLower(),
    QStringLiteral("Compare").toLower()
};

 const QList<QString> Action::executableMap
{
    QStringLiteral("Write").toLower(),
    QStringLiteral("TclParse").toLower(),
    QStringLiteral("Error").toLower(),
    //QStringLiteral("ChangeLastSavedStat").toLower(),
    //QStringLiteral("AddFunctionDefinition").toLower(),
    QStringLiteral("AddPreExpression").toLower(),
    QStringLiteral("AddUserInteraction").toLower(),
    QStringLiteral("CompareAndWrite").toLower(),
    //{QStringLiteral("FinalizeForEach"), Action::Executable::FinalizeForEach},  // private
    //{QStringLiteral("AddSnprintf"), Action::Executable::AddSnprintf},// private
};

const QList<QString> Format::ruleMap
{
    QStringLiteral("NameOrIndex").toLower(),
    QStringLiteral("ArgumentsFrom").toLower(),
    QStringLiteral("Separator").toLower(),
    QStringLiteral("Target").toLower(),
};

 const QList< QString> Format::targetMap
{
    QStringLiteral("Raw").toLower(),
    QStringLiteral("TclFormat").toLower(),
    QStringLiteral("CaplFormat").toLower(),
    QStringLiteral("ParametersStat").toLower(),
    //{QStringLiteral("Command").toLower(), FormatTarget::Raw},
    QStringLiteral("SnprintfFormat").toLower(),
};


// StartUp Error for Keywords Map
template <>
StartUpErrorBase::Message StartUpError<KeywordsController>::__startupErrorChecking(){
    using Keywords = KeywordsMap::Iterator;
    using Keyword = KeywordsMap::value_type::Iterator;
    KeywordsMap::size_type size = 0;
    for(Keywords keywords = KeywordsController::keywordsMap.begin(); keywords < KeywordsController::keywordsMap.end(); keywords++){
        if(keywords->isEmpty())
            return QString("Empty Keyword Container");
        if(size >= keywords->at(0).keyword.size())
            return QString("KeyWords Map Order Error");
        size = keywords->at(0).keyword.size();
        for(Keyword keyword = keywords->begin() + 1; keyword < keywords->end(); keyword++){
            if(keyword->keyword.size() != size)
                return QString("Keywords size (" + QString::number(size)  +") Error: " + keyword->keyword + "(" + QString::number(keyword->keyword.size()) + ")");
        }
    }

    return QString();
}

// Implemented Interpret Functions

//constexpr
/*
bool TCLInterpreter::checkInterpretFunctions(){
    constexpr Stat implementedStats[] = {
        Stat::Word,
        Stat::VariableSubbing,
        Stat::CommandSubbingStart,
        Stat::CommandSubbingEnd,
        Stat::BracesStart,
        Stat::Braces,
        Stat::DoubleQuotes,
        Stat::Whitespace,
        Stat::Namespace,
        Stat::Semicolon,
        Stat::Comment,
        Stat::EndOfString,
        //Stat::Script,
        //Stat::EndOfCodeBlock,
        Stat::BackslashSubbing,
        //Stat::Expression
    };
    static_assert (std::extent_v<decltype (implementedStats)> != 0, "No Implemented Stats");
    static_assert (std::extent_v<decltype (implementedStats)> <=  static_cast<std::underlying_type_t<Stat>>(Stat::Size) - 1, "Too many implemented stats");

    using InterpretFunctionIter = InterpretFunction*;
    using StatIter = const Stat*;
    StatIter endOfImplementedStats = implementedStats + static_cast<std::underlying_type_t<Stat>>(Stat::Size);
    for(StatIter statIter = implementedStats;
        statIter < endOfImplementedStats; statIter++){
        if(interpretFunctions[static_cast<std::underlying_type_t<Stat>>(*statIter)] == nullptr)
            return false;
    }

    return true;
}*/
using ProcedureDefinition = TclCommand_NS::Definition;
const TclCommand_NS::Definition::Rule defaultRuleForUnknownProcedureDefinition_onEndOfCall =
{   // Rule 1
    { // Conditions

    },
    {   // Action 1: For Any Procedure write call like ProcedureName(args, ...); // Default Call
        {
            ProcedureDefinition::Action::Executable::Write,
            QStringList{    // Arguments
            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
            "=",
            "(",
            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
            "@, ",
            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
            ">0",
            ")"
            }
        },
        {   // Action 2: Add to FunctionDefinitions
            ProcedureDefinition::Action::Executable::AddFunctionDefinition,
            {}
        }
    }
};

TclCommand_NS::CommandDefinitions TclProcedureInterpreter::defaultProcedureDefinitions = {
    {
        "set",
        {

        },
        {

        },
        {
            {   // Rule 1 - Error if 0 or 1 arguments
                {   // List of Conditions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {
                            QString::number(0),
                            QString::number(1)
                        }
                    }
                },
                {   // List of Executables
                    {
                        ProcedureDefinition::Action::Executable::Error,
                        {
                            QString("Set Procedure: Illegal number of arguments: 0 or 1")
                        }
                    }
                }
            },
            {   // Rule C2  - Obsługa
                {

                },
                {
                    {
                        ProcedureDefinition::Action::Executable::AddPredefinition,
                        {}
                    },
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "",
                            "T0",
                            "",
                            "=0",
                            "",
                            "T2",
                            " = ",
                            "",
                            "@ ",
                            "",
                            ">1"
                        }
                    }
                }
            }
        }
    },
    {

        "continue",
        {   // Rules for Arguments

        },
        {   // Rules for Unspecified Argument

        },
        {   // Rules on End of Procedure Call
            {   // Rule 1
                {   // Conditions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {
                            QString::number(0)
                        }
                    }

                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "continue",
                        }
                    }
                }
            },
            {   // Rule 2 - Number of arguments > 0
                {
                    // No Conditions
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Error,
                        {
                            QString("Continue Procedure: Number of Arguments cant be greater then 0.")
                        }
                    }
                }
            }
        }
    },
    {
        "return",
        {   // Rules for Arguments

        },
        {   // Rules for Unspecified Argument

        },
        {   // Rules on End of Procedure Call
            {   // Rule 1: Number of arguments == 1
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(0)}
                    }
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "return",
                        }
                    }
                }
            },
            {   // Rule 2: Any number of Arguments
                {}, // No conditions
                {
                    {   // Action Write "= =1;"
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "return ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0"
                        }
                    }
                }
            }
        }
    },
    {
        "delay",
        {   // Rules for Arguments

        },
        {   // Rules for Unspecified Argument

        },
        {   // Rules on End of Procedure Call
            {   // Rule 1
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {
                            QString::number(1)
                        }
                    }
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "testWaitForTimeout",
                            "(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            ")"
                        }
                    }
                }
            },
            {   // Rule 2
                {

                },
                {   // Error
                    {
                        ProcedureDefinition::Action::Executable::Error,
                        {
                            QString("Delay Procedure: Only 1 argument is expected.")
                        }
                    }
                }
            }
        }
    },
    // End of Definition ================================================,
    {
        "call",
        {   // Rules for Arguments

        },
        {   // Rules for Unspecified Argument

        },
        {   // Rules on End of Procedure Call
            {   // Rule 1 - Error on No Arguments
                {
                    {
                       ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {
                            QString::number(0)
                        }
                    }
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Error,
                        {
                            QString("Call Procedure - Illegal number of arguments: 0")
                        }
                    }
                }
            },
            {   // Rule 2
                {

                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::CaplFormat),
                            "(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "@, ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ">1",
                            ")"
                        }
                    }
                }
            }
        }
    },
    {
        "open", UserInteraction::Required,

    },
    // End of Definition ================================================,
    {
        "close", UserInteraction::Status::Required,

    },
    // End of Definition ================================================,
    {
        "eval", UserInteraction::Status::Required,

    },
    // End of Definition ================================================,
    /*{
        "expr",
        {   // Dynamic Rules

        },
        {   // U
            {   // Dynamic

            },
            {   // On Move
                {
                    {   // Conditions
                        {
                            ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                            {
                               QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Braces))
                           },
                        }
                    },
                    {   // Executables
                        {// Parse [expr_parser =-1]
                           ProcedureDefinition::Action::Executable::TclParse,
                           {"expr_parser ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ">0",
                           }
                       },
                    }
                }
            }
        },
        {   // On End of Call
            {   // Rule 1: Tcl Parse -> write Result
                {
                 // No conditions
                },
                 {
                    {// Parse [expr_parser =-1]
                       ProcedureDefinition::Action::Executable::Write,
                       {
                        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         "@ ",
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                        ">0",
                       }
                   },
                 }
            }
        }

    },
    // End of Definition ================================================,
    {
        "expr_parser",
        {   // Dynamic Rules

        },
        {   // On Move Rules

        },
        {   // On End of Call
            {   // Rule 1: Tcl Parse -> write Result
                {
                 // No conditions
                },
                 {
                     {// Parse [expr_parser =-1]
                        ProcedureDefinition::Action::Executable::Write,
                        {
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                          "@ ",
                          ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         ">0",
                        }
                    },
                 }
            }
        }

    },
    // End of Definition ================================================,
    {
        "string",
        {   // Dynamic Rules

        },
        {   // On Move Rules

        },
        {   // On End of Call            
            {   // Rule: No condtions -> As Unspecified Procedure Definition
                { // Conditions

                },
                {   // Action 1: For Any Procedure write call like ProcedureName(args, ...); // Default Call
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        QStringList{    // Arguments
                        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                        "=",
                        "(",
                        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                        "@, ",
                        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                        ">0",
                        ")"
                        }
                    },
                    {   // Action 2: Add to FunctionDefinitions
                        ProcedureDefinition::Action::Executable::AddFunctionDefinition,
                        {}
                    }
                }
            }
        }
    },*/
    // End of Definition ================================================,
    {
        "file",
        {   // Dynamic Rules
        },
        {   // On move
        },
        {   // On Finalize
            {   // Rule 1: Only 2 arguments -> 1 argument == SIZE - > Write fileSize(==1)
                {   // Condtions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(2)}
                    },
                    {
                        TclProcedureInterpreter::newCompareRule(
                        {"size"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0"
                        }
                        ),
                    }
                },
                {   // Actions
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "fileSize(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=1",
                            ")"
                        }
                    }
                }
            },
            {   // Rule 2: Only 2 arguments -> 1 argument == SIZE - > Write fileSize(==1)
                {   // Condtions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(2)}
                    },
                    {
                        TclProcedureInterpreter::newCompareRule(
                        {"exists"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0"
                        }
                        ),
                    }
                },
                {   // Actions
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "fileExists(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=1",
                            ")"
                        }
                    }
                }
            },
            {   // Rule 3:
                {}, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {}
                    }
                }
            }
        }

    },
    // End of Definition ================================================,
    {
        "incr",
        {   // Dynamic Rules
        },
        {   // On move
        },
        {   // On Finalize
            {   // Rule 1:
                {   // Condtions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(2)}
                    },
                },
                {   // Actions
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            " += ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=1",

                        }
                    }
                }
            },
            {   // Rule 2:
                {   // Condtions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(1)}
                    },
                },
                {   // Actions
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            "++"
                        }
                    }
                }
            },
            {   // Rule 3:
                {}, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {}
                    }
                }
            }
        }

    },
    // End of Definition ================================================,
    {
        "llength",
        {   // Dynamic Rules
        },
        {   // On move
        },
        {   // On Finalize
            {   // Rule 1:
                {   // Condtions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(1)}
                    },
                },
                {   // Actions
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "elcount(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            ")"
                        }
                    }
                }
            },
            {   // Rule 2:
                {}, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {}
                    }
                }
            }
        }

    },
    // End of Definition ================================================,
    {
        "puts",
        {   // Dynamic Rules
        },
        {   // On move
        },
        {   // On Finalize
            {   // Rule 1:
                {   // Condtions
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(1)}
                    },
                },
                {   // Actions
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "write(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            ")"
                        }
                    }
                }
            },
            {   // Rule 3:
                {}, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {}
                    }
                }
            }
        }

    },
    // End of Definition ================================================,
    {
        "stc_section",
        {   // Rules for Arguments
            {   // Argument 1
                {   // Dynamic Rules

                },
                { // Rules on Move
                  { // Rule 1: if RESULT -> change to EXPECTED
                    {   // Conditions
                        TclProcedureInterpreter::newCompareRule(
                        {"RESULT"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            "",
                            "=-1"
                        }
                        ),
                    },
                    {   // Actions
                        {   // Change Argument 1 to EXPECTED
                            ProcedureDefinition::Action::Executable::Write,
                            {
                                "EXPECTED"
                            }
                        }
                    }
                  }
                }
            }
        },
        {   // Rules for Unspecified Argument

        },
        {   // Rules on End of Procedure Call
            {   // Rule 1
                {

                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            "(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::CaplFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=1",
                            ")"
                        }
                    }
                }
            }
        }
    },
    // End of Definition ================================================,
    {   // Definition --------------------------------------------------------
        "if",
        {   // Rules for Arguments
            {   // Argument 1
                {   // Dynamic Rules
                    {   // Rule 1: If lastSavedStat stat == List or EndOfList or FunctionCall or Whitespace -> Do nothing (Break)
                        {
                            {
                             ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                             {
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))
                                }
                            },
                        }
                    },
                    {   // Rule 2: Error if no rules have been executed
                        {
                            // No Conditions
                        },
                        {
                            {   // Error
                                ProcedureDefinition::Action::Executable::Error,
                                {
                                    "\"If\" conditional expression isnt list"
                                }
                            }
                        }
                    }
                },
                {   // Rules on moveArgument
                    { // Rule 1
                       {
                        // No conditions
                       },
                       {
                           {// Parse [expr_parser =-1]
                               ProcedureDefinition::Action::Executable::TclParse,
                               {"expr_parser ",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-1",
                               }
                           },
                           {   // Write as ( =-1 )
                               ProcedureDefinition::Action::Executable::Write,
                               {
                                   "( ",
                                   ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                   ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                       ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                   ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                   "=-1",
                                   " )"
                               }
                           }
                        }
                    },
                    {
                        {},
                        {
                            {// Parse [expr_parser =-1]
                             ProcedureDefinition::Action::Executable::Error,
                             {"No rules have been executed for Argument 1"}
                            },
                        }
                    }
                }
            },
            {   // Argument 2
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                           }
                        }
                    },
                    {   // Rule 2: On lastSavedStat == List -> Change to CodeBlock
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {//QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfString)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BackslashSubbing)),}
                            },
                        },
                        {

                        }
                    },
                    {
                        {

                        },
                        {
                            {// Parse [expr_parser =-1]
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 2"}
                            },
                        }
                    }
                },
                {   // Rules on moveArgument
                    {   // Rule 1: Write as { =-1 }
                        {
                            // No Connditions
                        },
                        {
                            {
                                ProcedureDefinition::Action::Executable::Write,
                                {
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    "=-1",
                                }
                            }
                        }
                    },
                    {
                        {

                        },
                        {
                            {// Parse [expr_parser =-1]
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 2"}
                            },
                        }
                    }
                }
            }
        },
        {   // Rules for Unspecified Argument
            {   // Dynamic Rules
                {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                    {
                        {
                          ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                        },
                        {
                            TclProcedureInterpreter::newCompareRule(
                            {"else"},
                            {
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-1"
                            }
                            ),
                        },
                    },
                    {
                       {    // Action 1: Change stat to CodeBlock
                             ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                       }
                    }
                },
                {   // Rule 2: On lastSavedStat == List -> Change to CodeBlock
                    {
                        {
                          ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                        },
                        {
                            TclProcedureInterpreter::newCompareRule(
                            {"elseif"},
                            {
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-2"
                            }
                            ),
                        },
                    },
                    {
                       {    // Action 1: Change stat to CodeBlock
                             ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                       }
                    }
                }
            },
            {   // Rules on moveArgument
                {   // Rule 1: if lastSavedStat command == (elseif) -> Error
                    {
                        TclProcedureInterpreter::newCompareRule(
                        {"elseifelseif"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                             "=-2",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                             "=-3"
                        }
                        ),
                    },
                    {
                        {// Error
                            ProcedureDefinition::Action::Executable::Error,
                            {"Elseif elseif sequance"}
                        }
                    }
                },
                {   // Rule 2: if lastSavedStat command == (elseif) -> P
                    {
                        TclProcedureInterpreter::newCompareRule(
                        {"elseif"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=-2"
                        }
                        ),
                    },
                    {
                        { // Parse [expr_parser =-1]
                           ProcedureDefinition::Action::Executable::TclParse,
                           {"expr_parser ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=-1",
                           }
                       },
                        // Write as ( =-1 )
                        {
                            ProcedureDefinition::Action::Executable::Write,
                            {
                                "( ",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-1",
                                " )"
                            }
                        }
                    }
                },
                {   // Rule 3: If lastSavedStat command == else -> {\n =-1 \n}
                    {
                        TclProcedureInterpreter::newCompareRule(
                        {"else"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                             "=-2"
                        }
                        ),
                    },
                    {
                        {
                            ProcedureDefinition::Action::Executable::Write,
                            {

                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-1",
                            }
                        }
                    }
                },
                {   // Rule 4: If PreLastSavedStat command == else -> {\n =-1 \n}
                    {
                        TclProcedureInterpreter::newCompareRule(
                        {"elseif"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=-3"
                        }
                        ),
                    },
                    {
                        {
                            ProcedureDefinition::Action::Executable::Write,
                            {
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-1",
                            }
                        }
                    }
                }
            }
        },
        {   // Rules on End of Call
            {   // Rule 1

                {
                    // No conditions
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ">0"
                        }
                    }
                }

            }
        }
    },  // End of Definition ================================================,
    {
        "for",
        {   // Rules for Arguments
            {   // Argument 1
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to Expression
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
//                            Commented but required
//                           {    // Action 1: Change stat to Expression
//                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
//                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Expression))}
//                           }
                        }
                    },
                    {   // Rule 2: On Ignore
                        {   // Conditions: Ignore
                            {   // Condition 1:  Ignore
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {//QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                 //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing)),}
                            },
                        },
                        {

                        }
                    },
                    {
                        {

                        },
                        {
                            {//
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 1 (for loop)"}
                            },
                        }
                    }
                },
                {   // Rules on moveArgument
                    {   // Rule 1: Write as { =-1 }
                        {
                            // No Connditions
                        },
                        {
                            {
                                ProcedureDefinition::Action::Executable::Write,
                                {
                                    "",
                                    "=-1",
                                }
                            }
                        }
                    },
                    {
                        {

                        },
                        {
                            {// Parse [expr_parser =-1]
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 1 (for loop)"}
                            },
                        }
                    }
                }
            },
            {   // Argument 2
                {   // Dynamic Rules
                    {   // Rule 1: If lastSavedStat stat == List or EndOfList or FunctionCall or Whitespace -> Do nothing (Break)
                        {
                            {
                             ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                             {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart)),
                              //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Braces)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BackslashSubbing)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Word)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing)),
                                //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingSnprintf)),
                               // QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingString)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfString)),
                               // QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                }
                            },
                        }
                    },
                    {   // Rule 2: Error if no rules have been executed
                        {
                            // No Conditions
                        },
                        {
                            {   // Error
                                ProcedureDefinition::Action::Executable::Error,
                                {
                                    "\"for\" second expression isnt list"
                                }
                            }
                        }
                    }
                },
                {   // Rules on moveArgument
                    {
                       {
                        // No conditions
                       },
                        {
                            {// Parse [expr_parser =-1]
                               ProcedureDefinition::Action::Executable::TclParse,
                               {"expr_parser ",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-1",
                               }
                           },
                        },

                    },
                    {
                        {// No conditions
                        },
                        {
                            {// Parse [expr_parser =-1]
                             ProcedureDefinition::Action::Executable::Error,
                             {"No rules have been executed for Argument 2"}
                            }
                        }
                    }
                },
            },
            {   // Argument 3
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to Expression
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
//                            Commented but required
//                           {    // Action 1: Change stat to Exxpression
//                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
//                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Expression))}
//                           }
                        }
                    },
                    {   // Rule 2: On Ignore
                        {   // Conditions:  Ignore
                            {   // Condition 1:  Ignore
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Braces)),
                                 //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore)),
                                //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfString)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BackslashSubbing))}
                            },
                        },
                        {

                        }
                    },
                    {
                        {

                        },
                        {
                            {//
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 3 (for loop)"}
                            },
                        }
                    }
                },
                {   // Rules on moveArgument
                    {   // Rule 1: Write as { =-1 }
                        {
                            // No Connditions
                        },
                        {
                            {
                                ProcedureDefinition::Action::Executable::Write,
                                {
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    "=-1"
                                }
                            }
                        }
                    },
                    {
                        {

                        },
                        {
                            {// Parse [expr_parser =-1]
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 3 (for loop)"}
                            },
                        }
                    }
                }
            },
            {   // Argument 4
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                           }
                        }
                    },
                    {   // Rule 2: On Ignore -> Ignore
                        {   // Conditions:  Ignore
                            {   // Condition 1:  Ignore
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {
                                    //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                // QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore))}
                            },
                        },
                        {

                        }
                    },
                    {
                        {

                        },
                        {
                            {//
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 4 (CodeBlock)"}
                            },
                        }
                    }
                },
                {   // Rules on moveArgument
                    {   // Rule 1: Write as { =-1 }
                        {
                            // No Connditions
                        },
                        {
                            /*{
                                ProcedureDefinition::Action::Executable::Write,
                                {
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    "=-1",
                                }
                            }*/
                        }
                    },
                    {
                        {

                        },
                        {
                            {// Parse [expr_parser =-1]
                               ProcedureDefinition::Action::Executable::Error,
                               {"No rules have been executed for Argument 4"}
                            },
                        }
                    }
                },
            }
        },
        {   // Unspecified Arguments

        },
        {   // On End of Call
            {   // Rule 1
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {
                            QString::number(4)
                        }
                    }
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=",
                            "(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            "; ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=1",
                            "; ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::CaplFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=2",
                            ")",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=3"
                        }
                    }
                }
            }
        }
    },
    // End of Definition ================================================,
     /*{
         "foreach",
         {   // Rules for Arguments
             {  // Argument 1
                {   // Dynamic Rules

                },
                {   // On move Rules
                    {   // Rule 1: If parameter stat == String or PendingString -> Do nothing (OK)
                        {   // Conditions
                            {
                                TclProcedureInterpreter::newCompareRule(
                                // Strings to Compare
                                {
                                    TCLInterpreter::cast_stat_str(Stat::String),
                                    TCLInterpreter::cast_stat_str(Stat::PendingString),
                                    TCLInterpreter::cast_stat_str(Stat::Braces),

                                },
                                // Format to get string to compare
                                {
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                                    + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::ProcedureParametersStat),
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::INDEX_OR_FULL_LINE)
                                    + QString("-1")
                                }
                                )
                            },
                            {}//No Actions
                        },
                        {   // Rule 2: On no Rule 1 throw Error
                            { },//No Conditions
                            {   // Actions
                                {
                                    ProcedureDefinition::Action::Executable::Error,
                                    {
                                        QString("Definition ForEach - Argument 1 - On move argument Rules: Argument 1 isnt a variable.")
                                    }
                                }
                            }
                        }
                    }
                }
             },
             {   // Argument 2
                {   // Dynamic Rules

                },
                {   // On move Rules
                    {   // Rule 1: If parameter stat == Variable -> Do nothing (OK)
                        {   // Conditions
                            {
                                TclProcedureInterpreter::newCompareRule(
                                // Strings to Compare
                                {
                                    TCLInterpreter::cast_stat_str(Stat::StringInQuotes),
                                    TCLInterpreter::cast_stat_str(Stat::Braces),
                                    TCLInterpreter::cast_stat_str(Stat::DoubleQuotes),
                                    TCLInterpreter::cast_stat_str(Stat::PendingSnprintf),
                                    TCLInterpreter::cast_stat_str(Stat::Variable),
                                    TCLInterpreter::cast_stat_str(Stat::Snprintf),
                                    TCLInterpreter::cast_stat_str(Stat::CommandSubbing),
                                },
                                // Format to get string to compare
                                {
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                                    + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::ProcedureParametersStat),
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::INDEX_OR_FULL_LINE)
                                    + QString("-1")
                                }
                                )
                            },

                        },
                        {}//No Actions
                    },
                    {   // Rule 2: On no Rule 1 throw Error
                        {  }, //No Conditions
                        {   // Actions
                            {
                                ProcedureDefinition::Action::Executable::Error,
                                {
                                    QString("Definition ForEach - Argument 2 - On move argument Rules: Argument 2 isnt a variable.")
                                }
                            }
                        }
                    }
                 }
             },
             {   // Argument 3
                 {   // Dynamic Rules
                     {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                         {   // Conditions:  If SavedStat stat == List
                             {   // Condition 1:  If SavedStat stat == List
                                 ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                             },
                         },
                         {
                            {    // Action 1: Change stat to CodeBlock
                                  ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                            }
                         }
                     },
                     {   // Rule 2: On Ignore
                         {   // Conditions:  Ignore
                             {   // Condition 1:  Ignore
                                 ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                  QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                 QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script)),
                                 QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                 QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore))}
                             },
                         },
                         {

                         }
                     },
                     {
                         {

                         },
                         {
                             {//
                                ProcedureDefinition::Action::Executable::Error,
                                {"No rules have been executed for Argument 3 (CodeBlock)"}
                             },
                         }
                     }
                 },
                 {   // Rules on moveArgument
                     {   // Rule 1: Write as { =-1 }
                         {
                             // No Connditions
                         },
                         {
                             {
                                 ProcedureDefinition::Action::Executable::Write,
                                 {
                                     "",
                                     "=-1",                                     
                                 }
                             }
                         }
                     },
                     {
                         {

                         },
                         {
                             {// Parse [expr_parser =-1]
                                ProcedureDefinition::Action::Executable::Error,
                                {"No rules have been executed for Argument 3"}
                             },
                         }
                     }
                 }
             }
         },
         {   // Unspecified Arguments

         },
         {
            /* previous definition
            // On End of Call
             {   // Rule 1

                 {
                     // No conditions
                 },
                 {
                     {
                         ProcedureDefinition::Action::Executable::FinalizeForEach,
                         {}
                     }
                 }
             }
                         *//*
            {
                {   // No conditions

                },
                {
                    /*
                    [First release 0.9.0] Change rule for ForEach Procedure to form:
                    //"/* ForEach Procedure Parsing Temporarly Disabled\n
                      <procedure_name> <first_arg> <second_arg>\n changed
                     to if(false) with parseable block expression*/
                    //\n if(false)\n{<parsed_block_expr>}"
                    /*
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            "/* ForEach Procedure Parsing Temporarly Disabled\n",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::INDEX_OR_FULL_LINE)
                            + QString(""),
                            " ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::INDEX_OR_FULL_LINE)
                            + QString("0"),
                            " ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::INDEX_OR_FULL_LINE)
                            + QString("1"),
                            "\n changed to if(0) with parseable block expression*//*\nif(0)\n",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::CaplFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::INDEX_OR_FULL_LINE)
                            + QString("2")
                        }
                    }
                }
            }
         }
     } */
};


TclCommand_NS::Definition TclCommand_NS::Definition::defaultUnknownProcedureDefinition =
ProcedureDefinition(
    // NAME - For Default Procedure Definition: No Name specified
    "",
    // RULES FOR ARGUMENTS
    {   // RULES FOR ARGUMENTS
        /*{   // 1 Argument Rules
            {   // List of Rules
                {   // On List Stat "{" ->

                }
            }
        }*/
    },

    // RULES ON Move
    {

    },
    {   // Rules on End
        defaultRuleForUnknownProcedureDefinition_onEndOfCall
    }
);


//void TCLInterpreterPriv::ListStatInfo::newList(Command command){
//    if(lastListInfo){
//        if(!command.isEmpty())
//            lastListInfo->listInfos.append(ListInfo{{}, command});

//        lastListInfo->listInfos.append(ListInfo{});
//        lastListInfo = lastListInfo->listInfos.end() - 1;
//    }else{
//        if(!command.isEmpty())
//            listInfos.append(ListInfo{{}, command});

//        listInfos.append(ListInfo{});
//        lastListInfo = listInfos.end() - 1;
//    }

//}

//TCLInterpreterPriv::ListStatInfo::EndListResult
//TCLInterpreterPriv::ListStatInfo::endList(Command command){
//    if(lastListInfo){
//        if(!command.isEmpty())
//            lastListInfo->listInfos.append(ListInfo{{}, command});

//        ListInfos::Iterator prelastListInfo = listInfos.end() - 1;
//        if(prelastListInfo == lastListInfo){
//            lastListInfo = nullptr;
//        }else{
//            while(prelastListInfo->listInfos.end() - 1 != lastListInfo)
//                prelastListInfo = prelastListInfo->listInfos.end() - 1;
//            lastListInfo = prelastListInfo;
//        }
//    }else{
//        if(!command.isEmpty())
//            listInfos.append(ListInfo{{}, command});

//        return EndListResult::EndOfList;
//    }
//    return EndListResult::NoEndOfList;
//}

//QString TCLInterpreterPriv::ListStatInfo::ListInfo::toString(const QString& separator)
//{
//    using Info = ListInfos::Iterator;
//    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;

//    if(strData.isEmpty()){
//    QString str;

//    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

//    /*while(!loopControlData.isEmpty()){
//        if(loopControlData.last().first < loopControlData.last().second)
//        {
//            Info info = loopControlData.last().first;
//            if(info->strData.isEmpty()){
//                loopControlData.append(LoopControlData(info->listInfos.begin(), info->listInfos.end()));
//                loopControlData.
//            }else{

//            }
//        }else{
//            loopControlData.removeLast();
//        }
//    }*/
//    if(!listInfos.isEmpty()){
//        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
//            str += info->toString(separator) + separator;
//        str.chop(separator.length());
//    }

//        return QString("{") + str + "}";
//    }else{
//        return strData;
//    }
//}

void TCLInterpreter::addEmptyLine(){
    using InterpreterMode = Tcl::Interpreter::Command::Settings::InterpreterMode;
    const QString ERROR_DESCRIPTION = QString("addEmptyLine: ");
 /*  if(userConfig.proceduresSettings().mode() != InterpreterMode::TestCaseReport){
        if(savedStats().isEmpty())
        {
            throwError(ERROR_DESCRIPTION + "No saved stats");
            if(processError() == Error::NoError)
                clearError();
            return;
        }
        switch(lastSavedStat().stat()){
        case Stat::MainScript:
            addExpressionToMainCodeBlock({" "});
            break;
        case Stat::Script:
            addExpressionToCodeBlock({" "});
            break;
        case Stat::BracesStart:
            lastSavedStat().appendCommand("\n");
            break;
        default:
        {

            throwError(ERROR_DESCRIPTION + "Wrong saved stat");
            if(processError() == Error::NoError)
                clearError();
            return;
        }
        }
    }*/
}

//QString TCLInterpreterPriv::ListStatInfo::ListInfo::toCaplListString()
//{
//    using Info = ListInfos::Iterator;
//    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;
//    const QString separator = QString("");

//    if(strData.isEmpty()){
//        QString str;

//    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

//    /*while(!loopControlData.isEmpty()){
//        if(loopControlData.last().first < loopControlData.last().second)
//        {
//            Info info = loopControlData.last().first;
//            if(info->strData.isEmpty()){
//                loopControlData.append(LoopControlData(info->listInfos.begin(), info->listInfos.end()));
//                loopControlData.
//            }else{

//            }
//        }else{
//            loopControlData.removeLast();
//        }
//    }*/
//    if(!listInfos.isEmpty()){
//        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
//            str += info->toString(separator) + separator;
//        str.chop(separator.length());
//    }
//        return str;
//    }else{
//        return strData.trimmed();
//    }
//}

//QString TCLInterpreterPriv::ListStatInfo::toCaplListString()
//{
//    using Info = ListInfos::Iterator;
//    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;
//    const QString separator = QStringLiteral(", ");

//    QString str;
//    QString tempStr;

//    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

//    /*while(!loopControlData.isEmpty()){
//        if(loopControlData.last().first < loopControlData.last().second)
//        {
//            Info info = loopControlData.last().first;
//            if(info->strData.isEmpty()){
//                loopControlData.append(LoopControlData(info->listInfos.begin(), info->listInfos.end()));
//                loopControlData.
//            }else{

//            }
//        }else{
//            loopControlData.removeLast();
//        }
//    }*/
//    if(!listInfos.isEmpty()){
//        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
//            if(!(tempStr = info->toCaplListString()).isEmpty())
//                str += QString("\"") + tempStr + "\"" + separator;
//        if(str.endsWith(separator))
//            str.chop(separator.length());
//    }

//    return QString("{") + str + "}";

//}

//QString TCLInterpreterPriv::ListStatInfo::toString(const QString& separator)
//{
//    using Info = ListInfos::Iterator;
//    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;
//    QString str;

//    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

//    /*while(!loopControlData.isEmpty()){
//        if(loopControlData.last().first < loopControlData.last().second)
//        {
//            Info info = loopControlData.last().first;
//            if(info->strData.isEmpty()){

//            }else{

//            }
//        }else{
//            loopControlData.removeLast();
//        }
//    }*/
//    if(!listInfos.isEmpty()){
//        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
//            str += info->toString(separator) + separator;
//        str.chop(separator.length());
//    }
//    return str;
//}

QString SavedStat::listToCaplString(){
    QString str = "{";
//    if(info){
//        str += info->toCaplListString();
//    }else{
        QStringList elements = _command.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for(QStringList::Iterator element = elements.begin(); element < elements.end(); element++)
            *element = QString("\"") + *element + "\"";
        str += elements.join(", ");
//    }
    return str + "}";
}


QString TclCommand_NS::Call::Parameter::toString(ProcedureDefinition::Format::Target target){
    using Target = ProcedureDefinition::Format::Target;

    switch(target){
    case Target::Raw:
       return rawCommand();
    case Target::ProcedureParametersStat:
    {
        return QString::number(std::underlying_type_t<Stat>(stat()));
    }
    case Target::SnprintfFormat:
    {        
        switch(stat()){
        case Stat::Word:
        case Stat::DoubleQuotes:
        case Stat::ComplexWord:
            return QString("\"") + outputCommand() + "\"";
        case Stat::Script:
            return QString("{\n") + outputCommand() + "\n}\n";
        case Stat::Braces:
            //return savedStat().listToCaplString();
        default:
            return outputCommand();
        }
    }
    case Target::CaplFormat:
    {
        return outputCommand();
    }
    case Target::TclFormat:
    {
        using Parameter = Parameters::Iterator;
        return rawCommand();
    }
        break;
    default:
        break;
    }
    return QString();
}


Error TCLInterpreter::removeProcedureCall_writeOnlyProcedure(){
    commandsController.tryToDeactivateWriteOnlyProcedure();
    return removeProcedureCall_standard();
}

void TclCommandsController::addDefaultProcedureDefinitionsToUserProcedureDefintions(UserInputConfig& userDefinitions){

    if(!userDefinitions.userProcedureConfig().isEmpty()){
       for(CommandDefinitionIterator defaultDefinition = defaultProcedureDefinitions.begin();
           defaultDefinition < defaultProcedureDefinitions.end(); defaultDefinition++)
       {
           CommandDefinitionIterator definition;
           for(definition = userDefinitions.userProcedureConfig().begin();
               definition < userDefinitions.userProcedureConfig().end(); definition++)
           {
               if(defaultDefinition->name == definition->name){
                   // Rules For Arguments
                   using RulesForArgument = ProcedureDefinition::RulesForArguments::Iterator;
                   RulesForArgument loopEnd;
                   if(definition->rulesForArguments.size() < defaultDefinition->rulesForArguments.size()){
                       definition->rulesForArguments.resize(defaultDefinition->rulesForArguments.size());
                       loopEnd = definition->rulesForArguments.end();
                   }else{
                       loopEnd = definition->rulesForArguments.begin() + defaultDefinition->rulesForArguments.size();
                   }
                   for(RulesForArgument rulesForArgument = definition->rulesForArguments.begin(), defaultRulesForArgument = defaultDefinition->rulesForArguments.begin();
                       rulesForArgument < loopEnd; rulesForArgument++, defaultRulesForArgument++)
                   {
                       rulesForArgument->rules.append(defaultRulesForArgument->rules);
                       rulesForArgument->rulesOnMoveArgument.append(defaultRulesForArgument->rulesOnMoveArgument);
                   }

                   // Rules fo Unspecified Arguments
                   definition->rulesForUnspecifiedArgument.rules.append(defaultDefinition->rulesForUnspecifiedArgument.rules);
                   definition->rulesForUnspecifiedArgument.rulesOnMoveArgument.append(defaultDefinition->rulesForUnspecifiedArgument.rulesOnMoveArgument);

                   // Rules on End of Call
                   definition->rulesOnEndOfProcedureCall.append(defaultDefinition->rulesOnEndOfProcedureCall);

                   break;
               }
           }
           if(definition == userDefinitions.userProcedureConfig().end()){
               userDefinitions.userProcedureConfig().append(*defaultDefinition);
           }
       }
   }

    if(!userDefinitions.userDefaultProcedureConfig().isRulesEmpty()){
        // Rules For Arguments
        using RulesForArgument = ProcedureDefinition::RulesForArguments::Iterator;
        RulesForArgument loopEnd;
        if(userDefinitions.userDefaultProcedureConfig().rulesForArguments.size() < Definition::defaultUnknownProcedureDefinition.rulesForArguments.size()){
            userDefinitions.userDefaultProcedureConfig().rulesForArguments.resize(Definition::defaultUnknownProcedureDefinition.rulesForArguments.size());
            loopEnd = userDefinitions.userDefaultProcedureConfig().rulesForArguments.end();
        }else{
            loopEnd = userDefinitions.userDefaultProcedureConfig().rulesForArguments.begin() + Definition::defaultUnknownProcedureDefinition.rulesForArguments.size();
        }

        // Rules fo Unspecified Arguments
        userDefinitions.userDefaultProcedureConfig().rulesForUnspecifiedArgument.rules.append(Definition::defaultUnknownProcedureDefinition.rulesForUnspecifiedArgument.rules);
        userDefinitions.userDefaultProcedureConfig().rulesForUnspecifiedArgument.rulesOnMoveArgument.append(Definition::defaultUnknownProcedureDefinition.rulesForUnspecifiedArgument.rulesOnMoveArgument);

        // Rules on End of Call
       userDefinitions.userDefaultProcedureConfig().rulesOnEndOfProcedureCall.append(Definition::defaultUnknownProcedureDefinition.rulesOnEndOfProcedureCall);

    }
}
/*
template<>
Error TCLInterpreter::interpret<Stat::Word>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Word>: ";
    //SavedStat unknownStringStat;
    // Preconditions
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    // Initialize
    QString unknownString = readCurrentKeyword();

    // Processing
    // #1 ------------------------------------------------------------------
    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {
        if(commandsController.createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
        // Stat shall be replace by CommandSubbing in the CommandsController
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::Word) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;    
    case Stat::Ignore:
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown String Content: " + unknownString +
                          " Unknown Stat");
    }
    // #1# ------------------------------------------------------------------------------------
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::CommandSubbingStart>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::CommandSubbingStart>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::CommandSubbingStart) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::MainScript:
    {
        return throwError(ERROR_PREFIX + "CommandSubbingStart directly in script");
    }
    case Stat::Ignore:
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::CommandSubbingEnd>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::CommandSubbingEnd>: ";
    //bool ignoreMoveArgumentProcedure = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::CommandSubbingEnd) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::MainScript:
    {
       return throwError(ERROR_PREFIX + "CommandSubbingEnd in MainScript or Script");
    }
    case Stat::Ignore:
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}


template<>
Error TCLInterpreter::interpret<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::BracesStart>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {

        // New Procedure call  + Braces
        // Maybe create new Procedure first, then interpret as BracesStart
        if(commandsController.createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());

    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::BracesStart) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;    
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::Braces>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Braces>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {
        // New Procedure call  + Braces
        // Maybe create new Procedure first, then interpret as Braces
        if(commandsController.createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::Braces) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;    
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::DoubleQuotes>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {
        // New Procedure call  + DoubleQuotes
        // Maybe create new Procedure first, then interpret as DoubleQuotes
        if(commandsController.createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::DoubleQuotes) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}


template<>
Error TCLInterpreter::interpret<Stat::Whitespace>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Whitespace>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::Whitespace) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;
    default:
        break;
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::VariableSubbing>(){

    const QString ERROR_PREFIX = "TCL Interpreter <Stat::VariableSubbingStart>: ";
     if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {
        return throwError(ERROR_PREFIX + "VariableSubbingStart in MainScript ");
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::VariableSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::Namespace>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Namespace>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {
        // New Procedure call  + DoubleQuotes
        // Maybe create new Procedure first, then interpret as DoubleQuotes
        if(commandsController.createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::Namespace) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;    
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::Semicolon>(){

    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Semicolon>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::Semicolon) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
    {
        if(removeIgnore() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;

    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::Comment>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Comment>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {
        // Copy from dev branch + think about BackslashSubbing with newline
    }
        break;
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::Comment) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;

    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::EndOfString>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::EndOfString>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().filteredStat()){
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::Comment) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
    {
        if(removeIgnore() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::BackslashSubbing>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::BackslashSubbing>: ";
    QString specialSignStr;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    // Check and prepare specialSignStr
    if(textInterpreter().isCurrentChar()) // Accesible -> Analysis
    {
        bool whitespaceAfterSpecialSign = false;
        switch (textInterpreter().currentCharForSpecialSign()->toLatin1()) {
        case ' ':
        case '\t':
            specialSignStr = *textInterpreter().currentCharForSpecialSign();
            whitespaceAfterSpecialSign = true;
            break;
        case '\\':
        case '\'':
        case '\"':
        case 'a':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 'v':
        case 't':
            specialSignStr = QStringLiteral("\\") + *textInterpreter().currentCharForSpecialSign();
            break;
        default:
            specialSignStr = *textInterpreter().currentCharForSpecialSign();
        }

        // Finally _ Increment CurrentChar (Less Readable and safe, but more efficient and less work around)
        textInterpreter().incrementCurrentCharDueToSpecialSign();

        switch(lastSavedStat().stat()){

        case Stat::Ignore:
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat ");
        }
    }else{
        if(Error::Error == saveStatWithParsingControl({Stat::BackslashSubbing}))
            return throwError(ERROR_PREFIX + error());
    }

    switch(lastSavedStat().filteredStat()){
    case Stat::MainScript:
    {
        // New Procedure call  + DoubleQuotes
        // Maybe create new Procedure first, then interpret as DoubleQuotes
        if(commandsController.createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(Stat::BackslashSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;

    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

TCLInterpreterFunctions TCLInterpreter::interpretFunctions = {
    nullptr,    // None
    &TCLInterpreter::interpret<Stat::Word>,    // Word
    &TCLInterpreter::interpret<Stat::VariableSubbing>,
    &TCLInterpreter::interpret<Stat::CommandSubbingStart>,
    &TCLInterpreter::interpret<Stat::CommandSubbingEnd>,
    &TCLInterpreter::interpret<Stat::Braces>,
    &TCLInterpreter::interpret<Stat::Whitespace>,
    &TCLInterpreter::interpret<Stat::Namespace>,
    &TCLInterpreter::interpret<Stat::Semicolon>,
    &TCLInterpreter::interpret<Stat::Comment>,
    &TCLInterpreter::interpret<Stat::EndOfString>,
    &TCLInterpreter::interpret<Stat::BackslashSubbing>, // BackslashSubbing
    nullptr,    // MainScript
    nullptr,    // Ignore
    &TCLInterpreter::interpret<Stat::BracesStart>,
    &TCLInterpreter::interpret<Stat::DoubleQuotes>,

};
//static_assert (TCLInterpreter::checkInterpretFunctions() == true, "Wrong InterpretFunctions");
*/
Error KeywordsController::initialize(const QString &strToAnalyze){
    currentChar = savedFirstKeywordCharPos = beginOfString = savedChar = strToAnalyze.constBegin();
    endOfString = strToAnalyze.constEnd();
    lastKeywordLength = 0;
    return Error::NoError;
}

Error KeywordsController::deinitialize(){

    return Error::NoError;
}

ReadIgnoreResult KeywordsController::readIgnoreUntilPriv()
{
    while(currentChar < endOfString){
        if(ignoreReadUntilCondtions.contains(readCharacter(currentChar)))
            return ReadIgnoreResult::Satisfied;
        currentChar++;
    }
    return ReadIgnoreResult::EndOfString;
}

ReadIgnoreResult KeywordsController::readIgnoreIfPriv()
{
    while(currentChar < endOfString){
        if(!ignoreReadUntilCondtions.contains(readCharacter(currentChar)))
            return ReadIgnoreResult::Satisfied;
        currentChar++;
    }

    return ReadIgnoreResult::EndOfString;
}

CheckingResult KeywordsController::checkingPriv(){
    int index = currentChar - savedFirstKeywordCharPos;
    while(currentChar < endOfString){
        // Check current stat
        //index = currentChar - savedFirstKeywordCharPos;
        while(lastKeywordLength > 0){
            if(currentChar < endOfString){
                if(index < lastKeywordLength){
                    if(currentChar->toLower() == lastKeyword->keyword.at(index)){
                        currentChar++, index++;
                    }else{
                        lastKeywordLength = 0;  // KeyWord dont match, find another
                        index = 0;
                    }                
                }else{
                    if(checkKeyword(index) == CheckingResult::StatNotFound){  // Other stat has not been found, thats why I return LastStat as StatFound
                        return CheckingResult::StatFound;
                    }else{
                        currentChar++, index++;
                    }
                }
            }else{
                if(index == lastKeywordLength){

                }else{
                    savedFirstKeywordCharPos = currentChar;
                    lastKeyword = &endOfStringKeyword;
                }
                return CheckingResult::StatFound;
            }
        }
        // Find New Stat
        switch (checkKeyword(index)) {
        case CheckingResult::StatFound:
        {
            savedFirstKeywordCharPos = currentChar;
            index++;
        }
            break;
        default:
            break;
        }
        currentChar++;

    }
    if(lastKeywordLength > 0 and index == lastKeywordLength){

    }else{
        savedFirstKeywordCharPos = currentChar;
        lastKeyword = &endOfStringKeyword;
    }
    return CheckingResult::StatFound;;
}

CheckingResult KeywordsController::checkKeyword(int& index){
    int i = 0;
    for(KeywordsMap::ConstIterator keywords = keywordsMap.constBegin() + index;
        keywords < keywordsMap.constEnd(); keywords++)
    {
        for(Keywords::ConstIterator keyword = keywords->constBegin();
            keyword < keywords->constEnd(); keyword++)
        {
            for(i = 0; i <= index; i++){
                if((currentChar - i)->toLower() != keyword->keyword.at(index - i))
                    break;
            }
            if(i > index){
                lastKeyword = keyword;
                lastKeywordLength = keyword->keyword.size();
                return CheckingResult::StatFound;
            }
        }
    }
    return CheckingResult::StatNotFound;
}

Result KeywordsController::interpret(){

    savedChar = savedFirstKeywordCharPos = currentChar;
    lastKeywordLength = 0;

    if(checkingPriv() == CheckingResult::StatFound ){
        return Result::StatFound;
    }

    return Result::EndOfString;
}


void TclProcedureInterpreter::tryToActivateWriteOnlyProcedure(Call::Name& name){
    if(writeOnlyProcedureActiveIndex == -1 and userConfig.proceduresSettings().isWriteOnlyProcedure(name)){
        writeOnlyProcedureActiveIndex = procedureCalls.size() + 1;
        activateWriteOnlyProcedureMode();
    }
}


Error TCLInterpreter::toCAPL(TclCommand &tclCommand){
    if(isError())
        return Error::Error;
    textInterpreter().initialize(tclCommand);
    setProcessingStat(Stat::None);
//
    while(processingStat() != Stat::EndOfString)
        if(callCurrentInterpreterProcedure() == Error::Error)
            return Error::Error;

    return Error::NoError;
}

Error TCLInterpreter::interpreterProcedure_standard(){
    while(textInterpreter().runSearchingMode() == Result::StatFound){

        if(processUnknownString() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }

        setProcessingStat(textInterpreter().lastKeywordStat());

        if(callInterpretFunction() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }
        if(commandsController.performDynamicRulesCheck() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }
        if(processingStat() == Stat::EndOfString){
            textInterpreter().deinitialize();
            return Error::NoError;
        }
    }

    return isError()? Error::Error : Error::NoError;
}
Error TCLInterpreter::interpreterProcedure_backslashSubbingSpecial(){
    if(callInterpretFunction() == Error::Error){
        if(processError() == Error::Error)
            return Error::Error;
    }else{
        _processingStat = Stat::Whitespace;
        savedKeyword = " ";
        setSavedKeywordReadMode();
        if(callInterpretFunction() == Error::Error){
            setStandardReadKeywordMode();
            if(processError() == Error::Error)
                return Error::Error;
        }
        setStandardReadKeywordMode();
        setStandardInterpreterMode();
        commandsController.performDynamicRulesCheck();
        if(isError()){
            if(processError() == Error::Error)
                return Error::Error;
        }
    }

    return Error::NoError;
}
Error TCLInterpreter::interpreterProcedure_commentSpecial(){
    QString tempStr = textInterpreter().restOfString();

    if(tempStr.endsWith("\\")){
        tempStr.chop(1);
    }else{
        setStandardInterpreterMode();
    }
    if(commandsController.isMainScript())
        addExpressionToMainCodeBlock({"//" + tempStr + "\n"});
    else
        addExpressionToCodeBlock({"//" + tempStr + "\n"});

    setProcessingStat(Stat::EndOfString);
    return Error::NoError;
}


void TCLInterpreter::printErrorReport(QFile& reportFile, QString inputFileName){
    if(!ignoreMessages.isEmpty()) {
        if(!reportFile.isOpen()){
            if(!reportFile.open(QIODevice::Text | QIODevice::Append))
                return;
            reportFile.write(TCL_TO_CAPL_ERRORS_LOG_PREFIX_TEXT.toUtf8());
        }

        for(decltype (ignoreMessages)::Iterator message = ignoreMessages.begin();
            message < ignoreMessages.end();
            message++)
        {
            reportFile.write(QString(TCL_TO_CAPL_ERRORS_LOG_NEW_ROW_POSTFIX_TEXT + TCL_TO_CAPL_ERRORS_LOG_ERROR_CELL_TEXT + TCL_TO_CAPL_ERRORS_LOG_DETAILS_CELL_TEXT).toUtf8());
        }
    }
}

void TCLInterpreter::printErrorReport(QString& errorReport){
    if( not ignoreMessages.isEmpty()) {
        errorReport.append(TCL_TO_CAPL_ERRORS_LOG_PREFIX_TEXT.toUtf8());

        for(decltype (ignoreMessages)::Iterator message = ignoreMessages.begin();
            message < ignoreMessages.end();
            message++)
        {
            errorReport.append(QString(TCL_TO_CAPL_ERRORS_LOG_NEW_ROW_POSTFIX_TEXT + TCL_TO_CAPL_ERRORS_LOG_ERROR_CELL_TEXT_FOR_INSTRUCTION_PANEL + TCL_TO_CAPL_ERRORS_LOG_DETAILS_CELL_TEXT).toUtf8());
        }



    }
}


// PROCEDURE DEFINITION FUNCTIONS ==================================================================================
/*QString TclProcedureInterpreter::toString(SavedStat &stat, ProcedureDefinition::Format::Target target){
    using Target = ProcedureDefinition::Format::Target;
    QString str;

    /*switch(target){
    case Target::ProcedureParameters:
    {
        switch(stat.stat()){
        case Stat::String:
            str = QString("\"") + stat.command() + QString("\"");
            break;
        case Stat::Const:
            str = stat.command();
        default:
            break;
        }
    }
        break;
    case Target::OriginalProcedureParameters:
    {
        switch(stat.stat()){
        case Stat::String:
            str = QString("\"") + stat.command() + QString("\"");
            break;
        default:
            break;
        }
    }
        break;
    default:
        break;
    }
    return QString();
}*/


QStringList::size_type TclProcedureInterpreter::createAndAssignString(QString& dest, QStringList args){
    using Format = ProcedureDefinition::Format;
    using Target = Format::Target;
    Target target = Target::CaplFormat;
    //QString* targetStr = nullptr;
    QString seperator;
    bool separatorUsed = false;
    for(QStringList::Iterator arg = args.begin(); arg < args.end(); arg++){
        // For Empty String next arg is "special", "control string":
        // - "=<number>", ex. "=2" - take string from lastActionResponse at index 2
        // - "><number>", ex. ">2" - take all string from lastActionResponse in index range: 2 ...
        // - "<sign><number>@", ex. ">2@ " - after @ place separator between arguments
        int index = INT_MAX;
        bool ok = false;
        if(arg->isEmpty()){ // "Special Argument"
            arg++;
            if(arg < args.end()){
                // Format Rules
                    using Rule = Format::Rule;
                    Rule formatRule;
                    switch((formatRule = static_cast<Rule>(arg->at(0).toLatin1()), *arg = arg->mid(1), formatRule)){
                    case Rule::SEPARATOR:
                    {
                        seperator = *arg;
                        separatorUsed = false;
                    }
                        break;
                    case Rule::INDEX_OR_FULL_LINE:
                    {
                        if(arg->isEmpty()){
                            // FULL LINE ----------------
                            switch(target){
                            case Target::TclFormat:
                            case Target::CaplFormat:
                                dest += lastProcedureCall()._name();
                                break;
                            case Target::Raw:
                                dest += command;
                                break;
                            default:
                                return (arg - args.begin());
                            }
                            // FULL LINE ----------------
                        }else{
                            // ARGUMENT AT INDEX -------------------------------
                            index = arg->toInt(&ok);
                            if(not ok)
                                return (arg - args.begin());
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = lastProcedureCall().parametersLength() + index; // Index is negative
                            }
                            if(index < 0 or index >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            index++; // Ignore procedure name
                            switch(target){                            
                            case Target::Raw:
                            case Target::ProcedureParametersStat:
                            case Target::CaplFormat:
                            case Target::TclFormat:
                                dest += lastProcedureCall().parameters()[index].toString(target);
                                break;
                            default:
                                return (arg - args.begin());
                            }
                            // ARGUMENT AT INDEX -------------------------------
                        }
                    }
                        break;
                    case Rule::ARGS_AFTER_INDEX:
                    {
                        // ALL ARGUMENTS AFTER INDEX -------------------------------
                        if((static_cast<void>(index = arg->toInt(&ok)), !ok))
                            return (arg - args.begin());
                        if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                            index = lastProcedureCall().parametersLength() + index; // Index is negative
                        }
                        if(index < 0)
                            return (arg - args.begin());
                        index++; // Ignore procedure name
                        switch(target){
                        case Target::SnprintfFormat:
                        case Target::CaplFormat:
                        case Target::TclFormat:
                        case Target::Raw:
                        {
                            for(Call::Parameters::Iterator responseArg = lastProcedureCall().parameters().begin() + index; responseArg < lastProcedureCall().parameters().end(); responseArg++){
                                dest += responseArg->toString(target) + seperator;
                                separatorUsed = true;
                            }
                        }
                            break;
                        default:
                            return (arg - args.begin());
                        }
                        // ALL ARGUMENTS AFTER INDEX -------------------------------
                    }
                        break;
                    case Rule::TARGET:
                    {
                        if((static_cast<void>(target = static_cast<Target>(arg->toUInt(&ok))), !ok))
                           return (arg - args.begin());
                    }
                        break;                    
                    default:
                        return (arg - args.begin());
                    }
                if(separatorUsed){
                    dest.chop(seperator.size());
                    seperator = QString();
                    separatorUsed = false;
                }
            }else{
                arg--;
            }

        }else{
            dest += *arg;
        }
    }

    return args.size();
}


const TclCommand_NS::Definition::Rule::ConditionalActions::value_type TclProcedureInterpreter::newCompareRule(const QStringList stringsToCompare, const QStringList format){
    Q_ASSERT_X(stringsToCompare.size() > 0 and format.size() > 0, "newCompareRule", "Creating Compare Role");

    return ProcedureDefinition::Rule::ConditionalActions::value_type
    {
     ProcedureDefinition::Action::Conditional::Compare,
     QStringList(QString::number(stringsToCompare.size())) +
        stringsToCompare +
        QStringList(QString::number(format.size())) +
        format
    };
}


