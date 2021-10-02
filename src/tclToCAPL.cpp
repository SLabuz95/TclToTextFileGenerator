#include"tclToCAPL.hpp"
#include"tclToCAPL_errorFileConfig.hpp"
#include"Tcl2Capl/controller.hpp"
#include"Tcl2Capl/caplFunctionDefiniitions.hpp"

using Interpreter = TCLInterpreter;
using TextInterpreter = TCLInterpreter::TextInterpreter;
using TclProcedureInterpreter = TCLInterpreter::TCLProceduresInterpreter;
using Error = Interpreter::Error;
using Result = TextInterpreter::Result;
using ReadIgnoreResult = TextInterpreter::ReadIgnoreResult;
using CheckingResult = TextInterpreter::CheckingResult;
using KeywordsMap = TextInterpreter::KeywordsMap;
using TCLInterpreterFunctions = Interpreter::InterpretFunctions;
using UserInteractionStatus = TclProcedureInterpreter::UserInteractionStatus;
using Rule = TclProcedureInterpreter::ProcedureDefinition::Rule;
using Action = TclProcedureInterpreter::ProcedureDefinition::Action;
using RuleControlFlag = Rule::Control;
using Format = TclProcedureInterpreter::ProcedureDefinition::Format;
using FormatRule = TclProcedureInterpreter::ProcedureDefinition::Format::Rule;
using FormatTarget = TclProcedureInterpreter::ProcedureDefinition::Format::Target;


QString TCLInterpreterPriv::_error = QString();

TCLInterpreter::TCLInterpreter(UserInputConfig& userConfig, CAPLFunctionDefinitionsRef caplFunctionDefinitionsRef)
    : tclProceduresInterpreter(*this, userConfig), caplFunctionDefinitions(caplFunctionDefinitionsRef), userConfig(userConfig)
{clearError();}

TclProcedureInterpreter::TCLProceduresInterpreter(TCLInterpreter& tclInterpreter, UserInputConfig& userConfig) :
    tclInterpreter(tclInterpreter),
    procedureDefinitions(
        (userConfig.userProcedureConfig().isEmpty())?
            defaultProcedureDefinitions
          : userConfig.userProcedureConfig()),
    unknownProcedureDefinition(
        (userConfig.userDefaultProcedureConfig().isRulesEmpty())?
            defaultUnknownProcedureDefinition
          : userConfig.userDefaultProcedureConfig()),
    newProcedureCallFunction(ProcedureCallFunctions::newCallAt(userConfig.proceduresSettings().mode())),
    finalizeProcedureCallFunction(ProcedureCallFunctions::finalizeCallAt(userConfig.proceduresSettings().mode()))
{}

KeywordsMap TextInterpreter::keywordsMap ={
    {
        {"#", {TCLStat::Comment}},
        {"{", {TCLStat::List}, Keyword::UnknownStringRule::Forbidden},
        {"}", {TCLStat::EndOfList}},
        {"[", {TCLStat::LeftSquareBracket}},
        {"]", {TCLStat::RightSquareBracket}},
        {"(", {TCLStat::Operator}},
        {")", {TCLStat::Operator}},
        {"+", {TCLStat::Operator}},
        {"-", {TCLStat::Operator}},
        {"*", {TCLStat::Operator}},
        {"/", {TCLStat::Operator}},
        {"%", {TCLStat::Operator}},
        {">", {TCLStat::Operator}},
        {"<", {TCLStat::Operator}},
        {"!", {TCLStat::Operator}},
        {"&", {TCLStat::Operator}},
        {"|", {TCLStat::Operator}},
        {"^", {TCLStat::Operator}},
        {"?", {TCLStat::Operator}},
        {":", {TCLStat::Operator}},
        {" ", {TCLStat::Whitespace}},
        {"\t", {TCLStat::Whitespace}},
        {"$", {TCLStat::VariableAccess}},
        {"\"", {TCLStat::SpeechMark}},
        {";", {TCLStat::Semicolon}},
        {"\\", {TCLStat::SpecialSign}}

    },
    {
        {"::", {TCLStat::GlobalAccess}},
        {"==", {TCLStat::Operator}},
        {"!=", {TCLStat::Operator}},
        {">=", {TCLStat::Operator}},
        {"<=", {TCLStat::Operator}},
        {"&&", {TCLStat::Operator}},
        {"||", {TCLStat::Operator}},
        {"<<", {TCLStat::Operator}},
        {">>", {TCLStat::Operator}},
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
StartUpErrorBase::Message StartUpError<TextInterpreter>::__startupErrorChecking(){
    using Keywords = KeywordsMap::Iterator;
    using Keyword = KeywordsMap::value_type::Iterator;
    KeywordsMap::size_type size = 0;
    for(Keywords keywords = TextInterpreter::keywordsMap.begin(); keywords < TextInterpreter::keywordsMap.end(); keywords++){
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
        Stat::Operator,
        Stat::UnknownString,
        Stat::LeftSquareBracket,
        Stat::RightSquareBracket,
        Stat::List,
        Stat::EndOfList,
        Stat::SpeechMark,
        Stat::Whitespace,
        Stat::VariableAccess,
        Stat::GlobalAccess,
        Stat::Semicolon,
        Stat::Comment,
        Stat::EndOfString,
        //Stat::CodeBlock,
        //Stat::EndOfCodeBlock,
        Stat::SpecialSign,
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
using ProcedureDefinition = TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition;
const TclProcedureInterpreter::ProcedureDefinition::Rule defaultRuleForUnknownProcedureDefinition_onEndOfCall =
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
        {   // Action 2: Add to CaplFunctionDefinitions
            ProcedureDefinition::Action::Executable::AddFunctionDefinition,
            {}
        }
    }
};

TclProcedureInterpreter::ProcedureDefinitions TclProcedureInterpreter::defaultProcedureDefinitions = {
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
                            "continue;",
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
                            "return;",
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
                            "=0",
                            ";"
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
                            ");"
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
        {   // On Move Rules

        },
        {   // On End of Call
            {   // Rule 1: Tcl Parse -> write Result
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
                    {   // Action 2: Add to CaplFunctionDefinitions
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
                             {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfList)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::FunctionCall)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::SpecialSign)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Operator)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::FunctionCall)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingSnprintf)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingString))}
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
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List))}
                            },
                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock))}
                           }
                        }
                    },
                    {   // Rule 2: On lastSavedStat == List -> Change to CodeBlock
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfString)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::SpecialSign)),}
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
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List))}
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
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock))}
                       }
                    }
                },
                {   // Rule 2: On lastSavedStat == List -> Change to CodeBlock
                    {
                        {
                          ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List))}
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
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock))}
                       }
                    }
                }
            },
            {   // Rules on moveArgument
                {   // Rule 1: if lastSavedStat caplcommand == (elseif) -> Error
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
                {   // Rule 2: if lastSavedStat caplcommand == (elseif) -> P
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
                {   // Rule 3: If lastSavedStat caplCommand == else -> {\n =-1 \n}
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
                {   // Rule 4: If PreLastSavedStat caplCommand == else -> {\n =-1 \n}
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
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List))}
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
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                 QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::FunctionCall)),}
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
                             {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List)),
                              QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfList)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::FunctionCall)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::SpecialSign)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Operator)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::FunctionCall)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingSnprintf)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::PendingString)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfString)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),}
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
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List))}
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
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfList)),
                                 QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Comment)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Ignore)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfString)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::SpecialSign))}
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
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List))}
                            },
                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock))}
                           }
                        }
                    },
                    {   // Rule 2: On Ignore -> Ignore
                        {   // Conditions:  Ignore
                            {   // Condition 1:  Ignore
                                ProcedureDefinition::Action::Conditional::IsLastSavedStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfCodeBlock)),
                                 QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::EndOfExpression)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Whitespace)),
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock)),
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
     {
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
                                    Interpreter::cast_stat_str(Stat::String),
                                    Interpreter::cast_stat_str(Stat::PendingString),
                                    Interpreter::cast_stat_str(Stat::EndOfList),

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
                                    Interpreter::cast_stat_str(Stat::StringInQuotes),
                                    Interpreter::cast_stat_str(Stat::EndOfList),
                                    Interpreter::cast_stat_str(Stat::SpeechMark),
                                    Interpreter::cast_stat_str(Stat::PendingSnprintf),
                                    Interpreter::cast_stat_str(Stat::Variable),
                                    Interpreter::cast_stat_str(Stat::Snprintf),                                    
                                    Interpreter::cast_stat_str(Stat::FunctionCall),
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
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::List))}
                             },
                         },
                         {
                            {    // Action 1: Change stat to CodeBlock
                                  ProcedureDefinition::Action::Executable::ChangeLastSavedStat,
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock))}
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
                                 QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::CodeBlock)),
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
         {   // On End of Call
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
         }
     }
};


TclProcedureInterpreter::ProcedureDefinition TclProcedureInterpreter::defaultUnknownProcedureDefinition =
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


void TCLInterpreterPriv::ListStatInfo::newList(CAPLCommand caplCommand){
    if(lastListInfo){
        if(!caplCommand.isEmpty())
            lastListInfo->listInfos.append(ListInfo{{}, caplCommand});

        lastListInfo->listInfos.append(ListInfo{});
        lastListInfo = lastListInfo->listInfos.end() - 1;
    }else{
        if(!caplCommand.isEmpty())
            listInfos.append(ListInfo{{}, caplCommand});

        listInfos.append(ListInfo{});
        lastListInfo = listInfos.end() - 1;
    }

}

TCLInterpreterPriv::ListStatInfo::EndListResult
TCLInterpreterPriv::ListStatInfo::endList(CAPLCommand caplCommand){
    if(lastListInfo){
        if(!caplCommand.isEmpty())
            lastListInfo->listInfos.append(ListInfo{{}, caplCommand});

        ListInfos::Iterator prelastListInfo = listInfos.end() - 1;
        if(prelastListInfo == lastListInfo){
            lastListInfo = nullptr;
        }else{
            while(prelastListInfo->listInfos.end() - 1 != lastListInfo)
                prelastListInfo = prelastListInfo->listInfos.end() - 1;
            lastListInfo = prelastListInfo;
        }
    }else{
        if(!caplCommand.isEmpty())
            listInfos.append(ListInfo{{}, caplCommand});

        return EndListResult::EndOfList;
    }
    return EndListResult::NoEndOfList;
}

QString TCLInterpreterPriv::ListStatInfo::ListInfo::toString(const QString& separator)
{
    using Info = ListInfos::Iterator;
    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;

    if(strData.isEmpty()){
    QString str;

    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

    /*while(!loopControlData.isEmpty()){
        if(loopControlData.last().first < loopControlData.last().second)
        {
            Info info = loopControlData.last().first;
            if(info->strData.isEmpty()){
                loopControlData.append(LoopControlData(info->listInfos.begin(), info->listInfos.end()));
                loopControlData.
            }else{

            }
        }else{
            loopControlData.removeLast();
        }
    }*/
    if(!listInfos.isEmpty()){
        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
            str += info->toString(separator) + separator;
        str.chop(separator.length());
    }

        return QString("{") + str + "}";
    }else{
        return strData;
    }
}

void Interpreter::addEmptyLine(){
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
        case Stat::MainCodeBlock:
            addExpressionToMainCodeBlock({" "});
            break;
        case Stat::CodeBlock:
            addExpressionToCodeBlock({" "});
            break;
        case Stat::List:
            lastSavedStat().appendCAPLCommand("\n");
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

QString TCLInterpreterPriv::ListStatInfo::ListInfo::toCaplListString()
{
    using Info = ListInfos::Iterator;
    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;
    const QString separator = QString("");

    if(strData.isEmpty()){
        QString str;

    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

    /*while(!loopControlData.isEmpty()){
        if(loopControlData.last().first < loopControlData.last().second)
        {
            Info info = loopControlData.last().first;
            if(info->strData.isEmpty()){
                loopControlData.append(LoopControlData(info->listInfos.begin(), info->listInfos.end()));
                loopControlData.
            }else{

            }
        }else{
            loopControlData.removeLast();
        }
    }*/
    if(!listInfos.isEmpty()){
        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
            str += info->toString(separator) + separator;
        str.chop(separator.length());
    }
        return str;
    }else{
        return strData.trimmed();
    }
}

QString TCLInterpreterPriv::ListStatInfo::toCaplListString()
{
    using Info = ListInfos::Iterator;
    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;
    const QString separator = QStringLiteral(", ");

    QString str;
    QString tempStr;

    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

    /*while(!loopControlData.isEmpty()){
        if(loopControlData.last().first < loopControlData.last().second)
        {
            Info info = loopControlData.last().first;
            if(info->strData.isEmpty()){
                loopControlData.append(LoopControlData(info->listInfos.begin(), info->listInfos.end()));
                loopControlData.
            }else{

            }
        }else{
            loopControlData.removeLast();
        }
    }*/
    if(!listInfos.isEmpty()){
        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
            if(!(tempStr = info->toCaplListString()).isEmpty())
                str += QString("\"") + tempStr + "\"" + separator;
        if(str.endsWith(separator))
            str.chop(separator.length());
    }

    return QString("{") + str + "}";

}

QString TCLInterpreterPriv::ListStatInfo::toString(const QString& separator)
{
    using Info = ListInfos::Iterator;
    //using LoopControlData = QVector<QPair<Info, Info>>/*currentIteraror, endIterator*/;
    QString str;

    //LoopControlData loopControlData{LoopControlData(listInfos.begin(), listInfos.end())};

    /*while(!loopControlData.isEmpty()){
        if(loopControlData.last().first < loopControlData.last().second)
        {
            Info info = loopControlData.last().first;
            if(info->strData.isEmpty()){

            }else{

            }
        }else{
            loopControlData.removeLast();
        }
    }*/
    if(!listInfos.isEmpty()){
        for(Info info = listInfos.begin(); info < listInfos.end(); info++)
            str += info->toString(separator) + separator;
        str.chop(separator.length());
    }
    return str;
}

QString TCLInterpreterPriv::SavedStat::listToCaplString(){
    QString str = "{";
    if(info){
        str += info->toCaplListString();
    }else{
        QStringList elements = _caplCommand.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for(QStringList::Iterator element = elements.begin(); element < elements.end(); element++)
            *element = QString("\"") + *element + "\"";
        str += elements.join(", ");
    }
    return str + "}";
}


QString TclProcedureInterpreter::ProcedureCall::Parameter::toString(ProcedureDefinition::Format::Target target){
    using Target = ProcedureDefinition::Format::Target;

    switch(target){
    case Target::Command:
        return caplCommand();
    case Target::SnprintfFormat:
    {
        // Temporary
        return QString("%s");
        /*
        switch(stat()){
        case Stat::String:
        case Stat::SpeechMark:
        case Stat::PendingString:
        case Stat::StringInQuotes:
            return QString("\"") + caplCommand() + "\"";
        case Stat::EndOfCodeBlock:
            return QString("{\n") + caplCommand() + "\n}\n";
        case Stat::EndOfList:
            return savedStat().listToCaplString();
        default:
            return caplCommand();
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
        case Stat::String:
        case Stat::SpeechMark:
        case Stat::PendingString:
        case Stat::StringInQuotes:
            return QString("\"") + caplCommand() + "\"";
        case Stat::EndOfCodeBlock:
            return QString("{") + caplCommand() + "}";
        case Stat::EndOfList:
            return savedStat().listToCaplString();
        default:
            return caplCommand();
        }
    }
        break;
    case Target::TclFormat:
    {
        using Parameter = Parameters::Iterator;
        QString str;
        switch (stat()) {
        case Stat::Variable:
            return QString("$") + caplCommand();
        case Stat::EndOfList:
            return savedStat().listToTclListString();
        case Stat::SpeechMark:
        case Stat::StringInQuotes:
            return QString("\"") + caplCommand() + "\"";
        case Stat::FunctionCall:
        {
            str = QString("[");
            for(Parameter parameter = rawParameterStats.begin(); parameter < rawParameterStats.end();  parameter++)
                str += parameter->toString(target) + " ";
            str += QString("]");
            return str;
        }
            break;
        case Stat::Snprintf:
        {
            str = QString("\"");
            for(Parameter parameter = rawParameterStats.begin() + 2; parameter < rawParameterStats.end();  parameter++)
                str += parameter->toString(target);
            str += QString("\"");
            return str;
        }
            break;
        case Stat::PendingSnprintf:
        {
            for(Parameter parameter = rawParameterStats.begin() + 2; parameter < rawParameterStats.end();  parameter++)
                str += parameter->toString(target);
            return str;
        }
            break;
        case Stat::EndOfExpression:
        {
            throwError("Expression to string for TclFormat not implemented");
        }
            break;
        case Stat::EndOfCodeBlock:
        {
            throwError("CodeBlock to string for TclFormat not implemented");
        }
            break;
        default:
            return caplCommand();
        }
    }
    }
    return QString();
}


Error Interpreter::newProcedureCall(TclProcedureInterpreter::ProcedureCall::Name name){
    const QString ERROR_PREFIX = "TCLInterpreter::newProcedureCall: ";


    if(name == "proc")
        qDebug() << "proc";

    switch(lastSavedStat().stat()){
    case Stat::LeftSquareBracket:
        // Do not add predefinitions group
        break;
    case Stat::CodeBlock:
        predefinitionsControl().newGroup(savedStatsSize());
        break;
    default:
        break;
    }

    if(tclProceduresInterpreter.newProcedureCall(name) == Error::Error)
        return throwError(ERROR_PREFIX + error());


    return Error::NoError;
}

Error Interpreter::removeProcedureCall_writeOnlyProcedure(){
    tclProceduresInterpreter.tryToDeactivateWriteOnlyProcedure();
    return removeProcedureCall_standard();
}

Error Interpreter::saveStatWithParsingControl(SavedStat newSavedStat){
    // No Control for now
    saveStat(newSavedStat);
    return Error::NoError;
}

Error Interpreter::removeLastSavedStatWithParsingControl(){
    // No Control for now
    return removeLastStat();
}

Error Interpreter::takeLastSavedStatWithParsingControl(SavedStat& savedStat){
    // No Control for now
    lastSavedStat().hideInfoBeforeCopyConstructor();
    return (savedStat = takeLastSavedStat(), Error::NoError);
}

Error Interpreter::moveArgumentToFunctionCall(){
    const QString ERROR_DESCRIPTION = QString("InternalCall: moveArgumentToFunctionCall() : ");
     if(!isPrelastSavedStat())
        return throwError(ERROR_DESCRIPTION + "Empty Stats");
    switch (prelastSavedStat().stat()) {
    case Stat::Snprintf:
    case Stat::PendingSnprintf:
        return throwError(ERROR_DESCRIPTION + "No Function Call (Snprintf or PendingSnprintf Found) " );
        break;
    case Stat::FunctionCall:
        break;
    default:
        return throwError(ERROR_DESCRIPTION + "No Function Call" );
    }
    if(prelastSavedStat().isFunctionReady())
        return throwError(ERROR_DESCRIPTION + "Ready Function Call" );

    return (tclProceduresInterpreter.nextArgument() == Error::Error or
                tclProceduresInterpreter.onArgumentProcedureCheck() == Error::Error)
            ?
                Error::Error :
                Error::NoError;
}

Error Interpreter::moveArgumentToSnprintf_priv(Stat stat){
    using Parameter = TCLProceduresInterpreter::ProcedureCall::Parameter;
    const QString ERROR_DESCRIPTION = QString("InternalCall: Interpreter::moveArgumentToSnprintf_priv(): ");
    if(!isPrelastSavedStat())
        return throwError("No Stats for snprintf processing");
    switch(prelastSavedStat().stat()){
    case Stat::Snprintf:
    case Stat::PendingSnprintf:
    {
        if(tclProceduresInterpreter.nextArgument() == Error::Error or
                tclProceduresInterpreter.onArgumentProcedureCheck() == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    default:
    {
        if(tclProceduresInterpreter.nextArgumentForSnprintf_priv(stat) == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
    }
    return Error::NoError;
}

void TCLInterpreter::TCLProceduresInterpreter::addDefaultProcedureDefinitionsToUserProcedureDefintions(UserInputConfig& userDefinitions){

    if(!userDefinitions.userProcedureConfig().isEmpty()){
       for(ProcedureDefinitionIterator defaultDefinition = defaultProcedureDefinitions.begin();
           defaultDefinition < defaultProcedureDefinitions.end(); defaultDefinition++)
       {
           ProcedureDefinitionIterator definition;
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
        if(userDefinitions.userDefaultProcedureConfig().rulesForArguments.size() < defaultUnknownProcedureDefinition.rulesForArguments.size()){
            userDefinitions.userDefaultProcedureConfig().rulesForArguments.resize(defaultUnknownProcedureDefinition.rulesForArguments.size());
            loopEnd = userDefinitions.userDefaultProcedureConfig().rulesForArguments.end();
        }else{
            loopEnd = userDefinitions.userDefaultProcedureConfig().rulesForArguments.begin() + defaultUnknownProcedureDefinition.rulesForArguments.size();
        }

        // Rules fo Unspecified Arguments
        userDefinitions.userDefaultProcedureConfig().rulesForUnspecifiedArgument.rules.append(defaultUnknownProcedureDefinition.rulesForUnspecifiedArgument.rules);
        userDefinitions.userDefaultProcedureConfig().rulesForUnspecifiedArgument.rulesOnMoveArgument.append(defaultUnknownProcedureDefinition.rulesForUnspecifiedArgument.rulesOnMoveArgument);

        // Rules on End of Call
       userDefinitions.userDefaultProcedureConfig().rulesOnEndOfProcedureCall.append(defaultUnknownProcedureDefinition.rulesOnEndOfProcedureCall);

    }
}

template<>
Error Interpreter::interpret<Interpreter::Stat::UnknownString>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::UnknownString>: ";
    SavedStat unknownStringStat;
    bool whitespace = false;
    // Preconditions
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");
    if(lastSavedStat().stat() != Stat::UnknownString)
        return throwError(ERROR_PREFIX + "Lack of Saved Unknown String stat for Unknown String processing stat");

    // Initialize
    unknownStringStat = takeLastSavedStat();    // UnknownString
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Initialization");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");


    // Processing
    // #1 ------------------------------------------------------------------
    switch(lastSavedStat().stat()){
    /*case Stat::Variable:*/
    case Stat::EndOfList:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No Whitespace after List (Close-brackets Error)");
    }
    if(false){
        case Stat::String:
        case Stat::Const:
        {
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace for String or Const");
        }
    }
        //Q_FALLTHROUGH();
    if(false){
        case Stat::Variable:
        {
            if(!whitespace){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        saveStatWithParsingControl({Stat::PendingString, unknownStringStat.caplCommand()}) == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                break;
            }

        }
    }
        //Q_FALLTHROUGH();
    if(false){
        case Stat::StringInQuotes:
        {
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace for String with Quotes (Close-Quotes error)");
        }
    }
    case Stat::Operator:
    {
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(isStringConstNumber(unknownStringStat.caplCommand())){
            if(Error::Error == saveStatWithParsingControl({Stat::Const, unknownStringStat.caplCommand()}))
                    return throwError(ERROR_PREFIX + error());
        }else{  // String
            if(Error::Error == saveStatWithParsingControl({Stat::String, unknownStringStat.caplCommand()}))
                    return throwError(ERROR_PREFIX + error());
        }

    }
        break;
    case Stat::PendingSnprintf:
    {
        if(not (lastSavedStat().isFunctionReady()))
            return throwError(ERROR_PREFIX + "Incomplete Pending Snprintf ???");
        if(!whitespace)
            return throwError("No Whitespace for Complete PendingSnprintf");
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(isStringConstNumber(unknownStringStat.caplCommand())){
            if(Error::Error == saveStatWithParsingControl({Stat::Const, unknownStringStat.caplCommand()}))
                    return throwError(ERROR_PREFIX + error());
        }else{  // String
            if(Error::Error == saveStatWithParsingControl({Stat::String, unknownStringStat.caplCommand()}))
                    return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::Snprintf:
    {
        if(not (lastSavedStat().isFunctionReady())){
            if(whitespace)
                return throwError("Whitespace for Snprintf (SpeechMarks)");
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, unknownStringStat.caplCommand()}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!whitespace)
                return throwError(ERROR_PREFIX + "Chars after close-quote (Close-quotes Error)");
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(isStringConstNumber(unknownStringStat.caplCommand())){
                if(Error::Error == saveStatWithParsingControl({Stat::Const, unknownStringStat.caplCommand()}))
                        return throwError(ERROR_PREFIX + error());
            }else{  // String
                if(Error::Error == saveStatWithParsingControl({Stat::String, unknownStringStat.caplCommand()}))
                        return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){    // Pending Function Call -> Check variable type in unknown string
            if(whitespace){
                if(isStringConstNumber(unknownStringStat.caplCommand())){
                    if(Error::Error == saveStatWithParsingControl({Stat::Const, unknownStringStat.caplCommand()}))
                        return throwError(ERROR_PREFIX + error());
                }else{  // String
                    if(Error::Error == saveStatWithParsingControl({Stat::String, unknownStringStat.caplCommand()}))
                        return throwError(ERROR_PREFIX + error());
                }
            }else{
                return throwError(ERROR_PREFIX + "No whitespace after incomplete function call");
            }
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                if(isStringConstNumber(unknownStringStat.caplCommand())){
                    if(Error::Error == saveStatWithParsingControl({Stat::Const, unknownStringStat.caplCommand()}))
                        return throwError(ERROR_PREFIX + error());
                }else{  // String
                    if(Error::Error == saveStatWithParsingControl({Stat::String, unknownStringStat.caplCommand()}))
                        return throwError(ERROR_PREFIX + error());
                }
            }else{                
                if(moveArgumentToPendingSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, unknownStringStat.caplCommand()}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;

    case Stat::MainCodeBlock:   // Procedure Processing is Starting
    case Stat::CodeBlock:
    case Stat::Expression:
    {
        if(isStringConstNumber(unknownStringStat.caplCommand()))
            return throwError(ERROR_PREFIX + "Number used as procedure name");
        if(newProcedureCall(unknownStringStat.caplCommand()) == Error::Error or
            saveStatWithParsingControl({Stat::FunctionCall}) == Error::Error )
                       return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::LeftSquareBracket:
    {
        if(isStringConstNumber(unknownStringStat.caplCommand()))
            return throwError(ERROR_PREFIX + "Number used as procedure name");
        lastSavedStat() = {Stat::FunctionCall}; // _PH_
        if(newProcedureCall(unknownStringStat.caplCommand()) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::PendingString:
    {
        if(whitespace){
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No Prelast Stat");
            if(prelastSavedStat().stat() == Stat::LeftSquareBracket)
                return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");

            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(isStringConstNumber(unknownStringStat.caplCommand())){
                if(Error::Error == saveStatWithParsingControl({Stat::Const, unknownStringStat.caplCommand()}))
                    return throwError(ERROR_PREFIX + error());
            }else{  // String
                if(Error::Error == saveStatWithParsingControl({Stat::String, unknownStringStat.caplCommand()}))
                    return throwError(ERROR_PREFIX + error());
            }
        }else{            
            lastSavedStat().setCAPLCommand( lastSavedStat().caplCommand() + unknownStringStat.caplCommand());
        }
    }
        break;
    case Stat::GlobalAccess:
    {
        // At least 2 stats (size > 1) are required (Prevent QVector Error)
        // Prelast Stat must be VariableAccess
        if(whitespace){
            return throwError(ERROR_PREFIX + "Whitespace after GlobalAccess");
        }
        if(isPrelastSavedStat()){
            if(prelastSavedStat().stat() != Stat::VariableAccess){
                return throwError(ERROR_PREFIX + "GlobalAccess Stat (::) without Variable Access Stat ($)");
            }else{
                // Safe Just Remove Global Access -> Pass To Stat::VariableAccess
                if(removeLastSavedStatWithParsingControl() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
        }else{
           return throwError(ERROR_PREFIX + "Too little stats for GlobalAccess Stats");
        }
    }
    // Sprawdz lastSavedStat().caplCommand().isEmpty() poprawnosc przejsc dla lastSavedStat().isFunctionReady() !!!!!!!!!!!!!!!!!
    case Stat::VariableAccess:
    {
        if(whitespace){
            return throwError(ERROR_PREFIX + "Whitespace after VariableAccess");
        }

        if(lastSavedStat().caplCommand().isEmpty()){
            if(isStringConstNumber(unknownStringStat.caplCommand())){
                // Const in Variable Access ???
                return throwError(ERROR_PREFIX + "Const for VariableAccess");
            }else{  // String
                lastSavedStat() = {Stat::Variable};    // _PH_
                QString str = unknownStringStat.caplCommand();
                lastSavedStat().setCAPLCommand(str.replace(":", ""));
                // Check if stat before variable is Snprintf
                if(!isPrelastSavedStat())
                    return throwError(ERROR_PREFIX + "No prelast stat for VariableAccess processing");
                if(prelastSavedStat().stat() == Stat::Snprintf){
                    if(moveArgumentToSnprintf() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                }
            }
        }else{
            return throwError(ERROR_PREFIX + "VariableAccess Stat isnt empty");
        }
    }
        break;

    case Stat::List:
    {
        lastSavedStat().appendCAPLCommand(unknownStringStat.caplCommand());
    }
        break;

    case Stat::SpeechMark:  // Speech mark Processing
    {
        if(whitespace){
            return throwError(ERROR_PREFIX + "Whitespace Stat after Speech Mark (Whitespace shoudnt be saved for speech marks processing)");
        }
        lastSavedStat().appendCAPLCommand(unknownStringStat.caplCommand());
    }
        break;


    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    {
        if(!whitespace){
            return throwError("No Whitespace after Complete CodeBlock (Close Bracket Error)");
        }
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(isStringConstNumber(unknownStringStat.caplCommand())){
            if(Error::Error == saveStatWithParsingControl({Stat::Const, unknownStringStat.caplCommand()}))
                return throwError(ERROR_PREFIX + error());
        }else{  // String
            if(Error::Error == saveStatWithParsingControl({Stat::String, unknownStringStat.caplCommand()}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::Ignore:
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown String Content: " + unknownStringStat.caplCommand() +
                          " Unknown Stat");
    }
    // #1# ------------------------------------------------------------------------------------
    return Error::NoError;
}

template<>
Error Interpreter::interpret<Interpreter::Stat::LeftSquareBracket>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::LeftSquareBracket>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    /*case Stat::Variable:*/
    /*case Stat::PendingString:
    {
        savedStats.last().stat = Stat::String;
    }
        Q_FALLTHROUGH();*/
    case Stat::String:
    case Stat::Const:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(moveArgumentToPendingSnprintf() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::Variable:
    {
        if(!whitespace){
            if(moveArgumentToPendingSnprintf() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
                return throwError(ERROR_PREFIX + error());
            break;
        }
    }

    case Stat::Operator:
    {
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            // Find Other function call
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace for Function Call name");
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                // Change Function Call to PendingSnprintf
                if(moveArgumentToPendingSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
        }
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::List:
    {
        // For the list, interpret as a sign
        lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());

    }
        break;
    case Stat::EndOfList:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            return throwError(ERROR_PREFIX + "No whitespace after Complete List (Close-brackets Error)");
        }
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::StringInQuotes:
    {
        if(!whitespace){
            return throwError(ERROR_PREFIX + "No whitespace after StringInQuotes (Close-quotes Error)");
        }
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace){
                return throwError("Whitespace for Snprintf");
            }
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                return throwError("No Whitespace after Snprintf aka SpeechMark (Close-Quote Error)");
            }
        }
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            return throwError(ERROR_PREFIX + "Incomplete PendingSnprintf");
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                return throwError("No Whitespace after Snprintf aka SpeechMark (Close-Quote Error) ?");
            }
        }
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::PendingString:
    {
        if(whitespace)
        {            
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for VariableAccess processing");
            switch(prelastSavedStat().stat()){
            case Stat::Snprintf:
            {
                if(moveArgumentToSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
                break;
            case Stat::LeftSquareBracket:
            {
                return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
            }
                break;
            default:
            {
                if(moveArgumentToPendingSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            }
        }

        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::SpeechMark:
    {   // Procedure call in String
        if(whitespace)
            return throwError(ERROR_PREFIX + "Whitespace for Speech Mark Processing");

        // Change SpeechMark to Snprintf ->
        if(moveArgumentToSnprintf() == Error::Error)
            return throwError(ERROR_PREFIX + error());

        // Add Left SquareBracket as usual
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    {
        if(!whitespace)
            return throwError("No whitespace after Complete CodeBlock (Close-bracket Error)");
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(Error::Error == saveStatWithParsingControl({Stat::LeftSquareBracket}))
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
Error Interpreter::interpret<Interpreter::Stat::RightSquareBracket>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::RightSquareBracket>: ";
    bool ignoreMoveArgumentProcedure = false;

    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){

    case Stat::PendingSnprintf:
        {
            if(!lastSavedStat().isFunctionReady()){
                return throwError(ERROR_PREFIX + "Incomplete PendingSnprintf");
            }else{
                if(!whitespace)
                    return throwError(ERROR_PREFIX + "No whitespace for Complete PendingSnprintf");
            }
        }

    if(false){
    case Stat::Snprintf:
        if(!lastSavedStat().isFunctionReady()){
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "]"}))
                return throwError(ERROR_PREFIX + error());
            break;
        }
        //Q_FALLTHROUGH();
    }
    if(false){
        case Stat::PendingString:
        {
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast saved stat for Pending String");

            switch(prelastSavedStat().stat()){
            case Stat::Snprintf:
            {
                lastSavedStat().appendCAPLCommand( "]");
                return Error::NoError;
            }
                break;
            case Stat::PendingSnprintf:
            {
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
                break;
            case Stat::LeftSquareBracket:
            {
                if(isStringConstNumber(lastSavedStat().caplCommand()))
                    return throwError(ERROR_PREFIX + "Number used as procedure name");
                prelastSavedStat() = {Stat::FunctionCall}; // _PH_
                if(newProcedureCall(lastSavedStat().caplCommand()) == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                if(removeLastSavedStatWithParsingControl() == Error::Error)
                    return throwError(ERROR_PREFIX + "");
                ignoreMoveArgumentProcedure = true;
            }
                break;
            default:
                break;
            }
        }

    }
    if(!ignoreMoveArgumentProcedure){
        case Stat::StringInQuotes:
        case Stat::EndOfCodeBlock:
        case Stat::EndOfExpression:
        case Stat::EndOfList:
        case Stat::Const:
        case Stat::String:
        case Stat::Operator:
        {
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
    }
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){    // Pending Function Call -> Check variable type in unknown string
            if( finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!whitespace){
                if(!isPrelastSavedStat())
                    return throwError(ERROR_PREFIX + "No prelast stat for Function Call processing");
                if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                    if(moveArgumentToPendingSnprintf() == Error::Error or
                            finalizeSnprintfCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                }
            }
            if(moveArgumentToFunctionCall() == Error::Error
                    or finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        // In case of Snprintf Move FuctionCall as argument
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Variable processing");
        if(prelastSavedStat().stat() == Stat::Snprintf){
            if(moveArgumentToSnprintf() == Error::Error )
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::Variable:
    {
        if(!whitespace){
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable processing");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
        }
        if(moveArgumentToFunctionCall() == Error::Error
                or finalizeProcedureCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        // In case of Snprintf Move FuctionCall as argument
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Variable processing");
        if(prelastSavedStat().stat() == Stat::Snprintf){
            if(moveArgumentToSnprintf() == Error::Error )
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::SpeechMark:
    case Stat::List:
    {
        lastSavedStat().appendCAPLCommand( "]");
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
Error Interpreter::interpret<Interpreter::Stat::List>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::List>: ";    
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    case Stat::Operator:
    {
        return throwError(ERROR_PREFIX + "Not Implemented list after operator: " + lastSavedStat().caplCommand());
    }
        break;
    case Stat::StringInQuotes:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No Whitespace for Quoted String (Close-quote Error)");
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
    if(false){
        case Stat::Snprintf:
        {
            if(!lastSavedStat().isFunctionReady()){
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                    return throwError(ERROR_PREFIX + error());
                break;
            }else{
                if(!whitespace)
                    return throwError(ERROR_PREFIX + "No whitespace for Complete Snprintf (Close-quote Error)");
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
    if(false){
        case Stat::EndOfCodeBlock:
        case Stat::EndOfExpression:
        {
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace for End of Code Block");
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
    }
    if(false){
        case Stat::EndOfList:
        {
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                return throwError(ERROR_PREFIX + "No Whitespace after End of List (Close-Quotes Error)");
            }
        }
    }
    if(false){
        case Stat::String:
        case Stat::Const:
        {
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + "{"};   // _PH_
                break;
            }
        }
    }
        Q_FALLTHROUGH();
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(!whitespace){
                return throwError(ERROR_PREFIX + "No whitespace for function call");
            }
            if(Error::Error == saveStatWithParsingControl({Stat::List}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                if(Error::Error == saveStatWithParsingControl({Stat::List}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(moveArgumentToPendingSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;

    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(Error::Error == saveStatWithParsingControl({Stat::List}))
                return throwError( ERROR_PREFIX + error());
        }else{
            if(moveArgumentToPendingSnprintf() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::List:
    {
        if(whitespace){
            return throwError(ERROR_PREFIX + "Whitespace for List stat");
        }
        lastSavedStat().newListInfo();
    }
        break;

    case Stat::PendingString:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::List}))
                return throwError(ERROR_PREFIX + error());
        }else{
            lastSavedStat().appendCAPLCommand( "{");
        }
    }
        break;
    case Stat::SpeechMark:
    {
        if(whitespace)
            return throwError(ERROR_PREFIX + "Whitespace for speech mark or pending string processing");
        lastSavedStat().appendCAPLCommand( "{");
    }
        break;

    case Stat::PendingSnprintf:
    {
       if(!lastSavedStat().isFunctionReady()){
           return throwError(ERROR_PREFIX + "Incomplete PendingSnprintf stat");
       }else{
           if(!whitespace)
               return throwError(ERROR_PREFIX + "No Whitespace for complete PendingSprintf stat");
           if(moveArgumentToFunctionCall() == Error::Error or
                   Error::Error == saveStatWithParsingControl({Stat::List, ""}))
               return throwError(ERROR_PREFIX + error());
       }
    }
        break;
    case Stat::Ignore:
        break;
    case Stat::VariableAccess:
    {
        if(whitespace)
            return throwError(ERROR_PREFIX + "Whitespace for VariableAccess stat");
        if(Error::Error == saveStatWithParsingControl({Stat::List, ""}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error Interpreter::interpret<Interpreter::Stat::EndOfList>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::EndOfList>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    case Stat::FunctionCall:
    {
        if(not lastSavedStat().isFunctionReady()){
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Complete Function Call");
            switch (prelastSavedStat().stat()) {
            case Stat::Expression:
            {
                if(finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                lastSavedStat().setStat( Stat::EndOfExpression);
            }
                break;
            case Stat::CodeBlock:
            {
                if(finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                lastSavedStat().setStat( Stat::EndOfCodeBlock);
            }
                break;
            default:
                return throwError(ERROR_PREFIX + "Unknown stat for Function Call");;
            }
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                if(!isPrelastSavedStat())
                    return throwError(ERROR_PREFIX + "No prelast stat for Complete Function Call");
                switch (prelastSavedStat().stat()) {
                case Stat::Expression:
                    if(finalizeProcedureCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                    lastSavedStat().setStat( Stat::EndOfExpression);
                    break;
                case Stat::CodeBlock:
                    if(finalizeProcedureCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                    lastSavedStat().setStat( Stat::EndOfCodeBlock);
                    break;
                default:
                    return throwError(ERROR_PREFIX + "Unknown stat for Function Call");;
                }
            }else{
                if(!isPrelastSavedStat())
                    return throwError(ERROR_PREFIX + "No prelast stat for Complete Function Call");
                switch (prelastSavedStat().stat()) {
                case Stat::PendingSnprintf:
                {
                    if(moveArgumentToPendingSnprintf() == Error::Error or
                            finalizeSnprintfCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                    if(!isPrelastSavedStat())
                        return throwError(ERROR_PREFIX + "No prelast stat for Complete Function Call");
                    switch (prelastSavedStat().stat()) {
                    case Stat::FunctionCall:
                    {
                        if(moveArgumentToFunctionCall() == Error::Error or
                                finalizeProcedureCall() == Error::Error)
                            return throwError(ERROR_PREFIX + error());
                        switch(lastSavedStat().stat()){
                        case Stat::CodeBlock:
                            lastSavedStat().setStat( Stat::EndOfCodeBlock);
                            break;
                        case Stat::Expression:
                            lastSavedStat().setStat( Stat::EndOfExpression);
                            break;
                        default:
                            return throwError(ERROR_PREFIX + "Internal Error");
                        }
                    }
                         break;
                    default:
                        return throwError(ERROR_PREFIX + "Unknown stat for Function Call");;
                    }
                }
                    break;
                case Stat::FunctionCall:
                {
                    if(moveArgumentToFunctionCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                    if(!isPrelastSavedStat())
                        return throwError(ERROR_PREFIX + "No prelast stat for Complete Function Call");
                    switch (prelastSavedStat().stat()) {
                    case Stat::Expression:
                    {
                        if(finalizeProcedureCall() == Error::Error)
                            return throwError(ERROR_PREFIX + error());
                        lastSavedStat().setStat( Stat::EndOfExpression);
                    }
                        break;
                    case Stat::CodeBlock:
                    {
                        if(finalizeProcedureCall() == Error::Error)
                            return throwError(ERROR_PREFIX + error());
                        lastSavedStat().setStat( Stat::EndOfCodeBlock);
                    }
                        break;

                    default:
                        return throwError(ERROR_PREFIX + "Unknown stat for Function Call");;
                    }
                }
                    break;
                default:
                    return throwError(ERROR_PREFIX + "Unknown stat for Function Call");;
                }
            }
        }
    }
        break;
    case Stat::String:
    case Stat::Const:
    {
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for String or Const");
        switch (prelastSavedStat().stat()) {
        case Stat::Expression:
        {
            if(finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            lastSavedStat().setStat( Stat::EndOfExpression);
        }
            break;
        case Stat::CodeBlock:
        {
            if(finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            lastSavedStat().setStat( Stat::EndOfCodeBlock);
        }
            break;

        default:
            return throwError(ERROR_PREFIX + "Unknown stat for String or Const");;
        }
    }
        break;

    case Stat::Variable:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Variable");
        switch (prelastSavedStat().stat()) {
        case Stat::PendingSnprintf:
        {
            if(moveArgumentToPendingSnprintf() == Error::Error or
                    finalizeSnprintfCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Complete Function Call");
            switch (prelastSavedStat().stat()) {
            case Stat::FunctionCall:
                if(moveArgumentToFunctionCall() == Error::Error or
                        finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                switch(lastSavedStat().stat()){
                case Stat::CodeBlock:
                    lastSavedStat().setStat( Stat::EndOfCodeBlock);
                    break;
                case Stat::Expression:
                    lastSavedStat().setStat( Stat::EndOfExpression);
                    break;
                default:
                    return throwError(ERROR_PREFIX + "Internal Error");
                }
                break;

            default:
                return throwError(ERROR_PREFIX + "Unknown stat for Function Call");;
            }
        }
            break;
        case Stat::FunctionCall:
        {
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Complete Function Call");
            switch (prelastSavedStat().stat()) {
            case Stat::Expression:
            {
                if(finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                lastSavedStat().setStat( Stat::EndOfExpression);
            }
                break;
            case Stat::CodeBlock:
            {
                if(finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                lastSavedStat().setStat( Stat::EndOfCodeBlock);
            }
                break;

            default:
                return throwError(ERROR_PREFIX + "Unknown stat for Function Call");;
            }
        }
            break;

        default:
            return throwError(ERROR_PREFIX + "Unknown stat for String or Const");;
        }
    }
        break;
    case Stat::List:
    {
        if(lastSavedStat().endList() == TCLInterpreterPriv::ListStatInfo::EndListResult::EndOfList)
        {
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for List stat");
            switch(prelastSavedStat().stat()){
            case Stat::VariableAccess:
            {
                if(isStringConstNumber(lastSavedStat().caplCommand())){
                    // Const in Variable Access ???
                    return throwError(ERROR_PREFIX + "Const for VariableAccess");
                }else{  // String
                    if(lastSavedStat().caplCommand().isEmpty())
                        return throwError(ERROR_PREFIX + "Empty variable name");
                    QString str = lastSavedStat().caplCommand();
                    prelastSavedStat() = {Stat::Variable, str.replace(":", "")};    // _PH_

                    if(removeLastSavedStatWithParsingControl() == Error::Error)
                        return throwError(ERROR_PREFIX + "");
                    // Check if stat before variable is Snprintf
                    if(!isPrelastSavedStat())
                        return throwError(ERROR_PREFIX + "No prelast stat for VariableAccess processing");
                    if(prelastSavedStat().stat() == Stat::Snprintf){
                        if(moveArgumentToSnprintf() == Error::Error)
                            return throwError(ERROR_PREFIX + error());
                    }
                }
            }
                break;
            default:
                lastSavedStat().setStat( Stat::EndOfList); // _PH_
            }
        }
    }
        break;
    case Stat::EndOfList:
    case Stat::StringInQuotes:
    {
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for EndOfList or QuotedString");
        switch (prelastSavedStat().stat()) {
        case Stat::Expression:
        case Stat::CodeBlock:
            if(finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            switch(lastSavedStat().stat()){
            case Stat::CodeBlock:
                lastSavedStat().setStat( Stat::EndOfCodeBlock);
                break;
            case Stat::Expression:
                lastSavedStat().setStat( Stat::EndOfExpression);
                break;
            default:
                return throwError(ERROR_PREFIX + "Internal Error");
            }
            break;

        default:
            return throwError(ERROR_PREFIX + "Unknown stat for EndOfList or QuotedString");;
        }
    }
        break;
    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for Snprintf stat");
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "}"}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Snprintf");
            switch (prelastSavedStat().stat()) {
            case Stat::Expression:
            {
                if(finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                lastSavedStat().setStat( Stat::EndOfExpression);
            }
                break;
            case Stat::CodeBlock:
            {
                if(finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                lastSavedStat().setStat( Stat::EndOfCodeBlock);
            }
                break;

            default:
                return throwError(ERROR_PREFIX + "Unknown stat for Sprintf");;
            }
        }

    }
        break;
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for pending string");
        switch(prelastSavedStat().stat()){
        case Stat::Snprintf:
            lastSavedStat().appendCAPLCommand("}");
            break;
        case Stat::PendingSnprintf:
        {
            if(moveArgumentToPendingSnprintf() == Error::Error or
                    finalizeSnprintfCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for pending string");
            switch(prelastSavedStat().stat()){
            case Stat::FunctionCall:
            {
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                if(!isPrelastSavedStat())
                    return throwError(ERROR_PREFIX + "No prelast stat for PendingString");
                switch(prelastSavedStat().stat()){
                case Stat::Expression:
                case Stat::CodeBlock:
                {
                    if(finalizeProcedureCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                    switch(lastSavedStat().stat()){
                    case Stat::CodeBlock:
                        lastSavedStat().setStat( Stat::EndOfCodeBlock);
                        break;
                    case Stat::Expression:
                        lastSavedStat().setStat( Stat::EndOfExpression);
                        break;
                    default:
                        return throwError(ERROR_PREFIX + "Internal Error");
                    }
                    break;
                }
                    break;
                default:
                    return throwError(ERROR_PREFIX + "Unknown stat for PendingString");;
                }
            }
                break;
            default:
                return throwError(ERROR_PREFIX + "Unknown stat for PendingString");;
            }
        }
            break;
        case Stat::FunctionCall:
        {
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for PendingString");
            switch(prelastSavedStat().stat()){
            case Stat::Expression:
            case Stat::CodeBlock:
            {
                if(finalizeProcedureCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                switch(lastSavedStat().stat()){
                case Stat::CodeBlock:
                    lastSavedStat().setStat( Stat::EndOfCodeBlock);
                    break;
                case Stat::Expression:
                    lastSavedStat().setStat( Stat::EndOfExpression);
                    break;
                default:
                    return throwError(ERROR_PREFIX + "Internal Error");
                }
                break;
            }
                break;
            default:
                return throwError(ERROR_PREFIX + "Unknown stat for PendingString");;
            }
        }
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown stat for PendingString");;
        }
    }
        break;
    case Stat::SpeechMark:
    {
        lastSavedStat().appendCAPLCommand("}");
    }
        break;
    case Stat::Ignore:
        break;
    case Stat::Expression:
    {
        lastSavedStat().setStat( Stat::EndOfExpression);
    }
        break;
    case Stat::CodeBlock:
    {
        lastSavedStat().setStat( Stat::EndOfCodeBlock);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error Interpreter::interpret<Interpreter::Stat::SpeechMark>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::SpeechMark>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    /*case Stat::Variable:*/
    case Stat::String:
    case Stat::Const:
    {
        if(!whitespace){
            lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + "\""};    // _PH_
            break;
        }
    }
        Q_FALLTHROUGH();
    case Stat::Operator:
    {
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(moveArgumentToPendingSnprintf() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "\""})){
                return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace after function Call");
            if(Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
                return throwError( ERROR_PREFIX + error());
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, "\""}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;
    case Stat::EndOfList:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
                return throwError(ERROR_PREFIX + error());
        }else{
            return throwError(ERROR_PREFIX + "No Whitespace for EndOfList (Close-quote Error)");
        }

    }
        break;
    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    {
        if(!whitespace){
            return throwError(ERROR_PREFIX + "No Whitespace for Complete CodeBlock");
        }
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::SpeechMark:
    {
        if(whitespace)
            return throwError(ERROR_PREFIX + "WhiteSpace for String Processing");

        lastSavedStat().setStat(Stat::StringInQuotes);    // _PH_
    }
        break;
    case Stat::StringInQuotes:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
                return throwError(ERROR_PREFIX + error());
        }else{
            return throwError(ERROR_PREFIX + "No whitespace after Quoted String(Close-quotes Error)");
        }
    }
      break;
    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace)
                return throwError(ERROR_PREFIX + "WhiteSpace for String Processing");
            if(finalizeSnprintfCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                return throwError(ERROR_PREFIX + "No whitespace after Complete Snprintf or Quoted String(Close-quotes Error)");
            }
        }
    }
        break;
    case Stat::PendingString:
    {
       if(whitespace){
           if(moveArgumentToFunctionCall() == Error::Error or
                   Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
               return throwError(ERROR_PREFIX + error());
       }else{
           if(!isPrelastSavedStat())
               return throwError(ERROR_PREFIX + "No prestat for PendingString");
           if(prelastSavedStat().stat() == Stat::Snprintf){
               if(moveArgumentToSnprintf() == Error::Error or
                       finalizeSnprintfCall() == Error::Error)
                   return throwError(ERROR_PREFIX + error());
           }else{
               lastSavedStat().appendCAPLCommand("\"");
           }
       }
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            return throwError(ERROR_PREFIX + "Incomplete PendingSnPrintf");
        }else{
            if(!whitespace){
                return throwError(ERROR_PREFIX + "No whitespace for PendingSnprintf");
            }
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::SpeechMark}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::List:
    {
        lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());
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
Error Interpreter::interpret<Interpreter::Stat::Whitespace>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Whitespace>: ";
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    switch(lastSavedStat().stat()){
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Function call stat");
        switch(prelastSavedStat().stat()){
        case Stat::LeftSquareBracket:
        {
            if(isStringConstNumber(lastSavedStat().caplCommand()))
                return throwError(ERROR_PREFIX + "Number used as procedure name");
            prelastSavedStat() = {Stat::FunctionCall}; // _PH_
            if(newProcedureCall(lastSavedStat().caplCommand()) == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(removeLastSavedStatWithParsingControl() == Error::Error)
                return throwError(ERROR_PREFIX + "");
            if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
                return throwError(ERROR_PREFIX + error());

        }
            break;
        case Stat::Snprintf:
            lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());
            break;
        case Stat::PendingSnprintf:
        {
            if(moveArgumentToPendingSnprintf() == Error::Error or
                    finalizeSnprintfCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        default:
            if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
                return throwError(ERROR_PREFIX + error());
        }
    }

    break;

    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady())
            return throwError(ERROR_PREFIX + "Incomplete PendingSnprintf ?");
        if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){ // Incomplete
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                return throwError(ERROR_PREFIX + error());
        }else{
        if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
            return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::VariableAccess:
    case Stat::GlobalAccess:
        return throwError(ERROR_PREFIX + "Whitespace after VariableAccess or GlobalAccess Stat");
    case Stat::SpeechMark:
    case Stat::List:
    {
        if(pendingProccessingStats.isEmpty())
            lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());
        else
            lastSavedStat().appendCAPLCommand(" ");

    }
        Q_FALLTHROUGH();
    case Stat::Whitespace:
        break;
    case Stat::FunctionCall:
        if(lastSavedStat().isFunctionReady()){   // Complete only
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Function call stat");
            switch(prelastSavedStat().stat()){
            case Stat::PendingSnprintf:
            {
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            default:
                if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
                    return throwError(ERROR_PREFIX + error());
            }
            break;
        }
        if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
            return throwError(ERROR_PREFIX + error());
        break;
    case Stat::Variable:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Variable stat");
        switch(prelastSavedStat().stat()){
        case Stat::PendingSnprintf:
        {
            if(moveArgumentToPendingSnprintf() == Error::Error or
                    finalizeSnprintfCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        default:
            if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::Ignore:
        break;

    default:
        if(Error::Error == saveStatWithParsingControl({Stat::Whitespace}))
            return throwError(ERROR_PREFIX + error());
    }
    return Error::NoError;
}

template<>
Error Interpreter::interpret<Interpreter::Stat::VariableAccess>(){

    const QString ERROR_PREFIX = "TCL Interpreter <Stat::VariableAccess>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    case Stat::List:
    {
        if(whitespace){
            return throwError(ERROR_PREFIX + "List with whitespace (Check Whitespace Stat interpret function)");
        }
        lastSavedStat().appendCAPLCommand("$");
    }
        break;
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
        if(prelastSavedStat().stat() == Stat::Snprintf){
            if(moveArgumentToSnprintf() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
                return throwError(ERROR_PREFIX + error());
            break;
        }
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{

            // Change SpeechMark to Snprintf ->
            if(moveArgumentToPendingSnprintf() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        if(Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            return throwError(ERROR_PREFIX + "PendingSnprintf ?");
        }else{
            if(!whitespace){
                return throwError(ERROR_PREFIX + "Complete PendingSnprintf without whitespace");
            }
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::StringInQuotes:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
                return throwError(ERROR_PREFIX + error());
        }else{
            return throwError(ERROR_PREFIX + "No whitespace after Quoted String (Close-quote Error");
        }
    }
        break;
    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    {
        if(!whitespace){
            return throwError(ERROR_PREFIX + "No whitespace after Complete CodeBlock");
        }
        if(moveArgumentToFunctionCall() == Error::Error or
            Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::EndOfList:
    {
        if(!whitespace){
            return throwError(ERROR_PREFIX + "No whitespace after Complete List (Close-bracket Error)");
        }
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::SpeechMark:
    {
        // VariableAccess in String
        if(whitespace)
            return throwError(ERROR_PREFIX + "Whitespace for Speech Mark Processing");

        // Change SpeechMark to Snprintf ->
        if(moveArgumentToSnprintf() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::FunctionCall:
        if(!lastSavedStat().isFunctionReady()){
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace for Function Call or string or const");
        }else{
            case Stat::String:
            case Stat::Const:

                if(whitespace){
                    if(moveArgumentToFunctionCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                }else{
                    if(moveArgumentToPendingSnprintf() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                }
        }
        if(Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
            return throwError(ERROR_PREFIX + error());

        break;

    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(moveArgumentToPendingSnprintf() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        if(Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Operator:

    {
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Snprintf:
    {
        if(whitespace){
            return throwError(ERROR_PREFIX + "Whitespace for Snprintf -> Should be PendingString");
        }
        if(Error::Error == saveStatWithParsingControl({Stat::VariableAccess}))
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
Error Interpreter::interpret<Interpreter::Stat::GlobalAccess>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::GlobalAccess>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    case Stat::Operator:
    {
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(moveArgumentToPendingSnprintf() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
                return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace after Function Call");
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(moveArgumentToPendingSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
        }
        if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
            return throwError( ERROR_PREFIX + error());
    }
        break;
    case Stat::EndOfList:
    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No whitespace for EndofList or EndOfCodeBlock (Close-bracket Error)");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
        if(prelastSavedStat().stat() == Stat::Snprintf){
            lastSavedStat().appendCAPLCommand("::");
            break;
        }
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
                return throwError(ERROR_PREFIX + error());
        }else{
            lastSavedStat().appendCAPLCommand("::");
        }
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady())
            return throwError(ERROR_PREFIX + "Incomplete PendingSnprintf");
        if(!whitespace)
            return throwError(ERROR_PREFIX + "Complete PendingSnprintf without whitespace");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::StringInQuotes:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No whitespace for Quoted String (Close-quote Error)");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
            return throwError(ERROR_PREFIX + error());
    }
    case Stat::Const:
    case Stat::String:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
                return throwError(ERROR_PREFIX + error());
            break;
        }
        lastSavedStat().setStat(Stat::PendingString); // _PH_
    }
    //Q_FALLTHROUGH();
    if(false){
    case Stat::Snprintf:
        if(!lastSavedStat().isFunctionReady()){
            if(!whitespace){
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
                    return throwError(ERROR_PREFIX + error());
            }
        }else{
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
                return throwError(ERROR_PREFIX + error());
            break;
        }
    }
    case Stat::SpeechMark:
    case Stat::List:
    {
        if(whitespace){
            return throwError(ERROR_PREFIX + "List or String or Snprintf with whitespace (Check Whitespace Stat interpret function)");
        }
        lastSavedStat().appendCAPLCommand("::");
    }
        break;
    case Stat::VariableAccess:
    {
        if(whitespace){
            //savedStats.last() = {Stat::String, "$"};
            return throwError(ERROR_PREFIX + "Whitespace after VariableAccess stat (Possible but not implemented)");
        }
        if(Error::Error == saveStatWithParsingControl({Stat::GlobalAccess}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Ignore:
        break;
    case Stat::LeftSquareBracket:
    {
        if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "::"}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}

template<>
Error Interpreter::interpret<Interpreter::Stat::Semicolon>(){

    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Semicolon>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){

    case Stat::Variable:
        {
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
        }
    if(false){
        case Stat::Snprintf:
        {
            if(not lastSavedStat().isFunctionReady()){
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, ";"}))
                    return throwError(ERROR_PREFIX + error());
            }else{

            }
        }
    }
    if(false){
        case Stat::PendingString:
        {
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
            switch (prelastSavedStat().stat()) {
            case Stat::Snprintf:
                lastSavedStat().appendCAPLCommand(";");
                return Error::NoError;
            case Stat::PendingSnprintf:
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                break;
            case Stat::LeftSquareBracket:
            {
                return throwError(ERROR_PREFIX + "LeftSquareBracket before PendigString (Procedure Call Name)");
            }
                break;
            default:
                break;
            }

        }
    }
    /* MOVE TO String Processing
    if(false){
        case Stat::EndOfList:
        case Stat::EndOfCodeBlock:
        {
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace for End of List or End of Code Block (Close-bracket Error)");
        }
    }*/
    if(false){
        case  Stat::PendingSnprintf:
        {
            if(!lastSavedStat().isFunctionReady()) // Incomplete
                return throwError(ERROR_PREFIX + "Incomplete Pedning Snprintf ?");
            if(!whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for Complete PendingSnprintf");
        }
    }
    case Stat::EndOfList:
    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    case Stat::StringInQuotes:
    case Stat::String:
    {
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }

    case Stat::FunctionCall:
    {
        if(lastSavedStat().isFunctionReady()){   // Complete
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        if(finalizeProcedureCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;

    case Stat::SpeechMark:
    case Stat::List:
    {
        if(whitespace)
            return throwError(ERROR_PREFIX + "List or String with whitespace (Check Whitespace Stat interpret function)");
        lastSavedStat().appendCAPLCommand(";");
    }
        break;
    case Stat::MainCodeBlock:
    case Stat::CodeBlock:
        // Ignore
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
Error Interpreter::interpret<Interpreter::Stat::Comment>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Comment>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");
    switch(lastSavedStat().stat()){
    case Stat::String:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
                return throwError(ERROR_PREFIX + error());
        }else{
            lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + "#"};    // _PH_
        }
    }
        break;
    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
            if(moveArgumentToPendingSnprintf() == Error::Error )
                return throwError(ERROR_PREFIX + error());
        }
        if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
            return throwError( ERROR_PREFIX + error());
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace){
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                return throwError(ERROR_PREFIX + "No Whitespace for Incomplete Function call");
            }
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(moveArgumentToPendingSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }            
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    case Stat::EndOfList:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No whitespace for EndOfList or EndOfCodeBlock (Close-bracket Error)");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::CodeBlock:
    {
        QString tempStr = textInterpreter.restOfString();
        if(tempStr.endsWith("\\")){
            return throwError(ERROR_PREFIX + "Comment Extender");
        }else{
            addExpressionToCodeBlock({"//" + tempStr + "\n"});
            if(Error::Error == saveStatWithParsingControl({Stat::Comment}))
                return throwError(ERROR_PREFIX + error());
            proccessingStats.append(Stat::EndOfString);
        }
    }
        break;
    case Stat::MainCodeBlock:
    {
        QString tempStr = textInterpreter.restOfString();
        if(tempStr.endsWith("\\")){
            return throwError(ERROR_PREFIX + "Comment Extender");
        }else{
            addExpressionToMainCodeBlock({"//" + tempStr});
            if(Error::Error == saveStatWithParsingControl({Stat::Comment}))
                return throwError(ERROR_PREFIX + error());
            proccessingStats.append(Stat::EndOfString);
        }
    }
        break;
    case Stat::List:
    case Stat::SpeechMark:
    {
        if(whitespace)
            return throwError(ERROR_PREFIX + "Whitespace for List (Should be controlled in Whitespace interpret function)");
        lastSavedStat().appendCAPLCommand("#");
    }
        break;
    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for List (Should be controlled in Whitespace interpret function)");
            lastSavedStat().appendCAPLCommand("#");
        }else{
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace for Snprintf (Close-quote Error)");
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            return throwError(ERROR_PREFIX + "Incomplete PendingSnPrintf");
        }else{
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace for Complete PendingSnprintf ");
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
        switch (prelastSavedStat().stat()) {
        case Stat::Snprintf:
        case Stat::PendingSnprintf:
            lastSavedStat().appendCAPLCommand("#");
            break;
        case Stat::LeftSquareBracket:
        {
            return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
        }
            break;
        default:
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat().appendCAPLCommand("#");
            }
            break;
        }
    }
        break;
    case Stat::StringInQuotes:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No Whitespace for Snprintf (Close-quote Error)");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::PendingString, "#"}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Comment:
    {
        return throwError(ERROR_PREFIX + "Comment For Comment");
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
Error Interpreter::interpret<Interpreter::Stat::EndOfString>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::EndOfString>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    case Stat::Snprintf:
    {
        if(not lastSavedStat().isFunctionReady()){
            return throwError(ERROR_PREFIX + "End of String for Snprintf ?");
        }
    }
    if(false){
        case Stat::Variable:
        {
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
    if(false){
        case Stat::PendingString:
        {
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Pending String");
            switch (prelastSavedStat().stat()) {
            case Stat::Snprintf:
                return throwError(ERROR_PREFIX + "End of String for PendingString in Snprintf ?");
            case Stat::PendingSnprintf:
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
                break;
            case Stat::LeftSquareBracket:
            {
                return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
            }
                break;
            default:
                break;
            }
        }
    }
    if(false){
        case Stat::PendingSnprintf:
        {
            if(!lastSavedStat().isFunctionReady())
                return throwError(ERROR_PREFIX + "Incomplete PendingSnprintf");
        }
    }
    case Stat::StringInQuotes:
    case Stat::String:
    case Stat::Const:
    case Stat::Operator:
    case Stat::EndOfList:
    case Stat::EndOfCodeBlock:
    case Stat::EndOfExpression:
    {
        if(moveArgumentToFunctionCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }    

    case Stat::FunctionCall:
    {
        if(lastSavedStat().isFunctionReady()){
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
        if(finalizeProcedureCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(isSavedStatsEmpty())
            return throwError(ERROR_PREFIX + "No stats for Function Call (Finalize functionCall)");
        switch(lastSavedStat().stat()){
        case Stat::CodeBlock:
        case Stat::MainCodeBlock:        
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat for Function Call (Finalize functionCall)");
            break;
        }
    }
        break;
    case Stat::List:
        lastSavedStat().appendCAPLCommand(" ");
        break;
    case Stat::MainCodeBlock:
    case Stat::CodeBlock:
    {
        addEmptyLine();
    }
        break;
    case Stat::SpecialSign:
    {
        if(removeLastSavedStatWithParsingControl() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        addPendingProcessingStat(Stat::Whitespace);
    }
        break;
    case Stat::Ignore:
    {
        if(removeIgnore() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Comment:
    {
        if(removeLastSavedStatWithParsingControl() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
TCLInterpreter::Error TCLInterpreter::interpret<Interpreter::Stat::Operator>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Operator>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    case Stat::String:
    case Stat::Const:
    case Stat::Operator:
    {
        if(tclProceduresInterpreter.numberOfProcedureCalls() > 0 and ((tclProceduresInterpreter.lastProcedureName() == "expr_parser" or tclProceduresInterpreter.lastProcedureName() == "expr") or tclProceduresInterpreter.lastProcedureName() == "expr")){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + textInterpreter.readLastKeyword()};  // _PH_
            }
        }
    }
        break;
    case Stat::Variable:
    {
        if(tclProceduresInterpreter.numberOfProcedureCalls() > 0 and (tclProceduresInterpreter.lastProcedureName() == "expr_parser" or tclProceduresInterpreter.lastProcedureName() == "expr")){
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + textInterpreter.readLastKeyword()};  // _PH_
                break;
            }
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + textInterpreter.readLastKeyword()};  // _PH_
            }
        }
    }
        break;
    case Stat::PendingString:
    {
        if(tclProceduresInterpreter.numberOfProcedureCalls() > 0 and (tclProceduresInterpreter.lastProcedureName() == "expr_parser" or tclProceduresInterpreter.lastProcedureName() == "expr")){
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            switch(prelastSavedStat().stat()){
            case Stat::LeftSquareBracket:
            {
                return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
            }
                break;
            case Stat::PendingSnprintf:
            {
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            default:
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                    return throwError(ERROR_PREFIX + error());
            }
        }else{
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            switch(prelastSavedStat().stat()){
            case Stat::LeftSquareBracket:
            {
                return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
            }
                break;
            case Stat::PendingSnprintf:
            {
                lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());
            }
                break;
            case Stat::Snprintf:
            {
                if(whitespace)
                    return throwError(ERROR_PREFIX + "Whitespace for PendingString in Snprintf");
                lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());
            }
                break;
            default:
            {
                if(whitespace){
                    if(moveArgumentToFunctionCall() == Error::Error or
                            Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                        return throwError(ERROR_PREFIX + error());
                }else{
                    lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());
                }
            }
            }
        }

    }
        break;    
    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for Snprintf");
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace for Complete Snprintf (Close-quote Error)");
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            if(tclProceduresInterpreter.numberOfProcedureCalls() > 0 and (tclProceduresInterpreter.lastProcedureName() == "expr_parser" or tclProceduresInterpreter.lastProcedureName() == "expr")){
                if(Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                    return throwError( ERROR_PREFIX + error());
            }else{
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(tclProceduresInterpreter.numberOfProcedureCalls() > 0 and (tclProceduresInterpreter.lastProcedureName() == "expr_parser" or tclProceduresInterpreter.lastProcedureName() == "expr")){
                if(Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(whitespace){
                    if(Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                        return throwError( ERROR_PREFIX + error());
                }else{
                    return throwError(ERROR_PREFIX + "No Whitespace after function call");
                }
            }
        }else{
            if(tclProceduresInterpreter.numberOfProcedureCalls() > 1 and tclProceduresInterpreter.prelastProcedureCall().name() == "expr_parser"){
                if(!isPrelastSavedStat())
                    return throwError(ERROR_PREFIX + "No prelast stat for Function Call");
                if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                    if(moveArgumentToPendingSnprintf() == Error::Error or
                            finalizeSnprintfCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                }
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(!isPrelastSavedStat())
                    return throwError(ERROR_PREFIX + "No prelast stat for Function Call");
                if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                    if(moveArgumentToPendingSnprintf() == Error::Error or
                            Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                        return throwError(ERROR_PREFIX + error());
                    break;
                }
                if(whitespace){
                    if(moveArgumentToFunctionCall() == Error::Error or
                            Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                        return throwError(ERROR_PREFIX + error());
                }else{
                    if(moveArgumentToPendingSnprintf() == Error::Error or
                            Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
                        return throwError(ERROR_PREFIX + error());
                }
            }
        }
    }
        break;
    case Stat::SpeechMark:
    case Stat::List:
    {
        if(whitespace)
            return throwError(ERROR_PREFIX + "Whitespace for String Processing");

        lastSavedStat().appendCAPLCommand( textInterpreter.readLastKeyword());
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady())
            return throwError(ERROR_PREFIX + "Incomplete Pending Snprintf ?");
        if(tclProceduresInterpreter.numberOfProcedureCalls() > 1 and tclProceduresInterpreter.prelastProcedureCall().name() == "expr_parser"){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                return throwError(ERROR_PREFIX + error());
        }else{
           if(!whitespace)
               return throwError(ERROR_PREFIX + "No whitespace for Complete Pending Snprintf");
           if(moveArgumentToFunctionCall() == Error::Error or
                   Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
               return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::StringInQuotes:
    {
        if(tclProceduresInterpreter.numberOfProcedureCalls() > 0 and (tclProceduresInterpreter.lastProcedureName() == "expr_parser" or tclProceduresInterpreter.lastProcedureName() == "expr")){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::Operator, textInterpreter.readLastKeyword()}))
                return throwError(ERROR_PREFIX + error());
        }else{
           if(!whitespace)
               return throwError(ERROR_PREFIX + "No whitespace for Quoted String (Close-quote Error)");
           if(moveArgumentToFunctionCall() == Error::Error or
                   Error::Error == saveStatWithParsingControl({Stat::PendingString, textInterpreter.readLastKeyword()}))
               return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::Ignore:
        break;
    case Stat::GlobalAccess:
    {
        lastSavedStat() = {Stat::PendingString, "::" + textInterpreter.readLastKeyword()};
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    return Error::NoError;
}
/*
template<>
TCLInterpreter::Error TCLInterpreter::interpret<TCLInterpreter::Stat::CodeBlock>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::CodeBlock>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");
    switch(lastSavedStat().stat()){
    case Stat::String:
    case Stat::Const:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
                return throwError(ERROR_PREFIX + error());
        }else{
            lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + "{"};    // _PH_
        }
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace after incomplete Function call");
            if(Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
                return throwError( ERROR_PREFIX + error());
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;
    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(moveArgumentToPendingSnprintf() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::List:
    case Stat::SpeechMark:
        lastSavedStat().appendCAPLCommand( "{");
        break;
    case Stat::EndOfList:
    case Stat::EndOfCodeBlock:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            return throwError(ERROR_PREFIX + "No whitespace for End of List (Close-bracket Error)");
        }
    }
        break;
    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for Snprintf");
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace for Complete Snprintf (Close-quote Error)");
        }
    }
        break;
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Variable");
        switch(prelastSavedStat().stat()){
        case Stat::LeftSquareBracket:
        {
            return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
        }
            break;
        case Stat::Snprintf:
        {
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for Snprintf");
            lastSavedStat().appendCAPLCommand( "{");
        }
            break;
        case Stat::PendingSnprintf:
        {
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for PendingString in PendingSnprintf");
            lastSavedStat().appendCAPLCommand( "{");
        }
            break;
        default:
        {
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat().appendCAPLCommand( "{");
            }
        }
        }
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady())
            return throwError(ERROR_PREFIX + "Incomplete Pending Snprintf");
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No Whitespace for Complete PendingSnprintf");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::StringInQuotes:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No Whitespace for Quoted String (Close-quote Error)");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
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
TCLInterpreter::Error TCLInterpreter::interpret<TCLInterpreter::Stat::Expression>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::Expression>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");
    switch(lastSavedStat().stat()){
    case Stat::String:
    case Stat::Const:
    {
        if(!whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::Expression}))
                return throwError(ERROR_PREFIX + error());
        }else{
            //lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + "{"};    // _PH_
            return throwError(ERROR_PREFIX + "No Whitespace after String or Const");
        }
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace after incomplete Function call");
            if(Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
                return throwError( ERROR_PREFIX + error());
        }else{
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
    }
        break;
    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(moveArgumentToPendingSnprintf() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::List:
    case Stat::SpeechMark:
        lastSavedStat().appendCAPLCommand( "{");
        break;
    case Stat::EndOfList:
    case Stat::EndOfCodeBlock:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            return throwError(ERROR_PREFIX + "No whitespace for End of List (Close-bracket Error)");
        }
    }
        break;
    case Stat::Snprintf:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for Snprintf");
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace for Complete Snprintf (Close-quote Error)");
        }
    }
        break;
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Variable");
        switch(prelastSavedStat().stat()){
        case Stat::LeftSquareBracket:
        {
            return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
        }
            break;
        case Stat::Snprintf:
        {
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for Snprintf");
            lastSavedStat().appendCAPLCommand( "{");
        }
            break;
        case Stat::PendingSnprintf:
        {
            if(whitespace)
                return throwError(ERROR_PREFIX + "Whitespace for PendingString in PendingSnprintf");
            lastSavedStat().appendCAPLCommand( "{");
        }
            break;
        default:
        {
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, "{"}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat().appendCAPLCommand( "{");
            }
        }
        }
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady())
            return throwError(ERROR_PREFIX + "Incomplete Pending Snprintf");
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No Whitespace for Complete PendingSnprintf");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::StringInQuotes:
    {
        if(!whitespace)
            return throwError(ERROR_PREFIX + "No Whitespace for Quoted String (Close-quote Error)");
        if(moveArgumentToFunctionCall() == Error::Error or
                Error::Error == saveStatWithParsingControl({Stat::CodeBlock}))
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
TCLInterpreter::Error TCLInterpreter::interpret<Interpreter::Stat::EndOfCodeBlock>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::EndOfCodeBlock>: ";
    bool whitespace = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    switch(lastSavedStat().stat()){
    case Stat::PendingString:
    {
        if(!isPrelastSavedStat())
            return throwError(ERROR_PREFIX + "No prelast stat for Variable");
        switch(prelastSavedStat().stat()){
        case Stat::LeftSquareBracket:
        {
            return throwError(ERROR_PREFIX + "LeftSquareBracket before PendingString (Procedure Call Name)");
        }
            break;
        case Stat::PendingSnprintf:
        {
            if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
        }
            break;
        default:
            break;
        }
    }
    case Stat::String:
    case Stat::EndOfList:
    case Stat::EndOfCodeBlock:
    case Stat::StringInQuotes:
    {
        if(moveArgumentToFunctionCall() == Error::Error or
                finalizeProcedureCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::Variable:
    {
        if(whitespace){
            if(moveArgumentToFunctionCall() == Error::Error or
                    finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            if(moveArgumentToFunctionCall() == Error::Error or
                finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::FunctionCall:
    {
        if(!lastSavedStat().isFunctionReady()){
            if(finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }else{
            if(!isPrelastSavedStat())
                return throwError(ERROR_PREFIX + "No prelast stat for Variable");
            if(prelastSavedStat().stat() == Stat::PendingSnprintf){
                if(moveArgumentToPendingSnprintf() == Error::Error or
                        finalizeSnprintfCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            if(moveArgumentToFunctionCall() == Error::Error or
                finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::Snprintf:
    {
        if(lastSavedStat().isFunctionReady()){   // Complete
            if(moveArgumentToFunctionCall() == Error::Error or
                finalizeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    case Stat::PendingSnprintf:
    {
        if(!lastSavedStat().isFunctionReady())
            return throwError(ERROR_PREFIX + "Incomplete Pending Snprintf");
        if(moveArgumentToFunctionCall() == Error::Error or
            finalizeProcedureCall() == Error::Error)
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

    if(!isPrelastSavedStat())
        return throwError("No prelast stat for CodeBlock Control");
    if(prelastSavedStat().stat() != Stat::CodeBlock)
        return throwError(ERROR_PREFIX + "Cant end CodeBlock (wrong prestat)");
    lastSavedStat().setStat( Stat::EndOfCodeBlock);    // _PH_
    return Error::NoError;
}
*/
template<>
TCLInterpreter::Error TCLInterpreter::interpret<Interpreter::Stat::SpecialSign>(){
    const QString ERROR_PREFIX = "TCL Interpreter <Stat::SpecialSign>: ";
    QString specialSignStr;
    bool whitespace = false;
    bool whitespaceAfterSpecialSign = false;
    if(isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats");

    if((whitespace = checkWhitespace()) and isSavedStatsEmpty())
        return throwError(ERROR_PREFIX + "Empty Stats after Whitespace");

    // Check and prepare specialSignStr
    if(textInterpreter.isCurrentChar()) // Accesible -> Analysis
    {
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
        case Stat::String:
        case Stat::Const:
        {
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat() = {Stat::PendingString, lastSavedStat().caplCommand() + specialSignStr};  // _PH_
            }
        }
            break;
        case Stat::Operator:
        {
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                return throwError(ERROR_PREFIX + error());
        }
            break;
        case Stat::Variable:
        {
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(moveArgumentToPendingSnprintf() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                return throwError( ERROR_PREFIX + error());
        }
            break;
        case Stat::FunctionCall:
        {
            if(!lastSavedStat().isFunctionReady()){
                if(!whitespace)
                    return throwError(ERROR_PREFIX + "No Whitespace for Incomplete Function Call");
            }else{
                if(whitespace){
                    if(moveArgumentToFunctionCall() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                }else{
                    if(moveArgumentToPendingSnprintf() == Error::Error)
                        return throwError(ERROR_PREFIX + error());
                }
            }
            if(Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                return throwError(ERROR_PREFIX + error());
        }
            break;
        case Stat::List:
        case Stat::SpeechMark:
        {
            lastSavedStat().appendCAPLCommand(specialSignStr);
        }
            break;
        case Stat::EndOfCodeBlock:
        case Stat::EndOfExpression:
        case Stat::EndOfList:
        {
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No whitespace for EndOfExpression or EndOfList or EndOfCodeBlock (Close-bracket Error)");
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                return throwError(ERROR_PREFIX + error());
        }
            break;
        case Stat::Snprintf:
        {
            if(!lastSavedStat().isFunctionReady()){
                if(Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                if(!whitespace)
                    return throwError(ERROR_PREFIX + "No Whitespace after Snprintf (Close-quote Error)");
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
            break;
        case Stat::PendingString:
        {
            if(whitespace){
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                    return throwError(ERROR_PREFIX + error());
            }else{
                lastSavedStat().appendCAPLCommand(specialSignStr);
            }
        }
            break;
        case Stat::PendingSnprintf:
        {
            if(!lastSavedStat().isFunctionReady()){
                return throwError(ERROR_PREFIX + "Incomplete PendingSnprintf");
            }else{
                if(!whitespace)
                    return throwError(ERROR_PREFIX + "No whitespace for Complete PendingSnprintf");
                if(moveArgumentToFunctionCall() == Error::Error or
                        Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                    return throwError(ERROR_PREFIX + error());
            }
        }
            break;
        case Stat::StringInQuotes:
        {
            if(!whitespace)
                return throwError(ERROR_PREFIX + "No Whitespace for QuotedString (Close-quote Error)");
            if(moveArgumentToFunctionCall() == Error::Error or
                    Error::Error == saveStatWithParsingControl({Stat::PendingString, specialSignStr}))
                return throwError(ERROR_PREFIX + error());
        }
            break;
        case Stat::Ignore:
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat ");
        }
    }else{
        if(Error::Error == saveStatWithParsingControl({Stat::SpecialSign}))
            return throwError(ERROR_PREFIX + error());
    }

    return Error::NoError;
}

TCLInterpreterFunctions TCLInterpreter::interpretFunctions = {
    nullptr,    // None
    nullptr,    // String
    nullptr,    // Const
    &TCLInterpreter::interpret<Stat::Operator>,    // Operator
    nullptr,    // Variable
    &TCLInterpreter::interpret<Stat::UnknownString>,
    nullptr,    // FunctionCall
    &TCLInterpreter::interpret<Stat::LeftSquareBracket>,
    &TCLInterpreter::interpret<Stat::RightSquareBracket>,
    &TCLInterpreter::interpret<Stat::List>,
    &TCLInterpreter::interpret<Stat::EndOfList>,
    &TCLInterpreter::interpret<Stat::SpeechMark>,
    nullptr,    // MainCodeBlock
    &TCLInterpreter::interpret<Stat::Whitespace>,
    &TCLInterpreter::interpret<Stat::VariableAccess>,
    &TCLInterpreter::interpret<Stat::GlobalAccess>,
    &TCLInterpreter::interpret<Stat::Semicolon>,
    &TCLInterpreter::interpret<Stat::Comment>,
    &TCLInterpreter::interpret<Stat::EndOfString>,
    nullptr,// &TCLInterpreter::interpret<Stat::CodeBlock>,    // CodeBlock
    nullptr, //&TCLInterpreter::interpret<Stat::EndOfCodeBlock>,     // EndOfCodeBlock
    nullptr,    // Snprintf
    &TCLInterpreter::interpret<Stat::SpecialSign>,
    nullptr,    // PendingString
    nullptr,    // StringInQuotes
    nullptr,    // Ignore
    nullptr, //&TCLInterpreter::interpret<Stat::Expression>,

};
//static_assert (TCLInterpreter::checkInterpretFunctions() == true, "Wrong InterpretFunctions");

Error TextInterpreter::initialize(const QString &strToAnalyze){
    currentChar = savedFirstKeywordCharPos = beginOfString = savedChar = strToAnalyze.constBegin();
    endOfString = strToAnalyze.constEnd();
    lastKeywordLength = 0;
    return Error::NoError;
}

Error TextInterpreter::deinitialize(){

    return Error::NoError;
}

ReadIgnoreResult TextInterpreter::readIgnoreUntilPriv()
{
    while(currentChar < endOfString){
        if(ignoreReadUntilCondtions.contains(readCharacter(currentChar)))
            return ReadIgnoreResult::Satisfied;
        currentChar++;
    }
    return ReadIgnoreResult::EndOfString;
}

ReadIgnoreResult TextInterpreter::readIgnoreIfPriv()
{
    while(currentChar < endOfString){
        if(!ignoreReadUntilCondtions.contains(readCharacter(currentChar)))
            return ReadIgnoreResult::Satisfied;
        currentChar++;
    }

    return ReadIgnoreResult::EndOfString;
}

CheckingResult TextInterpreter::checkingPriv(){
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
                        lastKeywordLength = 0;  // KeyWord dont match, find other
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

CheckingResult TextInterpreter::checkKeyword(int& index){
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

Result TextInterpreter::interpret(){

    savedChar = savedFirstKeywordCharPos = currentChar;
    lastKeywordLength = 0;

    if(checkingPriv() == CheckingResult::StatFound ){
        return Result::StatFound;
    }

    return Result::EndOfString;
}

template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(ProcedureCall::Name name){
    using Definition = ProcedureDefinitions::Iterator;
    Definition definition;
    for(definition = procedureDefinitions.begin(); definition < procedureDefinitions.end(); definition++){
        if(definition->name == name)
            break;
    }
    try {
        if(definition == procedureDefinitions.end()){
            procedureCalls.append(ProcedureCall(name));
        }else{
            procedureCalls.append(ProcedureCall(definition));
        }
    }  catch (std::exception& e) {
        return throwError(e.what());
    }

    return Error::NoError;
}

inline void TCLInterpreter::TCLProceduresInterpreter::tryToActivateWriteOnlyProcedure(ProcedureCall::Name& name){
    if(writeOnlyProcedureActiveIndex == -1 and tclInterpreter.userConfig.proceduresSettings().isWriteOnlyProcedure(name)){
        writeOnlyProcedureActiveIndex = procedureCalls.size() + 1;
        activateWriteOnlyProcedureMode();
    }
}

template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCaseReport>(ProcedureCall::Name name){
    tryToActivateWriteOnlyProcedure(name);
    return newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::PredefinitionsOnly>(ProcedureCall::Name name){
    return newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

Error TclProcedureInterpreter::nextArgument()
{
    using Parameter = ProcedureCall::Parameter;
    switch(tclInterpreter.lastSavedStat().stat()){
    case Stat::FunctionCall:
    case Stat::PendingSnprintf:
    case Stat::Snprintf:
    {
        if(procedureCalls.size() < 2){
            return throwError("No Procedure Calls for Function-like Argument passing");
        }
        ProcedureCall procedureCall = procedureCalls.takeLast();
        Parameter parameter(tclInterpreter.takeLastSavedStat(), procedureCall);
        procedureCalls.last().nextArgument(parameter);
    }
        break;
    case Stat::EndOfCodeBlock:
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
    using Parameter = ProcedureCall::Parameter;
    switch(tclInterpreter.lastSavedStat().stat()){
    case Stat::PendingSnprintf:
    case Stat::Snprintf:
    {
        return throwError(ERROR_DESCRIPTION + "Snprintf as argument for Snprintf");
    }
    case Stat::FunctionCall:
    {
        ProcedureCall procedureCall = procedureCalls.takeLast();
        Parameter parameter(tclInterpreter.takeLastSavedStat(), procedureCall);
        tclInterpreter.saveStat({stat, QString()});
        tclInterpreter.saveStat({Stat::String, QString("concat")});
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
        tclInterpreter.saveStat({Stat::String, QString("concat")});
        if(tclInterpreter.newProcedureCall("string") == Error::Error or
                nextArgument() == Error::Error or
                onArgumentProcedureCheck() == Error::Error)
            return throwError(ERROR_DESCRIPTION + tclInterpreter.error());
        if(!parameter.caplCommand().isEmpty() and
                (procedureCalls.last().nextArgument(parameter),
                onArgumentProcedureCheck() == Error::Error))
            return throwError(ERROR_DESCRIPTION + tclInterpreter.error());
    }
    }
    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(SavedStat &statCaplCommand){
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
        ProcedureCall& procedureCall = procedureCalls.last();

        ProcedureDefinition::RulesOnEndOfCall& rulesOnEndOfCall = (procedureCall.rulesOnEndOfCall().isEmpty())? defaultUnknownProcedureDefinition.rulesOnEndOfProcedureCall : procedureCall.rulesOnEndOfCall();
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
                }
            }
        }
    }
    statCaplCommand.setCAPLCommand(command);
    finalizeOn = false;

    tclInterpreter.lastSavedStat().setFunctionReady();
    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCaseReport>(SavedStat &statCaplCommand){
    finalizeOn = false;

    tclInterpreter.lastSavedStat().setFunctionReady();
    return Error::NoError;

}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::PredefinitionsOnly>(SavedStat &statCaplCommand){

    return finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(statCaplCommand);

}

Error TCLInterpreter::finalizeProcedureCall(){
    const QString ERROR_DESCRIPTION = "TCL_Internal::finalizeProcedureCall";
    if(!isPrelastSavedStat())
        return throwError(ERROR_DESCRIPTION + " No stats");
    if(lastSavedStat().stat() != Stat::FunctionCall)
        return throwError(ERROR_DESCRIPTION + "Wrong stat to finalize procedure.");
    if(lastSavedStat().isFunctionReady())
        return throwError(ERROR_DESCRIPTION + "Function call is already complete.");
    switch(prelastSavedStat().stat()){
    case Stat::FunctionCall:
    case Stat::Snprintf:
    case Stat::PendingSnprintf:
    {
        if(tclProceduresInterpreter.finalizeProcedureCall(lastSavedStat()) == Error::Error/* or
                removeProcedureCall() == Error::Error*/)
            return throwError(ERROR_DESCRIPTION + error());
        lastSavedStat().setFunctionReady();
        if(prelastSavedStat().stat() == Stat::Snprintf){
            if(moveArgumentToSnprintf() == Error::Error)
                return throwError(ERROR_DESCRIPTION + error());
        }

    }
        break;
    case Stat::CodeBlock:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                tclProceduresInterpreter.finalizeProcedureCall(tempStat) == Error::Error or
                /*Preexpressions to Command */
                ( addPreexpressionsToCodeBlock(),
                  snprintfController().clear(),
                  addExpressionToCodeBlock({tempStat.caplCommand()}) ,
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
                tclProceduresInterpreter.finalizeProcedureCall(tempStat) == Error::Error or
                /*Preexpressions to Command */
                ( /*addPreexpressionsToCodeBlock(),
                  snprintfController().clear(),*/
                  addExpressionToCodeBlock({tempStat.caplCommand()}) ,
                 false) or
                // Continue conditional expression
                removeProcedureCall() == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    case Stat::MainCodeBlock:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                tclProceduresInterpreter.finalizeProcedureCall(tempStat) == Error::Error or
                /*Preexpressions to Command */
                (   addPreexpressionsToMainCodeBlock(),
                    snprintfController().clear(),
                    addExpressionToMainCodeBlock({tempStat.caplCommand()}) ,
                 false) or
                // Continue conditional expression
                ( tclProceduresInterpreter.lastProcedureCall().clearMemory(),
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
    if(not( lastSavedStat().stat() == Stat::Snprintf or lastSavedStat().stat() == Stat::PendingSnprintf))
        return throwError(ERROR_DESCRIPTION + "Wrong stat to finalize procedure.");
    if(lastSavedStat().isFunctionReady())
        return throwError(ERROR_DESCRIPTION + "Function call is already complete.");
    switch(prelastSavedStat().stat()){
    case Stat::Snprintf:
    case Stat::PendingSnprintf:
    {
        return throwError(ERROR_DESCRIPTION + "Snprintf should not call other Snprintf");
    }
        break;
    case Stat::FunctionCall:
    {
        if(tclProceduresInterpreter.finalizeProcedureCall(lastSavedStat()) == Error::Error/* or
                removeProcedureCall() == Error::Error*/)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    /*case Stat::MainCodeBlock:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                tclProceduresInterpreter.finalizeProcedureCall(tempStat) == Error::Error or
                // Preexpressions to Command
                (   addPreexpressionsToMainCodeBlock(),
                    addExpressionToMainCodeBlock( tempStat.caplCommand()) ,
                 false) or
                // Continue conditional expression
                removeProcedureCall() == Error::Error)
            return throwError(ERROR_DESCRIPTION + error());
    }
        break;
    case Stat::CodeBlock:
    {
        SavedStat tempStat;
        if(takeLastSavedStatWithParsingControl(tempStat) == Error::Error or
                tclProceduresInterpreter.finalizeProcedureCall(tempStat) == Error::Error or
                //Preexpressions to Command
                (   addPreexpressionsToCodeBlock(),
                    addExpressionToCodeBlock(tempStat.caplCommand()) ,
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
    ProcedureCall& procedureCall = procedureCalls.last();
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
    ProcedureCall& procedureCall = procedureCalls.last();
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
    while(!proccessingStats.isEmpty()){
        if(callInterpretFunction() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }
    }
    pendingProccessingStats = {};
    while(textInterpreter.runSearchingMode() == Result::StatFound){
        if(textInterpreter.throwErrorIfUnknownStringForForbiddenRule() == Error::Error){
            throwError("TCL Interpreter: Unknown String is Forbidden");
            if(processError() == Error::Error)
                return Error::Error;
        }
        if(processUnknownString() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }

        proccessingStats.append(textInterpreter.lastKeywordStats());

        while(!proccessingStats.isEmpty()){
            Stat savedProccessingStat = proccessingStats.last();
            if(callInterpretFunction() == Error::Error){
                if(processError() == Error::Error)
                    return Error::Error;
            }
            tclProceduresInterpreter.dynamicProcedureCheck();
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

Error Interpreter::processSavedStatsForError(){
    while(!isSavedStatsEmpty()){
        switch(lastSavedStat().stat()){
        case Stat::CodeBlock:
        case Stat::MainCodeBlock:
            saveStat({Stat::Ignore});
            return Error::NoError;
        case Stat::FunctionCall:
        case Stat::Snprintf:
        case Stat::PendingSnprintf:
            if(removeProcedureCall() == Error::Error)
                return throwError("Interpreter::processSavedStatsForError: " + error());
        default:
            if(removeLastSavedStatWithParsingControl() == Error::Error)
                return throwError("Interpreter::processSavedStatsForError: " + error() );
        }
    }
    return throwError("Interpreter::processSavedStatsForError: No stats");
}


void Interpreter::printErrorReport(QFile& reportFile, QString inputFileName){
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

void Interpreter::printErrorReport(QString& errorReport){
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

bool Interpreter::isPredefinitionMode(){
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
            str = QString("\"") + stat.caplCommand() + QString("\"");
            break;
        case Stat::Const:
            str = stat.caplCommand();
        default:
            break;
        }
    }
        break;
    case Target::OriginalProcedureParameters:
    {
        switch(stat.stat()){
        case Stat::String:
            str = QString("\"") + stat.caplCommand() + QString("\"");
            break;
        default:
            break;
        }
    }
        break;
    default:
        break;
    }*/
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
                                dest += lastProcedureCall().parameters()[index].caplCommand();
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

                            for(ProcedureCall::Parameters::Iterator responseArg = lastProcedureCall().parameters().begin() + index; responseArg < lastProcedureCall().parameters().end(); responseArg++){
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


// PROCEDURE DEFINITION ACTIONS ==================================================================================
template <>
TclProcedureInterpreter::ConditionResult
TclProcedureInterpreter::executeConditionalAction
<TclProcedureInterpreter::ProcedureDefinition::Action::Conditional::CompareNumbOfArguments>
(TclProcedureInterpreter::ConditionalActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeConditionalAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::CompareNumbOfArguments>: ";
    if(parameters.isEmpty()){
        throwError(ERROR_PREFIX + "No action parameters");
        return ConditionResult::Satisfied;
    }
    for(QStringList::ConstIterator parameter = parameters.begin(); parameter < parameters.end(); parameter++){
        bool ok;
        ProcedureCall::Parameters::size_type size = 0;
        if((size = (parameter->toInt(&ok))), !ok){
            throwError(ERROR_PREFIX + "Action Argument: " + *parameter + " cant be parsed as integer.");
            return ConditionResult::Satisfied;
        }
        if(lastProcedureCall().parametersLength() == size)
            return ConditionResult::Satisfied;
    }
    return ConditionResult::Unsatisfied;
}

template <>
TCLInterpreter::TCLProceduresInterpreter::ConditionResult
TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::IsLastSavedStat>
(ConditionalActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeConditionalAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::IsSavedStat>: ";

    if(parameters.isEmpty()){
        throwError(ERROR_PREFIX + "No action parameters");
        return ConditionResult::Satisfied;
    }
    for(QStringList::ConstIterator parameter = parameters.begin(); parameter < parameters.end(); parameter++){
        bool ok;
        Stat stat = Stat::Size;
        if((stat = static_cast<Stat>(parameter->toInt(&ok))), !ok){
            throwError(ERROR_PREFIX + "Action Argument: " + *parameter + " cant be parsed as integer.");
            return ConditionResult::Satisfied;
        }
        if(stat >= Stat::Size){
            throwError(ERROR_PREFIX + "Stat not in range. Current index: " + QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Size)) + " Current size: " + QString::number(tclInterpreter.savedStatsSize()));
            return ConditionResult::Satisfied;
        }
        if(tclInterpreter.lastSavedStat().stat() == stat)
            return ConditionResult::Satisfied;
    }
    return ConditionResult::Unsatisfied;
}

template <>
TCLInterpreter::TCLProceduresInterpreter::ConditionResult
TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::Compare>
(ConditionalActionsParameters parameters){
    const QString ERROR_PREFIX = "executeConditionalAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Compare>: ";


    QString formatStr;
    bool ok = false;
    uint numbOfCompares = UINT_MAX;
    uint numbOfFormatArgs = UINT_MAX;
    if(parameters.size() < 3){
        throwError(ERROR_PREFIX + "Number of Action Parameters dont match.");
        return ConditionResult::Satisfied;
    }
    if((numbOfCompares = parameters.at(0).toUInt(&ok), !ok) ||
            numbOfCompares + 1 >= parameters.size()  ||
            (numbOfFormatArgs = parameters.at(numbOfCompares + 1).toUInt(&ok), !ok) ||
            numbOfCompares + numbOfFormatArgs + 2 != parameters.size())
    {
        throwError(ERROR_PREFIX + "Action Parameters are wrong.");
        return ConditionResult::Satisfied;
    }
    if(!createAndAssignString(formatStr, parameters.mid(2 + numbOfCompares)))
    {
        throwError(ERROR_PREFIX + "");
        return ConditionResult::Satisfied;
    }
    for(QStringList::ConstIterator str = parameters.begin() + 1; str < parameters.begin() + 1 + numbOfCompares; str++)
        if(*str == formatStr)
            return ConditionResult::Satisfied;
    return ConditionResult::Unsatisfied;
}

const TclProcedureInterpreter::ProcedureDefinition::Rule::ConditionalActions::value_type TclProcedureInterpreter::newCompareRule(const QStringList stringsToCompare, const QStringList format){
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


template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Write>
(ExecutableActionsParameters arguments)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Write>: ";
    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, arguments)) != arguments.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }
    if(finalizeOn){ // Used for procedure
        command = str;
    }else{  // For last argument
        lastProcedureCall().lastParameter().setCaplCommand( str);
    }
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction<
TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::TclParse> (ExecutableActionsParameters arguments){
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::TclParse>: ";
    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, arguments)) != arguments.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }

    TCLInterpreter newTclInterpreter(tclInterpreter.userConfig, tclInterpreter.caplFunctionDefinitions);
    if(newTclInterpreter.toCAPL(str) == Error::Error or newTclInterpreter.anyErrors())
    {
        throwError(ERROR_PREFIX + "TclParsing Failed:" + " {Current Result: " + newTclInterpreter.error() +"}");
        return;
    }
    if(finalizeOn){ // Used for procedure
        command = newTclInterpreter.readCaplCommand().remove("\n");
    }else{  // For last argument
        lastProcedureCall().lastParameter().setCaplCommand(newTclInterpreter.readCaplCommand().remove("\n"));
    }
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Error>
(ExecutableActionsParameters arguments)
{
    const QString ERROR_PREFIX = "executeAction<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Error>: ";

    if(arguments.isEmpty())
        throwError(ERROR_PREFIX + "No arguments for Action Rule");
    else
        throwError(QString("Error Action: ") + arguments.first());

}


template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::ChangeLastSavedStat>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::ChangeLastSavedStat>: ";

    // IMPORTANT - You cant change FunctionCall, Snprintf or PendingSnprintf stat
    switch(tclInterpreter.lastSavedStat().stat()){
        case Stat::FunctionCall:
        case Stat::Snprintf:
        case Stat::PendingSnprintf:
        {
            throwError(ERROR_PREFIX + "Its illegal to change FunctionCall, Snprintf or PendingSnprintf");
            return ;
        }
    }

    if(parameters.size() != 1){
        throwError(ERROR_PREFIX + "Number of action arguments dont match. Expected: 1.");
        return ;
    }
    bool ok;
    Stat stat = Stat::Size;
    if((stat = static_cast<Stat>(parameters.first().toInt(&ok))), !ok){
        throwError(ERROR_PREFIX + "Action Argument: " + parameters.first() + " cant be parsed as integer.");
        return ;
    }
    if(stat >= Stat::Size or stat == Stat::None){
        throwError(ERROR_PREFIX + "Stat not in range. Current index: " + QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Size)) + " Current size: " + QString::number(tclInterpreter.savedStatsSize()));
        return ;
    }
    //if(lastProcedureCall().name() == "foreach")
      //  qDebug() << "Test";
    tclInterpreter.lastSavedStat().setStat(stat);
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddFunctionDefinition>
(ExecutableActionsParameters parameters)
{
    tclInterpreter.caplFunctionDefinitions.addDefinition(lastProcedureCall());
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPreExpression>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPreExpression>: ";

    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, parameters)) != parameters.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }

    tclInterpreter.preexpressions().append(str);
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddUserInteraction>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddUserInteraction>: ";

    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, parameters)) != parameters.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }
    if(!finalizeOn)
    {
        throwError(ERROR_PREFIX + "Internal Error");
        return;
    }

    if(addPreExpressionForUserInteraction() == Error::Error){
        throwError(ERROR_PREFIX + tclInterpreter.error());
        return;
    }

}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::FinalizeForEach>
(ExecutableActionsParameters parameters)
{
    /*
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::FinalizeForEach>: ";


    QString result;


    // First Argument processing
    QStringList argumentsForParam1;

    switch(procedureCalls.last().parameters().first().stat()){
    case Stat::String:
    case Stat::PendingString:
        argumentsForParam1 = {procedureCalls.last().parameters().first().caplCommand()};
        break;
    case Stat::EndOfList:
    {
        if(procedureCalls.last().parameters().first().savedStat().isAdvancedList())
        {
            throwError(ERROR_PREFIX + "Not implemented Advanced List control for Argument 1");
            return;
        }
        argumentsForParam1 = procedureCalls.last().parameters().first().savedStat().caplCommand().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    }
        break;
    default:
    {
        throwError(ERROR_PREFIX + "Not implemented control for Argument 1");
        return;
    }
    }

    // Second Argument processing
    QStringList argumentsForParam2;
    enum VariableType{
       String,
       Long
    }variableType = VariableType::String;
    QString loopIteratorEndValue;

    switch(procedureCalls.last().parameters().at(1).stat()){
    case Stat::Variable:
    {
        argumentsForParam2 += procedureCalls.last().parameters()[1].savedStat().caplCommand();
        loopIteratorEndValue = QString("elcount(") + argumentsForParam2.last() + ")";
    }
        break;
    case Stat::Snprintf:
    {
        throwError(ERROR_PREFIX + "List of Variables and function calls are forbidden for Argument 2 ");
        return;
    }
        break;

    case Stat::StringInQuotes:
    {
        variableType = VariableType::Long;
        argumentsForParam2 = procedureCalls.last().parameters()[1].savedStat().caplCommand().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for(QStringList::Iterator arg2 = argumentsForParam2.begin();
            arg2 < argumentsForParam2.end() and variableType == VariableType::Long; arg2++){
            if(arg2->endsWith(",")){
                arg2->chop(1);
                if(!tclInterpreter.isStringConstNumber(*arg2))
                {
                    arg2->append(",");
                    variableType = VariableType::String;
                }
            }else{
                if(!tclInterpreter.isStringConstNumber(*arg2))
                {
                    variableType = VariableType::String;
                }
            }
        }
    }
        break;
    case Stat::EndOfList:
    {
        if(procedureCalls.last().parameters()[1].savedStat().isAdvancedList())
        {
           ListStatInfo::ListInfos& listInfos = procedureCalls.last().parameters()[1].savedStat().advancedList()->getListInfos();
           QStringList tempArguments;
           using ListElement = ListStatInfo::ListInfos::Iterator;
           for(ListElement listElement = listInfos.begin(); listElement < listInfos.end(); listElement++)
           {
               QString tempStr = listElement->toString("");
               if(tempStr.startsWith("{") and tempStr.endsWith("}")){
                   argumentsForParam2.append(tempStr.mid(1, tempStr.size()-2));
               }else{
                   tempArguments = tempStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                   argumentsForParam2.append(tempArguments);
               }
           }
        }else{
            variableType = VariableType::Long;
            argumentsForParam2 = procedureCalls.last().parameters()[1].savedStat().caplCommand().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            for(QStringList::Iterator arg2 = argumentsForParam2.begin();
                arg2 < argumentsForParam2.end() and variableType == VariableType::Long; arg2++){
                if(arg2->endsWith(",")){
                    arg2->chop(1);
                    if(!tclInterpreter.isStringConstNumber(*arg2))
                    {
                        arg2->append(",");
                        variableType = VariableType::String;
                    }
                }else{
                    if(!tclInterpreter.isStringConstNumber(*arg2))
                    {
                        variableType = VariableType::String;
                    }
                }
            }
        }
    }
        break;
    default:
    {
        throwError(ERROR_PREFIX + "Not implemented control for Argument 2");
        return;
    }
    }
    // Write Results
    /* Result:
     {
    // _FOREACH_CONFIG_
      long _foreachIter = 0;
      Declare Arguments 1
      int _foreachList[<size of initializer>];
      // = {<List>};
      Declare Arguments 2 -> Comment Initializers
    for(_foreachIter = 0; _foreachIter < elcount(argument2); _foreachIter += numbOfArg1){
    // Loop Initialization

    // -------------
    Content

    // -------
    }

    if(loopIteratorEndValue.isEmpty()){
        if(argumentsForParam2.size() % argumentsForParam1.size() != 0){
            throwError(ERROR_PREFIX + "Wrong arguments implemntation - Arg1: " + argumentsForParam1.join(", ") + " Arg2 : " + argumentsForParam2.join(", ")) ;
            return;
        }else{
            loopIteratorEndValue = QString::number(argumentsForParam2.size() / argumentsForParam1.size());
        }
    }
    result = QString("{\n// _FOREACH_CONFIG_\nlong_foreachIter = 0;\n");
    // Arguments 1
    if(variableType == VariableType::String){
        for(QStringList::Iterator str = argumentsForParam1.begin(); str < argumentsForParam1.end(); str++){
            result += QString("char ") + *str + "[128];\n";
        }
    }else{
        for(QStringList::Iterator str = argumentsForParam1.begin(); str < argumentsForParam1.end(); str++){
            result += QString("long ") + *str + ";\n";
        }
    }
    // Arguments 2
    if(variableType == VariableType::String){
        result += QString("char _forEachList[") + QString::number(argumentsForParam2.size()) + "];\n// = {";
        for(QStringList::Iterator str = argumentsForParam1.begin(); str < argumentsForParam1.end(); str++){
            result += QString("\"") + *str + "\", ";
        }
        result.chop(QStringLiteral(", ").length());
        result += QString("};");
    }
    result +=  QString("for(_foreachIter = 0; _foreachIter < " + loopIteratorEndValue + "; _foreachIter++){");
    */

}


template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddSnprintf>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddSnprintf>: ";

    const QString snprintfVarName = tclInterpreter.snprintfController().getNextSnprintfVarName();
    ExecutableActionsParameters preexpressionParameters = {
        QStringLiteral("snprintf(") + snprintfVarName + ", elcount(" + snprintfVarName + "), \"",

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
            ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::SnprintfFormat),

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::ARGS_AFTER_INDEX) +
            "1",   // Ignore argument 0 == concat

        "\", ",
        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
            ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::CaplFormat),

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::SEPARATOR) +
            ", ",

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::ARGS_AFTER_INDEX) +
            "1",   // Ignore argument 0 == concat
        ")"
    };


    (this->*executableInterpretFunctions[
            static_cast<std::underlying_type_t<ProcedureDefinition::Action::Executable>>(
                ProcedureDefinition::Action::Executable::AddPreExpression)])(preexpressionParameters);
    QString CHAR_TYPE = "char";
    tclInterpreter.predefinitionsControl().newVariable(CHAR_TYPE, snprintfVarName, {128});

    if(tclInterpreter.isError()){
        throwError(ERROR_PREFIX + tclInterpreter.error());
        return;
    }

    command = (snprintfVarName);
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPredefinition>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPredefinition>: ";
    using ArrayRanks = PredefinitionsControl::ArrayRanks; // 1 Element == 0, no array
    using Variable = PredefinitionsControl::Variable;
    //WARNING: Nowy typ lub zmiana nazwy wymaga sprawdzenia algorytmow ponizej
    using ProcedureReturnType = Variable::Type;
    struct ProcedureReturnTypeInfo{
        ProcedureReturnType returnType = QString();
        ProcedureCall::Parameters::size_type parseFrom = 0;
    };

    ProcedureReturnType FLOAT_TYPE = "float";
    //ProcedureReturnType UINT_TYPE = "dword";
    ProcedureReturnType INT_TYPE = "long";
    ProcedureReturnType CHAR_TYPE = "char";

    using ReturnTypeForProcedure =  const QMap<const QString, ProcedureReturnTypeInfo>;
    ReturnTypeForProcedure returnTypeByProcedure = {
        {"expr", {}},
        {"expr_parser", {}}
    };

    std::decay_t<ProcedureReturnType> type;
    QString value;
    ArrayRanks arrayRanks = {0};
//TODO: Zabezpieczyc obsuge wszystkich stanow - powod: konfiguracja uzytkownika
    // If Procedure Name == "set"
    if(procedureCalls.last().name() == "set"){
        if(procedureCalls.last().parametersLength() == 2){
            // Init
            procedureCalls.last().parameters()[0].setCaplCommand(procedureCalls.last().parameters().at(0).caplCommand().replace(":", ""));
            using Mode = UserInputConfig::Settings::InterpreterMode;
            using ParameterRefs = QList<ProcedureCall::Parameters::Iterator>;
            using ParameterRef = ProcedureCall::Parameters::Iterator;
            using RawParameterRef = ProcedureCall::Parameters::ConstIterator;
            // First parameterRefs is always first argumet of set procedure
            ParameterRefs parameterRefs{procedureCalls.last().parameters().begin() + 1};
            ParameterRef lastParameterRef = nullptr;
            while(not parameterRefs.isEmpty()){
                lastParameterRef = parameterRefs.last();
                if(type.isEmpty()){ // Initializing (second original parameter of set procedure)
                    switch(lastParameterRef->stat()){
                       case Stat::Const:    // If const that means const is float, check if its uinteger
                       {
                            /*if(tclInterpreter.isStringUIntNumber(lastParameterRef->caplCommand())){ // If Uint
                                //type = UINT_TYPE;
                            }else{  // No Uint, Probably Integer*/
                                if(tclInterpreter.isStringIntNumber(lastParameterRef->caplCommand())){  // Integer
                                    type = INT_TYPE;
                                }else{ // No Integer, for sure its Float
                                    type = FLOAT_TYPE;
                                }
                            //}
                            // Stop Loop Clear ParameterRefs
                            value = lastParameterRef->caplCommand();
                            parameterRefs.clear();
                       }
                        break;

                    case Stat::Variable:
                    {
                        Variable variable;
                        if(tclInterpreter.predefinitionsControl().findVariable(variable, lastParameterRef->caplCommand()) or tclInterpreter.predefinitionsControl().findVariableGlobally(tclInterpreter.userConfig.predefinitions(), variable, lastParameterRef->caplCommand())){
                            if(tclInterpreter.userConfig.proceduresSettings().mode() != Mode::PredefinitionsOnly or not variable.type.isEmpty()){
                                type = variable.type;
                                value = lastParameterRef->caplCommand();
                                arrayRanks = variable.arrayRanks;
                            }
                        }else{
                            if(tclInterpreter.userConfig.proceduresSettings().mode() != Mode::PredefinitionsOnly){
                                type = CHAR_TYPE;
                                arrayRanks = {128};
                                value = lastParameterRef->caplCommand();
                            }
                        }
                        parameterRefs.clear();
                    }
                        break;

                    case Stat::FunctionCall:
                    {
                        type = CHAR_TYPE;
                        arrayRanks = {128};
                        value = lastParameterRef->caplCommand();
                        parameterRefs.clear();
                    }
                        break;

                    case Stat::EndOfList:
                    {
                        type = CHAR_TYPE;
                        arrayRanks = {128};
                        value = lastParameterRef->caplCommand();
                        parameterRefs.clear();
                    }
                        break;

                    case Stat::String:
                    {
                        type = CHAR_TYPE;
                        arrayRanks = {128};
                        value = lastParameterRef->caplCommand();
                        parameterRefs.clear();
                    }
                        break;
                    case Stat::SpeechMark:
                    case Stat::StringInQuotes:
                    case Stat::PendingString: //TODO: Dodajac argument w postaci Pending String jako argument procedury, sprawdz czy jest stala
                    {
                        if(tclInterpreter.isStringConstNumber(lastParameterRef->caplCommand())){ // For any new string, check if its number
                            /*if(tclInterpreter.isStringUIntNumber(lastParameterRef->caplCommand())){ // If Uint
                                //type = UINT_TYPE;
                            }else{  // No Uint, Probably Integer*/
                                if(tclInterpreter.isStringIntNumber(lastParameterRef->caplCommand())){  // Integer
                                    type = INT_TYPE;
                                }else{ // No Integer, for sure its Float
                                    type = FLOAT_TYPE;
                                }
                            //}
                            lastParameterRef->setStat(Stat::Const);
                        }else{
                            type = CHAR_TYPE;
                            arrayRanks = {128};
                        }
                        // Stop Loop Clear ParameterRefs
                        value = lastParameterRef->caplCommand();
                        parameterRefs.clear();
                    }
                        break;

                    case Stat::Snprintf:
                    case Stat::PendingSnprintf:
                    {
                        type = CHAR_TYPE;
                        arrayRanks = {128};
                        value = lastParameterRef->caplCommand();
                        parameterRefs.clear();
                    }
                        break;

                    default:
                    {
                       throwError(ERROR_PREFIX + "Not implemented stat for initialising stat");
                       return;
                    }

                    }
                }else{
                    for(RawParameterRef rawParameter = lastParameterRef->rawParameters().constBegin();
                        rawParameter < lastParameterRef->rawParameters().constEnd(); rawParameter++)
                    {
                        switch(rawParameter->stat()){
                           case Stat::Const:    // If const that means const is float, check if its uinteger
                           {
                                // For Expressions
                                if(type.front() != FLOAT_TYPE.front()){ // Type other than FLOAT_TYPE
                                    if(tclInterpreter.isStringIntNumber(rawParameter->caplCommand())){ // Int
                                        type = INT_TYPE;
                                    }else{  // Parameter is Float
                                        type = FLOAT_TYPE;
                                    }
                                }else{} // Type is already float
                           }
                            break;
                        case Stat::Variable:
                        {

                        }
                            break;

                        case Stat::FunctionCall:
                        {

                        }
                            break;

                        case Stat::EndOfList:
                        {

                        }
                            break;
                        case Stat::String:
                        {
                            type = CHAR_TYPE;
                            arrayRanks = {128};
                            value = lastParameterRef->caplCommand();
                            parameterRefs.clear();
                        }
                            break;
                        case Stat::SpeechMark:
                        case Stat::StringInQuotes:
                        case Stat::PendingString: //TODO: Dodajac argument w postaci Pending String jako argument procedury, sprawdz czy jest stala
                        {

                        }
                            break;

                        case Stat::Snprintf:
                        case Stat::PendingSnprintf:
                        {

                        }
                            break;

                        default:
                        {
                           throwError(ERROR_PREFIX + "Not implemented stat for initialized stat");
                           return;
                        }
                        }
                    }
                }
            }
        }else{
            type = CHAR_TYPE;
            arrayRanks = {128};
        }
    }else{
        type = CHAR_TYPE;
    }

    // Otherwise

    if(procedureCalls.last().parametersLength() == 0){
        throwError(ERROR_PREFIX + "Numb of arguments of procedure call is 0.");
        return;
    }
    tclInterpreter.predefinitionsControl().newVariable(type, procedureCalls.last().parameters().first().caplCommand());

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


TclProcedureInterpreter::ConditionInterpretFunctions TclProcedureInterpreter::conditionalInterpreterFunctions = {
    &TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::CompareNumbOfArguments>,
    &TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::IsLastSavedStat>,
    &TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::Compare>,
};

TclProcedureInterpreter::ExecutableInterpretFunctions TclProcedureInterpreter::executableInterpretFunctions = {
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Write>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::TclParse>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Error>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::ChangeLastSavedStat>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddFunctionDefinition>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPreExpression>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddUserInteraction>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::FinalizeForEach>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddSnprintf>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPredefinition>,
};
