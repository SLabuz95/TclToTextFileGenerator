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
using TCLInterpreterFunctions = TCLInterpreter::InterpretFunctions;
using UserInteractionStatus = Tcl::UserInteractionStatus;
using Rule = TclCommand_NS::Definition::Rule;
using Action = TclCommand_NS::Definition::Action;
using RuleControlFlag = Rule::Control;
using Format = TclCommand_NS::Definition::Format;
using FormatRule = TclCommand_NS::Definition::Format::Rule;
using FormatTarget = TclCommand_NS::Definition::Format::Target;

using namespace Tcl::Interpreter;

TCLInterpreter::TCLInterpreter(UserInputConfig& userConfig, FunctionDefinitionsRef functionDefinitionsRef)
    : commandsController(*this, userConfig), functionDefinitions(functionDefinitionsRef), userConfig(userConfig)
{clearError();}



KeywordsMap KeywordsController::keywordsMap ={
    {
        {"#", {Stat::Comment}},
        {"{", {Stat::BracesStart}, Keyword::UnknownStringRule::Forbidden},
        {"}", {Stat::Braces}},
        {"[", {Stat::CommandSubbingStart}},
        {"]", {Stat::CommandSubbingEnd}},
        {"(", {Stat::Word}},
        {")", {Stat::Word}},
        {"+", {Stat::Word}},
        {"-", {Stat::Word}},
        {"*", {Stat::Word}},
        {"/", {Stat::Word}},
        {"%", {Stat::Word}},
        {">", {Stat::Word}},
        {"<", {Stat::Word}},
        {"!", {Stat::Word}},
        {"&", {Stat::Word}},
        {"|", {Stat::Word}},
        {"^", {Stat::Word}},
        {"?", {Stat::Word}},
        {":", {Stat::Word}},
        {" ", {Stat::Whitespace}},
        {"\t", {Stat::Whitespace}},
        {"$", {Stat::VariableSubbingStart}},
        {"\"", {Stat::DoubleQuotes}},
        {";", {Stat::Semicolon}},
        {"\\", {Stat::BackslashSubbing}}

    },
    {
        {"::", {Stat::Namespace}},
        {"==", {Stat::Word}},
        {"!=", {Stat::Word}},
        {">=", {Stat::Word}},
        {"<=", {Stat::Word}},
        {"&&", {Stat::Word}},
        {"||", {Stat::Word}},
        {"<<", {Stat::Word}},
        {">>", {Stat::Word}},
    },
};

//TODO: Dodaj definiowanie typów zmiennych
//TODO: DONE Zmodyfikuj taby w pannelach (Ustawiono w konstruktorze)
//TODO: Może dodaj dopisywanie znacznikow i ich podpowiadanie
//TODO: DONE Lepsza kontrola kopiowanego tekstu: ten sam font, dodatkowe znaki (Wyłaczenie RichTextow)
//TODO: DONE Zrobić domyślne przekierowania do domyślnych reguł nieokreślonych dla pustych reguł DONE Jesli procedura nie ma regul onEndOfCall, to pobieram reguly nieznanych procedur

//WARNING: Zawsze dodawaj String Literal'y z toLower
//TODO: Dodać ostrzeżenia o braku EndOfCall z RulesForArgument i rulesForUnspeicifiedArgument
 QMap<const QString,  UserInteractionStatus> TclProcedureInterpreter::userInteractionMap
{
    {QStringLiteral("NotRequired").toLower(), UserInteractionStatus::NotRequired},
    {QStringLiteral("Required").toLower(), UserInteractionStatus::Required}
};

 QMap<QString, RuleControlFlag> Rule::controlMap
{
    {QStringLiteral("BreakRuleCheck").toLower(), RuleControlFlag::BreakRuleCheck},
    {QStringLiteral("NoBreakRuleCheck").toLower(), RuleControlFlag::NoBreakRuleCheck},
    //{QStringLiteral("BreakRuleCheckDontExecOnEndActions").toLower(), RuleControlFlag::BreakRuleCheckDontExecOnEndActions}
};

 QMap<const QString, Action::Conditional> Action::conditionalMap
{
    {QStringLiteral("CompareNumbOfArguments").toLower(), Action::Conditional::CompareNumbOfArguments},
    {QStringLiteral("IsLastSavedStat").toLower(), Action::Conditional::IsLastSavedStat},
    {QStringLiteral("Compare").toLower(), Action::Conditional::Compare}
};

 QMap<const QString,  Action::Executable> Action::executableMap
{
    {QStringLiteral("Write").toLower(), Action::Executable::Write},
    {QStringLiteral("TclParse").toLower(), Action::Executable::TclParse},
    {QStringLiteral("Error").toLower(), Action::Executable::Error},
    {QStringLiteral("ChangeLastSavedStat").toLower(), Action::Executable::ChangeLastSavedStat},
    {QStringLiteral("AddFunctionDefinition").toLower(), Action::Executable::AddFunctionDefinition},
    {QStringLiteral("AddPreExpression").toLower(), Action::Executable::AddPreExpression},
    {QStringLiteral("AddUserInteraction").toLower(), Action::Executable::AddUserInteraction},
    //{QStringLiteral("FinalizeForEach"), Action::Executable::FinalizeForEach},  // private
    //{QStringLiteral("AddSnprintf"), Action::Executable::AddSnprintf},// private
};

 QMap<const QString,  FormatRule> Format::ruleMap
{
    {QStringLiteral("NameOrIndex").toLower(), FormatRule::INDEX_OR_FULL_LINE},
    {QStringLiteral("ArgumentsFrom").toLower(), FormatRule::ARGS_AFTER_INDEX},
    {QStringLiteral("Separator").toLower(), FormatRule::SEPARATOR},
    {QStringLiteral("Target").toLower(), FormatRule::TARGET},
};

 QMap<const QString,  FormatTarget> Format::targetMap
{
    {QStringLiteral("Raw").toLower(), FormatTarget::Raw},
    {QStringLiteral("TclFormat").toLower(), FormatTarget::TclFormat},
    {QStringLiteral("CaplFormat").toLower(), FormatTarget::CaplFormat},
    {QStringLiteral("ParametersStat").toLower(), FormatTarget::ProcedureParametersStat},
    {QStringLiteral("Command").toLower(), FormatTarget::Command},
    {QStringLiteral("SnprintfFormat").toLower(), FormatTarget::SnprintfFormat},
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
bool TCLInterpreter::checkInterpretFunctions(){
    constexpr Stat implementedStats[] = {
        Stat::Word,
        Stat::CommandSubbingStart,
        Stat::CommandSubbingEnd,
        Stat::BracesStart,
        Stat::Braces,
        Stat::DoubleQuotes,
        Stat::Whitespace,
        Stat::VariableSubbingStart,
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
}
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
                            "T4",
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
    {
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
            {   // Rule 1: if(=0 == concat) -> write Result
                {
                    {
                        TclProcedureInterpreter::newCompareRule(
                        {"concat"},
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0"
                        }
                        ),
                    },
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::AddSnprintf,
                        {}
                    }
                }
            },
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
    },
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
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Braces)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BackslashSubbing)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Word)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing)),
                                //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingSnprintf)),
                                //QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingString))
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
                                       ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
                           {    // Action 1: Change stat to Expression
                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Expression))}
                           }
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
                           {    // Action 1: Change stat to Exxpression
                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Expression))}
                           }
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
                            + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Command),
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
    using InterpreterMode = TclProcedureInterpreter::ProdecuresSettings::InterpreterMode;
    const QString ERROR_DESCRIPTION = QString("addEmptyLine: ");
    if(userConfig.proceduresSettings().mode() != InterpreterMode::TestCaseReport){
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
    }
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
    case Target::Command:
        return command();

    case Target::SnprintfFormat:
    {
        // Temporary
        return QString("%s");
        /*
        switch(stat()){
        case Stat::String:
        case Stat::DoubleQuotes:
        case Stat::PendingString:
        case Stat::StringInQuotes:
            return QString("\"") + command() + "\"";
        case Stat::EndOfCodeBlock:
            return QString("{\n") + command() + "\n}\n";
        case Stat::Braces:
            return savedStat().listToCaplString();
        default:
            return command();
        }*/
    }
    case Target::ProcedureParametersStat:
    {
        return QString::number(std::underlying_type_t<Stat>(stat()));
    }
        break;
    case Target::CaplFormat:
    {
        switch(stat()){
//        case Stat::String:
        case Stat::DoubleQuotes:
//        case Stat::PendingString:
//        case Stat::StringInQuotes:
            return QString("\"") + command() + "\"";
        case Stat::Script:
            return QString("{") + command() + "}";
        case Stat::Braces:
            return savedStat().listToCaplString();
        default:
            return command();
        }
    }
        break;
    case Target::TclFormat:
    {
        using Parameter = Parameters::Iterator;
        QString str;
        switch (stat()) {
        case Stat::VariableSubbing:
            return QString("$") + command();
        case Stat::Braces:
            return QString("{") + savedStat().listToTclListString() + "}";
        case Stat::DoubleQuotes:
            return command();
//        case Stat::StringInQuotes:
//            return QString("\"") + command() + "\"";
        case Stat::CommandSubbing:
        {
            str = QString("[");
            for(Parameter parameter = rawParameterStats.begin(); parameter < rawParameterStats.end();  parameter++)
                str += parameter->toString(target) + " ";
            str += QString("]");
            return str;
        }
            break;
//        case Stat::Snprintf:
//        {
//            str = QString("\"");
//            for(Parameter parameter = rawParameterStats.begin() + 2; parameter < rawParameterStats.end();  parameter++)
//                str += parameter->toString(target);
//            str += QString("\"");
//            return str;
//        }
//            break;
//        case Stat::PendingSnprintf:
//        {
//            for(Parameter parameter = rawParameterStats.begin() + 2; parameter < rawParameterStats.end();  parameter++)
//                str += parameter->toString(target);
//            return str;
//        }
//            break;
        case Stat::Expression:
        {
            TCLInterpreter::throwError("Expression to string for TclFormat not implemented");
        }
            break;
        case Stat::Script:
        {
            TCLInterpreter::throwError("CodeBlock to string for TclFormat not implemented");
        }
            break;
        default:
            return command();
        }
    }
        break;
    default:
        break;
    }
    return QString();
}


Error TCLInterpreter::newProcedureCall(TclCommand_NS::Call::Name name){
    const QString ERROR_PREFIX = "TCLInterpreter::newProcedureCall: ";


    switch(lastSavedStat().stat()){
    case Stat::CommandSubbingStart:
        // Do not add predefinitions group
        break;
    case Stat::Script:
        predefinitionsController.newGroup(savedStatsSize());
        break;
    default:
        break;
    }

    if(commandsController.newProcedureCall(name) == Error::Error)
        return throwError(ERROR_PREFIX + error());


    return Error::NoError;
}

Error TCLInterpreter::removeProcedureCall_writeOnlyProcedure(){
    commandsController.tryToDeactivateWriteOnlyProcedure();
    return removeProcedureCall_standard();
}

Error TCLInterpreter::saveStatWithParsingControl(SavedStat newSavedStat){
    // No Control for now
    saveStat(newSavedStat);
    return Error::NoError;
}

Error TCLInterpreter::removeLastSavedStatWithParsingControl(){
    // No Control for now
    return removeLastStat();
}

Error TCLInterpreter::takeLastSavedStatWithParsingControl(SavedStat& savedStat){
    // No Control for now
    lastSavedStat().hideInfoBeforeCopyConstructor();
    return (savedStat = takeLastSavedStat(), Error::NoError);
}

Error TCLInterpreter::moveArgumentToFunctionCall(){
    const QString ERROR_DESCRIPTION = QString("InternalCall: moveArgumentToFunctionCall() : ");
     if(!isPrelastSavedStat())
        return throwError(ERROR_DESCRIPTION + "Empty Stats");
    switch (prelastSavedStat().stat()) {
//    case Stat::Snprintf:
//    case Stat::PendingSnprintf:
//        return throwError(ERROR_DESCRIPTION + "No Function Call (Snprintf or PendingSnprintf Found) " );
//        break;
    case Stat::CommandSubbing:
        break;
    default:
        return throwError(ERROR_DESCRIPTION + "No Function Call" );
    }

    return (commandsController.nextArgument() == Error::Error or
                commandsController.onArgumentProcedureCheck() == Error::Error)
            ?
                Error::Error :
                Error::NoError;
}

Error TCLInterpreter::moveArgumentToSnprintf_priv(Stat stat){
    using Parameter = TclCommand_NS::Call::Parameter;
    const QString ERROR_DESCRIPTION = QString("InternalCall: TCLInterpreter::moveArgumentToSnprintf_priv(): ");
    if(!isPrelastSavedStat())
        return throwError("No Stats for snprintf processing");
    switch(prelastSavedStat().stat()){
//    case Stat::Snprintf:
//    case Stat::PendingSnprintf:
//    {
//        if(commandsController.nextArgument() == Error::Error or
//                commandsController.onArgumentProcedureCheck() == Error::Error)
//            return throwError(ERROR_DESCRIPTION + error());
//    }
//        break;
    default:
    {
        if(commandsController.nextArgumentForSnprintf_priv(stat) == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
    }
    return Error::NoError;
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
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret(unknownString) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::MainScript:
    {
        if(commandsController.newProcedureCall(unknownString) == Error::Error)
            return throwError(ERROR_PREFIX + error());
        // Stat shall be replace by CommandSubbing in the CommandsController
    #ifdef QT_DEBUG
        if(lastSavedStat().filteredStat() != Stat::CommandSubbing)
            qFatal("New Procedure Call did not remove CommandSubbingStart stat");
    #endif
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
        if(commandsController.interpret("[") == Error::Error)
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
        if(commandsController.interpret("[") == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::MainScript:
    {
       return throwError(ERROR_PREFIX + "CommandSubbingEnd in MainScript or Script");
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
Error TCLInterpreter::interpret<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::BracesStart>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().stat()){
    case Stat::MainScript:
    {
        // New Procedure call  + Braces
        // Maybe create new Procedure first, then interpret as BracesStart
        //if(commandsController.newProcedureCall() == Error::Error)
        //    return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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

    switch(lastSavedStat().stat()){
    case Stat::MainScript:
    {
        // New Procedure call  + Braces
        // Maybe create new Procedure first, then interpret as Braces
        //if(commandsController.newProcedureCall() == Error::Error)
        //    return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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

    switch(lastSavedStat().stat()){
    case Stat::MainScript:
    {
        // New Procedure call  + DoubleQuotes
        // Maybe create new Procedure first, then interpret as DoubleQuotes
        //if(commandsController.newProcedureCall() == Error::Error)
        //    return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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

    switch(lastSavedStat().stat()){    
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;
    default:
        // Set Whitespace occurance flag -- Modify Whitespace checking
        break;
    }
    return Error::NoError;
}

template<>
Error TCLInterpreter::interpret<Stat::VariableSubbingStart>(){

    const QString ERROR_PREFIX = "TCL Interpreter <Stat::VariableSubbingStart>: ";
     if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().stat()){
    case Stat::MainScript:
    {
        return throwError(ERROR_PREFIX + "VariableSubbingStart in MainScript ");
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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

    switch(lastSavedStat().stat()){
    case Stat::MainScript:
    {
        // New Procedure call  + DoubleQuotes
        // Maybe create new Procedure first, then interpret as DoubleQuotes
        //if(commandsController.newProcedureCall() == Error::Error)
        //    return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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

    switch(lastSavedStat().stat()){    
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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

    switch(lastSavedStat().stat()){
    case Stat::MainScript:
    {
        // Copy from dev branch + think about BackslashSubbing with newline
    }
        break;
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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

    switch(lastSavedStat().stat()){
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
    if(textInterpreter.isCurrentChar()) // Accesible -> Analysis
    {
        bool whitespaceAfterSpecialSign = false;
        switch (textInterpreter.currentCharForSpecialSign()->toLatin1()) {
        case ' ':
        case '\t':
            specialSignStr = *textInterpreter.currentCharForSpecialSign();
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
            specialSignStr = QStringLiteral("\\") + *textInterpreter.currentCharForSpecialSign();
            break;
        default:
            specialSignStr = *textInterpreter.currentCharForSpecialSign();
        }

        // Finally _ Increment CurrentChar (Less Readable and safe, but more efficient and less work around)
        textInterpreter.incrementCurrentCharDueToSpecialSign();

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

    switch(lastSavedStat().stat()){
    case Stat::MainScript:
    {
        // New Procedure call  + DoubleQuotes
        // Maybe create new Procedure first, then interpret as DoubleQuotes
        //if(commandsController.newProcedureCall() == Error::Error)
        //    return throwError(ERROR_PREFIX + error());
    }
    case Stat::CommandSubbing:
    {
        if(commandsController.interpret("[") == Error::Error)
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
    nullptr, // Variable Subbing
    &TCLInterpreter::interpret<Stat::CommandSubbingStart>,
    &TCLInterpreter::interpret<Stat::CommandSubbingEnd>,
    &TCLInterpreter::interpret<Stat::Braces>,
    &TCLInterpreter::interpret<Stat::Whitespace>,
    &TCLInterpreter::interpret<Stat::VariableSubbingStart>,
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

template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(Call::Name name){
    using Definition = CommandDefinitions::Iterator;
    Definition definition;
    for(definition = procedureDefinitions.begin(); definition < procedureDefinitions.end(); definition++){
        if(definition->name == name)
            break;
    }
    try {
        if(definition == procedureDefinitions.end()){
            procedureCalls.append(Call(name));
        }else{
            procedureCalls.append(Call(definition));
        }
    }  catch (std::exception& e) {
        return throwError(e.what());
    }

    return Error::NoError;
}

inline void TclProcedureInterpreter::tryToActivateWriteOnlyProcedure(Call::Name& name){
    if(writeOnlyProcedureActiveIndex == -1 and tclInterpreter.userConfig.proceduresSettings().isWriteOnlyProcedure(name)){
        writeOnlyProcedureActiveIndex = procedureCalls.size() + 1;
        activateWriteOnlyProcedureMode();
    }
}

template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCaseReport>(Call::Name name){
    tryToActivateWriteOnlyProcedure(name);
    return newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::PredefinitionsOnly>(Call::Name name){
    return newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

Error TclProcedureInterpreter::interpret(QString newArgument)
{
    using Parameter = Call::Parameter;
    switch(tclInterpreter.lastSavedStat().stat()){
    case Stat::CommandSubbing:
//    case Stat::PendingSnprintf:
//    case Stat::Snprintf:
//    {
//        if(procedureCalls.size() < 2){
//            return throwError("No Procedure Calls for Function-like Argument passing");
//        }
//        Call procedureCall = procedureCalls.takeLast();
//        Parameter parameter(tclInterpreter.takeLastSavedStat(), procedureCall);
//        procedureCalls.last().nextArgument(parameter);
//    }
//        break;
    case Stat::Script:
    {
        tclInterpreter.prepareCodeBlockContent();
        Parameter parameter(tclInterpreter.takeLastSavedStat());
        procedureCalls.last().nextArgument(parameter);
    }
        break;
    default:
    {
        Parameter parameter(tclInterpreter.takeLastSavedStat());
        procedureCalls.last().nextArgument(parameter);
    }
    }
    return Error::NoError;
}

Error TclProcedureInterpreter::nextArgumentForSnprintf_priv(Stat stat)
{
    const QString ERROR_DESCRIPTION = QString("InternalCall: TclProcedureInterpreter::nextArgumentForSnprintf_priv(): ");
    using Parameter = Call::Parameter;
    switch(tclInterpreter.lastSavedStat().stat()){
//    case Stat::PendingSnprintf:
//    case Stat::Snprintf:
//    {
//        return throwError(ERROR_DESCRIPTION + "Snprintf as argument for Snprintf");
//    }
    case Stat::CommandSubbing:
    {
        Call procedureCall = procedureCalls.takeLast();
        Parameter parameter(tclInterpreter.takeLastSavedStat(), procedureCall);
        tclInterpreter.saveStat({stat, QString()});
//        tclInterpreter.saveStat({Stat::String, QString("concat")});
        if(tclInterpreter.newProcedureCall("string") == Error::Error or
                nextArgument() == Error::Error or
                onArgumentProcedureCheck() == Error::Error or
             (procedureCalls.last().nextArgument(parameter),
                onArgumentProcedureCheck() == Error::Error))
            return throwError(ERROR_DESCRIPTION + tclInterpreter.error());
    }
        break;
    default:
    {
        Parameter parameter(tclInterpreter.takeLastSavedStat());
        tclInterpreter.saveStat({stat, QString()});
//        tclInterpreter.saveStat({Stat::String, QString("concat")});
        if(tclInterpreter.newProcedureCall("string") == Error::Error or
                nextArgument() == Error::Error or
                onArgumentProcedureCheck() == Error::Error)
            return throwError(ERROR_DESCRIPTION + tclInterpreter.error());
        if(!parameter.command().isEmpty() and
                (procedureCalls.last().nextArgument(parameter),
                onArgumentProcedureCheck() == Error::Error))
            return throwError(ERROR_DESCRIPTION + tclInterpreter.error());
    }
    }
    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(SavedStat &statCommand){
    using RulesForArguments = ProcedureDefinition::RulesForArguments;
    using RulesForArgument = ProcedureDefinition::RulesForArguments::Iterator;
    using Rules = ProcedureDefinition::Rules;
    using Rule = ProcedureDefinition::Rules::Iterator;

    const QString ERROR_PREFIX = "TclProcedureInterpreter::finalizeProcedureCall: ";
    command.clear();
    if(procedureCalls.isEmpty()){
        return throwError("No Procedure Calls for Finalize");
    }
    // User Interaction Section ---------------------------
    if(procedureCalls.last().isUserInteractionRequired()){
        if(addPreExpressionForUserInteraction() == Error::Error){
            return throwError(ERROR_PREFIX + tclInterpreter.error());
        }
    } // -----------------------------------------------------
    else{
        finalizeOn = true;
        Call& procedureCall = procedureCalls.last();

        Definition::RulesOnEndOfCall& rulesOnEndOfCall = (procedureCall.rulesOnEndOfCall().isEmpty())? Definition::defaultUnknownProcedureDefinition.rulesOnEndOfProcedureCall : procedureCall.rulesOnEndOfCall();
        bool ruleCondtionsPassed = false;

        using Actions = Rules::value_type::ExecutableActions;
        using Action = Actions::Iterator;
        for(Rule rule = rulesOnEndOfCall.begin(); rule < rulesOnEndOfCall.end(); rule++){
            using Conditions = Rules::value_type::ConditionalActions;
            using Condition = Conditions::Iterator;
            Condition condition;
            ConditionResult conditionResult = ConditionResult::Satisfied;
            for(condition = rule->conditions.begin(); condition < rule->conditions.end(); condition++){
                conditionResult = (this->*(conditionalInterpreterFunctions[static_cast<std::underlying_type_t<Conditions::value_type::ActionType>>(condition->type())]))(condition->parameters());
                if(tclInterpreter.isError()){
                    finalizeOn = false;
                    return throwError(ERROR_PREFIX + tclInterpreter.error());
                }
                if(conditionResult == ConditionResult::Unsatisfied){
                    break;
                }
            }
            if(condition == rule->conditions.end()){
                ruleCondtionsPassed = true;
                for(Action action = rule->actions.begin(); action < rule->actions.constEnd(); action++){
                    (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Actions::value_type::ActionType>>(action->type())]))(action->parameters());
                    if(tclInterpreter.isError()){
                        finalizeOn = false;
                        return throwError(ERROR_PREFIX + tclInterpreter.error());
                    }
                }
                switch(rule->controlFlag){
                //case Rules::value_type::Control::BreakRuleCheckDontExecOnEndActions:
                case Rules::value_type::Control::BreakRuleCheck:
                    rule = rulesOnEndOfCall.end();
                    break;
                case Rules::value_type::Control::NoBreakRuleCheck:
                    ruleCondtionsPassed = false;
                    break;
                default:
                    break;
                }
            }
        }
        if(not ruleCondtionsPassed){
            tclInterpreter.functionDefinitions.addDefinitionNotSatisfiedRules(procedureCall);
        }
    }
    statCommand.setCommand(command);
    finalizeOn = false;

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCaseReport>(SavedStat &){
    finalizeOn = false;

    return Error::NoError;

}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::PredefinitionsOnly>(SavedStat &statCommand){

    return finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(statCommand);

}

Error TCLInterpreter::finalizeProcedureCall(){
    const QString ERROR_DESCRIPTION = "TCL_Internal::finalizeProcedureCall";
    const QString END_OF_EXPRESSION_SIGN = ";\n";
    if(!isPrelastSavedStat())
        return throwError(ERROR_DESCRIPTION + " No stats");
    if(lastSavedStat().stat() != Stat::CommandSubbing)
        return throwError(ERROR_DESCRIPTION + "Wrong stat to finalize procedure.");
    switch(prelastSavedStat().stat()){
//    case Stat::Snprintf:
//    case Stat::PendingSnprintf:
    case Stat::CommandSubbing:
    {
        if(commandsController.finalizeProcedureCall(lastSavedStat()) == Error::Error/* or
                removeProcedureCall() == Error::Error*/)
            return throwError(ERROR_DESCRIPTION + error());
//        if(prelastSavedStat().stat() == Stat::Snprintf){
//            if(moveArgumentToSnprintf() == Error::Error)
//                return throwError(ERROR_DESCRIPTION + error());
//        }
    }
        break;
    case Stat::Script:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                commandsController.finalizeProcedureCall(tempStat) == Error::Error or
                /*Preexpressions to Command */
                ( addPreexpressionsToCodeBlock(),
                  snprintfController.clear(),
                  addExpressionToCodeBlock({tempStat.command() + commandsController.lastProcedureCall().tryToAddEndOfExpressionSign()}),
                 false) or
                // Continue conditional expression
                removeProcedureCall() == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    case Stat::Expression:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                commandsController.finalizeProcedureCall(tempStat) == Error::Error or
                /*Preexpressions to Command */
                ( /*addPreexpressionsToCodeBlock(),
                  snprintfController().clear(),*/
                  addExpressionToCodeBlock({tempStat.command()}) ,
                 false) or
                // Continue conditional expression
                removeProcedureCall() == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    case Stat::MainScript:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                commandsController.finalizeProcedureCall(tempStat) == Error::Error or
                /*Preexpressions to Command */
                (   addPreexpressionsToMainCodeBlock(),
                    snprintfController.clear(),
                    addExpressionToMainCodeBlock({tempStat.command() + commandsController.lastProcedureCall().tryToAddEndOfExpressionSign()}) ,
                 false) or
                // Continue conditional expression
                ( commandsController.lastProcedureCall().clearMemory(),
                removeProcedureCall() == Error::Error))
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    default:
        return throwError(ERROR_DESCRIPTION + "Wrong prestat to finalize procedure.");
    }
    return Error::NoError;
}

Error TCLInterpreter::finalizeSnprintfCall(){
    const QString ERROR_DESCRIPTION = "TCL_Internal::finalizeProcedureCall";
    if(!isPrelastSavedStat())
        return throwError(ERROR_DESCRIPTION + " No stats");
//    if(not( lastSavedStat().stat() == Stat::Snprintf or lastSavedStat().stat() == Stat::PendingSnprintf))
//        return throwError(ERROR_DESCRIPTION + "Wrong stat to finalize procedure.");
    switch(prelastSavedStat().stat()){
//    case Stat::Snprintf:
//    case Stat::PendingSnprintf:
//    {
//        return throwError(ERROR_DESCRIPTION + "Snprintf should not call other Snprintf");
//    }
//        break;
    case Stat::CommandSubbing:
    {
        if(commandsController.finalizeProcedureCall(lastSavedStat()) == Error::Error/* or
                removeProcedureCall() == Error::Error*/)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    /*case Stat::MainScript:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                commandsController.finalizeProcedureCall(tempStat) == Error::Error or
                // Preexpressions to Command
                (   addPreexpressionsToMainCodeBlock(),
                    addExpressionToMainCodeBlock( tempStat.command()) ,
                 false) or
                // Continue conditional expression
                removeProcedureCall() == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    case Stat::Script:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                commandsController.finalizeProcedureCall(tempStat) == Error::Error or
                //Preexpressions to Command
                (   addPreexpressionsToCodeBlock(),
                    addExpressionToCodeBlock(tempStat.command()) ,
                 false) or
                // Continue conditional expression
                removeProcedureCall() == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;*/
    default:
        return throwError(ERROR_DESCRIPTION + "Wrong prestat to finalize procedure.");
    }
    return Error::NoError;
}


Error TclProcedureInterpreter::dynamicProcedureArgumentCheck_priv(){
    using RulesForArguments = ProcedureDefinition::RulesForArguments;
    using RulesForArgument = ProcedureDefinition::RulesForArguments::Iterator;
    using Rules = ProcedureDefinition::Rules;
    using Rule = ProcedureDefinition::Rules::Iterator;
    Call& procedureCall = procedureCalls.last();
    RulesForArgument rulesForArgument = procedureCall.lastRulesForArgument_dynamicCheck();
    if(procedureCall.isRulesInRange(rulesForArgument) == Error::Error /*or
            rulesForArgument->status == RulesForArguments::Type::Status::Unspecified*/){
        rulesForArgument = procedureCall.rulesForUnspecifiedArgument();
    }
    bool ruleCondtionsPassed = false;
    using Actions = Rules::value_type::ExecutableActions;
    using Action = Actions::Iterator;
    for(Rule rule = rulesForArgument->rules.begin(); rule < rulesForArgument->rules.end(); rule++){
        using Conditions = Rules::value_type::ConditionalActions;
        using Condition = Conditions::Iterator;
        Condition condition;
        ConditionResult conditionResult = ConditionResult::Satisfied;
        for(condition = rule->conditions.begin(); condition < rule->conditions.end(); condition++){
            conditionResult = (this->*(conditionalInterpreterFunctions[static_cast<std::underlying_type_t<Conditions::value_type::ActionType>>(condition->type())]))(condition->parameters());
            if(tclInterpreter.isError()){
                return Error::Error;
            }
            if(conditionResult == ConditionResult::Unsatisfied){
                break;
            }
        }
        if(condition == rule->conditions.end()){
            ruleCondtionsPassed = true;
            for(Action action = rule->actions.begin(); action < rule->actions.end(); action++){
                (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Actions::value_type::ActionType>>(action->type())]))(action->parameters());
                if(tclInterpreter.isError()){
                    return Error::Error;
                }
            }
            switch(rule->controlFlag){
            //case Rules::value_type::Control::BreakRuleCheckDontExecOnEndActions:
                //return Error::NoError;
            case Rules::value_type::Control::BreakRuleCheck:
                rule = rulesForArgument->rules.end();
                break;
            case Rules::value_type::Control::NoBreakRuleCheck:
                ruleCondtionsPassed = false;
                break;
            default:
                break;
            }
        }
    }
    return Error::NoError;
}


Error TclProcedureInterpreter::dynamicProcedureCheck(){
    if(procedureCalls.isEmpty() or
            procedureCalls.last().isRulesEmpty() or
            procedureCalls.last().isUserInteractionRequired())
        return Error::NoError;
    return dynamicProcedureArgumentCheck_priv();
}

Error TclProcedureInterpreter::onArgumentProcedureCheck_priv(){
    using RulesForArguments = ProcedureDefinition::RulesForArguments;
    using RulesForArgument = ProcedureDefinition::RulesForArguments::Iterator;
    using Rules = ProcedureDefinition::Rules;
    using Rule = ProcedureDefinition::Rules::Iterator;
    Call& procedureCall = procedureCalls.last();
    ProcedureDefinition::RulesForArguments::Iterator rulesForArgument = procedureCall.lastRulesForArgument_onMoved();
    if(procedureCall.isRulesInRange(rulesForArgument) == Error::Error){
        rulesForArgument = procedureCall.rulesForUnspecifiedArgument();
    }
    bool ruleCondtionsPassed = false;
    using Actions = Rules::value_type::ExecutableActions;
    using Action = Actions::Iterator;
    for(Rule rule = rulesForArgument->rulesOnMoveArgument.begin(); rule < rulesForArgument->rulesOnMoveArgument.end(); rule++){
        using Conditions = Rules::value_type::ConditionalActions;
        using Condition = Conditions::Iterator;
        Condition condition;
        ConditionResult conditionResult = ConditionResult::Satisfied;
        for(condition = rule->conditions.begin(); condition < rule->conditions.end(); condition++){
            conditionResult = (this->*(conditionalInterpreterFunctions[static_cast<std::underlying_type_t<Conditions::value_type::ActionType>>(condition->type())]))(condition->parameters());
            if(tclInterpreter.isError()){
                return Error::Error;
            }
            if(conditionResult == ConditionResult::Unsatisfied){
                break;
            }
        }
        if(condition == rule->conditions.end()){
            ruleCondtionsPassed = true;
            for(Action action = rule->actions.begin(); action < rule->actions.end(); action++){
                (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Actions::value_type::ActionType>>(action->type())]))(action->parameters());
                if(tclInterpreter.isError()){
                    return Error::Error;
                }
            }
            switch(rule->controlFlag){
            //case Rules::value_type::Control::BreakRuleCheckDontExecOnEndActions:
              //  return Error::Error;
            case Rules::value_type::Control::BreakRuleCheck:
                rule = rulesForArgument->rulesOnMoveArgument.end();
                break;
            case Rules::value_type::Control::NoBreakRuleCheck:
                ruleCondtionsPassed = false;
                break;
            default:
                break;
            }
        }
    }
    return Error::NoError;
}


Error TclProcedureInterpreter::onArgumentProcedureCheck(){
    if(procedureCalls.isEmpty() or
            procedureCalls.last().isRulesEmpty() or
            procedureCalls.last().isUserInteractionRequired())
        return Error::NoError;
    return onArgumentProcedureCheck_priv();

}

Error TCLInterpreter::toCAPL(TclCommand &tclCommand){
    if(isError())
        return Error::Error;
    textInterpreter.initialize(tclCommand);
    proccessingStats = pendingProccessingStats;
    pendingProccessingStats = {};
    while(!proccessingStats.isEmpty()){
        Stat savedProccessingStat = proccessingStats.last();

        if(callInterpretFunction() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }
        commandsController.dynamicProcedureCheck();
        if(isError()){
            if(processError() == Error::Error)
                return Error::Error;
        }
        if(savedProccessingStat == Stat::EndOfString){
            textInterpreter.deinitialize();
            return Error::NoError;
        }
    }
    pendingProccessingStats = {};
    while(textInterpreter.runSearchingMode() == Result::StatFound){
        if(textInterpreter.throwErrorIfUnknownStringForForbiddenRule() == Error::Error){
            throwError("TCL Interpreter: Unknown String is Forbidden");
            if(processError() == Error::Error)
                return Error::Error;
        }
        proccessingStats.append(textInterpreter.lastKeywordStats());

        if(processUnknownString() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }


        while(!proccessingStats.isEmpty()){
            Stat savedProccessingStat = proccessingStats.last();
            if(callInterpretFunction() == Error::Error){
                if(processError() == Error::Error)
                    return Error::Error;
            }
            commandsController.dynamicProcedureCheck();
            if(isError()){
                if(processError() == Error::Error)
                    return Error::Error;
            }
            if(savedProccessingStat == Stat::EndOfString){
                textInterpreter.deinitialize();
                return Error::NoError;
            }
        }
    }

    return isError()? Error::Error : Error::NoError;
}

Error TCLInterpreter::processSavedStatsForError(){
    while(!isSavedStatsEmpty()){
        switch(lastSavedStat().stat()){
        case Stat::Script:
        case Stat::MainScript:
            saveStat({Stat::Ignore});
            return Error::NoError;
        case Stat::CommandSubbing:
//        case Stat::Snprintf:
//        case Stat::PendingSnprintf:
//            if(removeProcedureCall() == Error::Error)
//                return throwError("TCLInterpreter::processSavedStatsForError: " + error());
        default:
            if(removeLastSavedStatWithParsingControl() == Error::Error)
                return throwError("TCLInterpreter::processSavedStatsForError: " + error() );
        }
    }
    return throwError("TCLInterpreter::processSavedStatsForError: No stats");
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

bool TCLInterpreter::isPredefinitionMode(){
    return userConfig.proceduresSettings().mode() == UserInputConfig::Settings::InterpreterMode::PredefinitionsOnly;
}

// PROCEDURE DEFINITION FUNCTIONS ==================================================================================
QString TclProcedureInterpreter::toString(SavedStat &stat, ProcedureDefinition::Format::Target target){
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
    }*/
    return QString();
}


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
                                dest += lastProcedureName();
                                break;
                            case Target::Command:
                                dest += command;
                                break;
                            /*case Target::RAW:
                            //case Target::SSTR:
                                if(!targetStr)
                                    return false;
                                dest += *targetStr;
                                break;*/
                            default:
                                return (arg - args.begin());
                            }
                            // FULL LINE ----------------
                        }else{
                            // ARGUMENT AT INDEX -------------------------------
                            if((index = arg->toInt(&ok), !ok))
                                return (arg - args.begin());
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = lastProcedureCall().parametersLength() + index; // Index is negative
                            }
                            if(index < 0 || index >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            switch(target){
                            /*case Target::RAW:
                            //case Target::SSTR:
                                if(!targetStr)
                                    return false;
                                if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    index = targetStr->size() + index; // Index is negative
                                }
                                if(index < 0 || index >= targetStr->size())
                                    return false;
                                dest += targetStr->at(index);
                                break;*/
                            case Target::Command:
                                dest += lastProcedureCall().parameters()[index].command();
                                break;
                            case Target::ProcedureParametersStat:
                                dest += QString::number(
                                            std::underlying_type_t<Stat>(lastProcedureCall().parameters().at(index).stat()));
                                break;
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
                        if((index = arg->toInt(&ok), !ok))
                            return (arg - args.begin());
                        if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                            index = lastProcedureCall().parametersLength() + index; // Index is negative
                        }
                        if(index < 0)
                            return (arg - args.begin());
                        switch(target){
                        /*case Target::RAW:
                        //case Target::SSTR:
                        {
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = targetStr->size() + index; // Index is negative
                            }
                            if(index < 0 || index >= targetStr->size())
                                return false;
                            dest += targetStr->sliced(index);
                        }
                            break;*/
                        case Target::SnprintfFormat:

                        case Target::CaplFormat:
                        case Target::TclFormat:
                        case Target::Command:
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
                    /*case Rule::ARG_IN_RANGE_P1:
                    {
                        switch((formatRule = static_cast<Rule>(arg->at(0).toLatin1()), *arg = arg->sliced(1), formatRule)){
                        case Rule::ARG_IN_RANGE_P2:
                        {
                            // ALL ARGUMENTS IN RANGE -------------------------------
                            int firstIndex = INT_MAX;
                            int secondIndex = INT_MAX;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (firstIndex = indexes.at(0).toInt(&ok), !ok) ||
                                    (secondIndex = indexes.at(1).toInt(&ok), !ok))
                                return false;
                            switch(target){
                            case Target::RAW:
                            case Target::SSTR:
                            {
                                if(!targetStr)
                                    return false;
                                if(firstIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    firstIndex = targetStr->size() + firstIndex; // Index is negative
                                }
                                if(secondIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    secondIndex = targetStr->size() + secondIndex; // Index is negative
                                }
                                if(firstIndex < 0 || firstIndex >= targetStr->size() ||
                                        secondIndex < 0 || secondIndex >= targetStr->size() ||
                                        firstIndex >= secondIndex)
                                     return false;
                                for(QString::Iterator responseArg = targetStr->begin() + firstIndex; responseArg < targetStr->begin() + secondIndex; responseArg++){
                                    dest += *responseArg + seperator;
                                    separatorUsed = true;
                                }
                            }
                                break;
                            case Target::SPLITTED_RAW:
                            {
                                if(firstIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    firstIndex = lastActionResponse.size() + firstIndex; // Index is negative
                                }
                                if(secondIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    secondIndex = lastActionResponse.size() + secondIndex; // Index is negative
                                }
                                if(firstIndex < 0 || firstIndex >= lastActionResponse.size() ||
                                        secondIndex < 0 || secondIndex >= lastActionResponse.size() ||
                                        firstIndex >= secondIndex)
                                     return false;
                                for(QStringList::Iterator responseArg = lastActionResponse.begin() + firstIndex; responseArg < lastActionResponse.begin() + secondIndex; responseArg++){
                                    dest += *responseArg + seperator;
                                    separatorUsed = true;
                                }
                            }
                                break;
                            default:
                                return false;
                            }
                            // ALL ARGUMENTS IN RANGE -------------------------------
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;
                    case Rule::CUT_INDEX:
                    {
                        switch(target){
                        case Target::RAW:
                        case Target::SSTR:
                        {
                            return false;   // Not Implemented yet
                        }
                            break;
                        case Target::SPLITTED_RAW:
                        {
                            int cutWidth = 0;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (index = indexes.at(0).toInt(&ok), !ok) || index < 0 || index >= lastActionResponse.size()||
                                    (cutWidth = indexes.at(1).toInt(&ok), !ok) || cutWidth < 1 || cutWidth > lastActionResponse.at(index).size() )
                                return false;
                            int lastI = lastActionResponse.at(index).size() - cutWidth;
                            for(int i = 0; i <= lastI; i+= cutWidth){
                                dest += lastActionResponse.at(index).sliced(i, cutWidth) + seperator;
                                separatorUsed = true;
                            }
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;
                    case Rule::CUT_AFTER_INDEX:
                    {
                        switch(target){
                        case Target::RAW:
                        case Target::SSTR:
                        {
                            return false;   // Not Implemented yet
                        }
                            break;
                        case Target::SPLITTED_RAW:
                        {
                            int cutWidth = 0;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (index = indexes.at(0).toInt(&ok), !ok) || index < 0 || index >= lastActionResponse.size()||
                                    (cutWidth = indexes.at(1).toInt(&ok), !ok) || cutWidth < 1 || cutWidth > lastActionResponse.at(index).size() )
                                return false;
                            for(QStringList::Iterator responseArg = lastActionResponse.begin() + index + 1; responseArg < lastActionResponse.end(); responseArg++){
                                int lastI = responseArg->size() - cutWidth;
                                for(int i = 0; i <= lastI; i+= cutWidth){
                                    dest += responseArg->sliced(i, cutWidth) + seperator;
                                    separatorUsed = true;
                                }
                            }
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;
                        */
                    case Rule::TARGET:
                    {
                        if((target = static_cast<Target>(arg->toUInt(&ok)), !ok))
                           return (arg - args.begin());

                    }
                        break;
                    /*case Rule::SPLIT:
                    {
                        switch (target) {
                        case Target::RAW:
                        case Target::SSTR:
                            if(!targetStr)
                                return false;
                            lastActionResponse = targetStr->split(QRegularExpression(*arg), Qt::SkipEmptyParts);
                            break;
                        case Target::SPLITTED_RAW:
                        default:
                            return false;
                        }
                    }
                        break;
                    case Rule::SLICED:
                    {
                        switch (target) {
                        case Target::RAW:
                        case Target::SSTR:
                        {
                            if(!targetStr)
                                return false;
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = targetStr->size() + index; // Index is negative
                            }
                            if(index < 0 || index >= targetStr->size())
                                return false;
                            *targetStr = targetStr->sliced(index);
                        }
                            break;
                        case Target::SPLITTED_RAW:
                        {
                            int firstIndex = INT_MAX;
                            int secondIndex = INT_MAX;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (firstIndex = indexes.at(0).toInt(&ok), !ok) ||
                                    (secondIndex = indexes.at(1).toInt(&ok), !ok))
                                return false;
                            if(firstIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                firstIndex = lastActionResponse.size() + firstIndex; // Index is negative
                            }
                            if(firstIndex < 0 || firstIndex >= lastActionResponse.size())
                                return false;
                            if(secondIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                secondIndex = lastActionResponse.at(firstIndex).size() + secondIndex; // Index is negative
                            }
                            if(secondIndex < 0 || secondIndex >= lastActionResponse.at(firstIndex).size())
                                return false;
                            lastActionResponse[firstIndex] = lastActionResponse.at(firstIndex).sliced(secondIndex);
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;*/
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




const TclProcedureInterpreter::NewProcedureCallFunction
TclProcedureInterpreter::ProcedureCallFunctions::newProcedureCalls
[TclProcedureInterpreter::ProdecuresSettings::mode2number(Settings::InterpreterMode::NumbOfModes)] =
{
    &TclProcedureInterpreter::newProcedureCall_mode<Settings::InterpreterMode::TestCase>,
    &TclProcedureInterpreter::newProcedureCall_mode<Settings::InterpreterMode::TestCaseReport>,
    &TclProcedureInterpreter::newProcedureCall_mode<Settings::InterpreterMode::PredefinitionsOnly>,
};


const TclProcedureInterpreter::FinalizeProcedureCallFunction
TclProcedureInterpreter::ProcedureCallFunctions::finalizeProcedureCalls
[TclProcedureInterpreter::ProdecuresSettings::mode2number(Settings::InterpreterMode::NumbOfModes)] =
{
    &TclProcedureInterpreter::finalizeProcedureCall_mode<Settings::InterpreterMode::TestCase>,
    &TclProcedureInterpreter::finalizeProcedureCall_mode<Settings::InterpreterMode::TestCaseReport>,
    &TclProcedureInterpreter::finalizeProcedureCall_mode<Settings::InterpreterMode::PredefinitionsOnly>,
};

