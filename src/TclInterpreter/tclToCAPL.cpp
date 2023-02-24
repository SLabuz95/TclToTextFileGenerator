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
        {" ", Stat::Whitespace},
        {"\t", Stat::Whitespace},        
        {"\n", Stat::Whitespace},
        {"$", Stat::VariableSubbing},
        {"\"", Stat::DoubleQuotes},
        {";", Stat::Semicolon},
        {"\\", Stat::BackslashSubbing}

    }
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
    QStringLiteral("NotRequired"),
    QStringLiteral("Required"),
};

 const QList<QString> Rule::controlMap
{
    QStringLiteral("BreakRuleCheck"),
    QStringLiteral("NoBreakRuleCheck"),
    //{QStringLiteral("BreakRuleCheckDontExecOnEndActions"), RuleControlFlag::BreakRuleCheckDontExecOnEndActions}
};

const QList<QString> Action::conditionalMap
{
    QStringLiteral("CompareNumbOfArguments"),
    QStringLiteral("Compare"),
    QStringLiteral("CompareAndWrite"),
    QStringLiteral("CompareArgumentStat"),
};

 const QList<QString> Action::executableMap
{
    QStringLiteral("Write"),
    QStringLiteral("TclParse"),
    QStringLiteral("Error"),
    //QStringLiteral("ChangeLastArgumentStat"),
    QStringLiteral("AddPreExpression"),
    QStringLiteral("AddUserInteraction"),
    QStringLiteral("AddPredefinition"),
    QStringLiteral("WriteAttribute"),
    //{QStringLiteral("FinalizeForEach"), Action::Executable::FinalizeForEach},  // private
    //{QStringLiteral("AddSnprintf"), Action::Executable::AddSnprintf},// private
};

const QList<QString> Format::ruleMap
{
    QStringLiteral("NameOrIndex"),
    QStringLiteral("ArgumentsFrom"),
    QStringLiteral("Separator"),
    QStringLiteral("Target"),
    QStringLiteral("Attribute"),
};

 const QList< QString> Format::targetMap
{
    QStringLiteral("TclFormat"),
    QStringLiteral("Raw"),
    QStringLiteral("CaplFormat"),
    QStringLiteral("ParametersStat"),
    //{QStringLiteral("Command"), FormatTarget::Raw},
    QStringLiteral("SnprintfFormat"),
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
const TclCommand_NS::Definition::Rules defaultRulesForUnknownProcedureDefinition_onEndOfCall =
{
    // Rule 1 - 0 arguments version
    {
        { // Conditions
          {
              ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
              {
                  QString::number(0)
              }
          }
        },
        {   // Action 1: For Any Procedure write call like ProcedureName(); // Default Call
            {
                ProcedureDefinition::Action::Executable::Write,
                QStringList{    // Arguments
                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                "=",
                "();"
                }
            },
            {   // Action 2: Add to FunctionDefinitions
                ProcedureDefinition::Action::Executable::AddFunctionDefinition,
                {}
            }
        }
    },
    // Rule 1
    {
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
                ");"
                }
            },
            {   // Action 2: Add to FunctionDefinitions
                ProcedureDefinition::Action::Executable::AddFunctionDefinition,
                {}
            }
        }
    }
};


TclCommand_NS::CommandDefinitions TclProcedureInterpreter::hardcodedProcedureDefinitions =
{
    {
        "expr",
        { // On Argument

        },
        { // On Undefined Argument
          0,
          { // Dynamic Rules
              { // Rule 1: Braces -> BracesExprOnly
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                        {
                        QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))
                        }
                    },
                },
                {
                   {    // Action 1: Change stat to DoubleQuoteExprOnly
                         ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                        {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStartExprOnly))}
                   }
                }
              },
              { // Rule 2: DoubleQuotes -> DoublesQuotesExprOnly
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                        {
                        QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::DoubleQuotes))
                        }
                    },
                },
                {
                   {    // Action 1: Change stat to DoubleQuoteExprOnly
                         ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                        {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::DoubleQuotesExprOnly))}
                   }
                }
              },
              { // Rule 3: ComplexWord -> ComplexWordExprOnly
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                        {
                        QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::ComplexWord))
                        }
                    },
                },
                {
                   {    // Action 1: Change stat to DoubleQuoteExprOnly
                         ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                        {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::ComplexWordExprOnly))}
                   }
                }
              }
          },
          { // On Move
              { // Rule 1: Expr Argument Control
                {},// No Conditions
                {
                    {ProcedureDefinition::Action::Executable::ExprProcessParameter}
                }
              }
          }
        },
        { // On End of Call
          { // Rule 1: Finalize Expr
            {},// No Conditions
            {
                {ProcedureDefinition::Action::Executable::ExprFinalize},
            }
          }
        }
    },  // End of Definition ================================================,
    {   // Definition --------------------------------------------------------
        "while",
        {   // Rules for Arguments
            {
                0,
                {   // Dynamic Rules
                    {   // Rule 1: If lastSavedStat stat == List or EndOfList or FunctionCall or Whitespace -> Do nothing (Break)
                        {
                            {
                             ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                             {
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))
                             }
                            },

                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStartExprOnly))}
                           }
                        }
                    },
                    {   // Rule 1: If lastSavedStat stat == ComplexWord -> ChangeTo
                        {
                            {
                             ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                             {
                                QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::ComplexWord))
                             }
                            },

                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::ComplexWordExprOnly))}
                           }
                        }
                    },
                    {   // Rule 3: Error if no rules have been executed
                        {
                            // No Conditions
                        },
                        {
                            {   // Error
                                ProcedureDefinition::Action::Executable::Error,
                                {
                                    "\"If\" conditional expression isnt list or complexWord"
                                }
                            }
                        }
                    }
                },
                {   // Rules on moveArgument
                   { // Rule 1
                       {
                       },
                       {
                         {   // Finalize expr
                             ProcedureDefinition::Action::Executable::ExprFinalize,
                             {}
                         },
                         {   // Write as ( =-1 )
                             ProcedureDefinition::Action::Executable::Write,
                             {
                                 "( ",
                                 ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                 "=-1",
                                 " )"
                             }
                         }
                       }
                    },
                }
            },
            { // On 2nd argument (then or script)
                1,
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                           }
                        }
                    },

                },
                {   // Rules on moveArgument
                    {   // Rule 1: If Script
                        {
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                            },
                        },
                        {

                        }
                    },
                    {   // Rule 3: Error
                        {

                        },
                        {
                            {// Error
                                ProcedureDefinition::Action::Executable::Error,
                                {"While procedure incorrect 2nd argument"}
                            }
                        }
                    },
                }
            },
        },
        {   // Rules for Unspecified Argument

        },
        {   // Rules on End of Call
            {   // Rule 1

                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {
                            QString::number(2)
                        }
                    }
                },
                {
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=",
                            //ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            //ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                            //+ ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ">0"
                        }
                    }
                }

            },
            {   // Rule 2: Error
                {

                },
                {
                    {// Error
                        ProcedureDefinition::Action::Executable::Error,
                        {"While procedure requires only 2 parameters"}
                    }
                }
            },
        }
    }
};

TclCommand_NS::CommandDefinitions TclProcedureInterpreter::defaultProcedureDefinitions = {
       {   // Definition --------------------------------------------------------
           "_script_parser",
           {   // Rules for Arguments
               {
                   0,
                   { // Dynamic Rules
                     { // Rule 1
                         {   // Conditions:  If SavedStat stat == List
                             {   // Condition 1:  If SavedStat stat == List
                                 ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                             },
                         },
                         {
                             {    // Action 1: Change stat to CodeBlock
                                  ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                  {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                             }
                         }
                     }
                   }
               }
           },
           {   // Rules for Unspecified Argument

           },
           {   // Rules on End of Call
               {   // Rule 1: Error when 0 or 1 or 2 parameters
                   {
                       {
                           ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                           {
                               QString::number(1),
                           }
                       },
                       {
                           ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                       }
                   },
                   {
                       {
                           ProcedureDefinition::Action::Executable::Write,
                           {
                               ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                               "=0",
                           }
                       }
                   }
               },
               {   // Rule 1

                   {

                   },
                   {
                       {// Error
                           ProcedureDefinition::Action::Executable::Error,
                           {"Rules not satisfied: _script_parser procedure expects only 1 parameter"}
                       }
                   }

               },
           }
       },
     // End of Definition ----------------------------------------------
        {   // Definition --------------------------------------------------------
            "if",
            {   // Rules for Arguments
                {
                    0,
                    {   // Dynamic Rules
                        {   // Rule 1: If lastSavedStat stat == BracesStart -> Change to BracesStartExprOnly
                            {
                                {
                                 ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                 {
                                    QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))
                                 }
                                },

                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStartExprOnly))}
                               }
                            }
                        },
                        {   // Rule 2: If lastSavedStat stat ==  ComplexWord -> Change to ComplexWordExprOnly
                            {
                                {
                                 ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                 {
                                    QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::ComplexWord))
                                 }
                                },

                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::ComplexWordExprOnly))}
                               }
                            }
                        },
                        {   // Rule 3: If lastSavedStat stat ==  DoubleQuotes -> Change to DoubleQuotesExprOnly
                            {
                                {
                                 ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                 {
                                    QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::DoubleQuotes))
                                 }
                                },

                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::DoubleQuotesExprOnly))}
                               }
                            }
                        },
                        {   // Rule 4: Error if no rules have been executed
                            {
                                // No Conditions
                            },
                            {
                                {   // Error
                                    ProcedureDefinition::Action::Executable::Error,
                                    {
                                        "\"If\" conditional expression isnt list or complexWord"
                                    }
                                }
                            }
                        }
                    },
                    {   // Rules on moveArgument
                       { // Rule 1
                           {
                           },
                           {
                             {   // Finalize expr
                                 ProcedureDefinition::Action::Executable::ExprFinalize,
                                 {}
                             },
                             {   // Write as ( =-1 )
                                 ProcedureDefinition::Action::Executable::Write,
                                 {
                                     "( ",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "=-1",
                                     " )"
                                 }
                             }
                           }
                        },
                    }
                },
                { // On 2nd argument (then or script)
                    1,
                    {   // Dynamic Rules
                        {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                            {   // Conditions:  If SavedStat stat == List
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                                },
                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                               }
                            }
                        },

                    },
                    {   // Rules on moveArgument
                        {   // Rule 1: If Script
                            {
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                                },
                            },
                            {

                            }
                        },
                        {   // Rule 2: If "then"
                            {
                                TclProcedureInterpreter::newCompareRule(
                                {"then"},
                                {
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                    ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                        ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                    ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "=-1",
                                }
                                ),
                            },
                            {

                            }
                        },
                        {   // Rule 3: Error
                            {

                            },
                            {
                                {// Error
                                    ProcedureDefinition::Action::Executable::Error,
                                    {"If procedure incorrect 2nd argument"}
                                }
                            }
                        },
                    }
                },
            { // On 3rd argument (script or else or elseif or nothing)
                2,
                {   // Dynamic Rules
                    { // Rule 1: If argument 1 == "then"
                      {   // Conditions:  If SavedStat stat == List
                          {   // Condition 1:  If SavedStat stat == List
                              ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                              {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                          },
                      },
                      {
                         {    // Action 1: Change stat to CodeBlock
                               ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                              {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                         }
                      }
                    },
                },
                {   // Rules on moveArgument
                    {   // Rule 1: If Script and prelast parameter is then
                        {
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                            },
                            TclProcedureInterpreter::newCompareRule(
                            {"then"},
                            {
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                 "=-2",
                            }
                            ),
                        },
                        {

                        }
                    },
                    {   // Rule 2: If "else" or "elseif" and if prelastStat  is script
                        {
                            TclProcedureInterpreter::newCompareRule(
                            {"else", "elseif"},
                            {
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                 "=-1",
                            }
                            ),
                        },
                        {

                        }
                    },
                    {   // Rule 3: Error
                        {

                        },
                        {
                            {// Error
                                ProcedureDefinition::Action::Executable::Error,
                                {"If procedure incorrect 3nd argument"}
                            }
                        }
                    },
                }
                },
            },
            {   // Rules for Unspecified Argument
                0,
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                        {
                            {
                              ProcedureDefinition::Action::Conditional::CompareArgumentStat,
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
                                    "=-2"
                                }
                                ),
                            },
                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                           }
                        }
                    },
                    {   // Rule 2: On lastSavedStat == List -> Change to CodeBlock
                        {
                            {
                              ProcedureDefinition::Action::Conditional::CompareArgumentStat,
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
                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStartExprOnly))}
                           }
                        }
                    },

                },
                {   // Rules on moveArgument
                    {   // Rule 1: if lastSavedStat command == (elseif) -> P
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
                               {"expr ",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=-1",
                                ";"
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
                                "If procedure without arguments",
                            }
                        }
                    }

                },
                {   // Rule 2

                    {
                        // No conditions
                    },
                    {
                        {
                            ProcedureDefinition::Action::Executable::Write,
                            {
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=",
                                //ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                //ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                                //+ ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ">0"
                            }
                        }
                    }

                }
            }
        },
    // End of Definition ================================================,
        {   // Definition --------------------------------------------------------
            "for",
            {   // Rules for Arguments
                {
                    0,
                    {   // Dynamic Rules
                        {   // Rule 1: If lastSavedStat stat == List or EndOfList or FunctionCall or Whitespace -> Do nothing (Break)
                            {
                                {
                                 ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                 {
                                    QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))
                                 }
                                },

                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                               }
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
                                        "\"For\" initialization expression (argument 1) isnt list"
                                    }
                                }
                            }
                        }
                    },
                    {   // Rules on moveArgument
                        {   // Rule 1: If Script
                            {
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                                },
                            },
                            {

                            }
                        },
                        {   // Rule 3: Error
                            {

                            },
                            {
                                {// Error
                                    ProcedureDefinition::Action::Executable::Error,
                                    {"For procedure incorrect 1st argument"}
                                }
                            }
                        },
                    }
                },
                { // On 2nd argument (then or script)
                    1,
                    {   // Dynamic Rules
                        {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                            {   // Conditions:  If SavedStat stat == List
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                                },
                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStartExprOnly))}
                               }
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
                                        "\"For\" conditional expression (argument 2) isnt list"
                                    }
                                }
                            }
                        }
                    },
                    {   // Rules on moveArgument
                          {   // Rule 1: If Script
                              {
                                  {   // Condition 1:  If SavedStat stat == List
                                      ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                      {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStartExprOnly))}
                                  },
                              },
                              {
                                  {   // Finalize expr
                                      ProcedureDefinition::Action::Executable::ExprFinalize,
                                      {}
                                  },
                              }
                          },
                          {   // Rule 3: Error
                              {

                              },
                              {
                                  {// Error
                                      ProcedureDefinition::Action::Executable::Error,
                                      {"For procedure incorrect 2nd argument"}
                                  }
                              }
                          },/*
                            {
                            },
                            {
                              {   // Finalize expr
                                  ProcedureDefinition::Action::Executable::ExprFinalize,
                                  {}
                              },/*
                              {   // Write as ( =-1 )
                                  ProcedureDefinition::Action::Executable::Write,
                                  {
                                      "( ",
                                      ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                      "=-1",
                                      " )"
                                  }
                              }
                            }
                         }, */
                    }
                },
                { // On 3nd argument (then or script)
                    2,
                    {   // Dynamic Rules
                        {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                            {   // Conditions:  If SavedStat stat == List
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                                },
                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                               }
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
                                        "\"For\" increment expression (argument 3) isnt list"
                                    }
                                }
                            }
                        }
                    },
                    {   // Rules on moveArgument
                        {   // Rule 1: If Script
                            {
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                                },
                            },
                            {

                            }
                        },
                        {   // Rule 3: Error
                            {

                            },
                            {
                                {// Error
                                    ProcedureDefinition::Action::Executable::Error,
                                    {"For procedure incorrect 3nd argument"}
                                }
                            }
                        },
                    }
                },
                { // On 3nd argument (then or script)
                    3,
                    {   // Dynamic Rules
                        {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                            {   // Conditions:  If SavedStat stat == List
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                                },
                            },
                            {
                               {    // Action 1: Change stat to CodeBlock
                                     ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                               }
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
                                        "\"For\" increment expression (argument 4) isnt list"
                                    }
                                }
                            }
                        }
                    },
                    {   // Rules on moveArgument
                        {   // Rule 1: If Script
                            {
                                {   // Condition 1:  If SavedStat stat == List
                                    ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                    {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                                },
                            },
                            {

                            }
                        },
                        {   // Rule 3: Error
                            {

                            },
                            {
                                {// Error
                                    ProcedureDefinition::Action::Executable::Error,
                                    {"For procedure incorrect 4nd argument"}
                                }
                            }
                        },
                    }
                },
            },
            {   // Rules for Unspecified Argument

            },
            {   // Rules on End of Call
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
                            ProcedureDefinition::Action::Executable::AddUserInteraction,
                            {
                                "Prepare for control"
                            }
                        },
                        {
                            ProcedureDefinition::Action::Executable::AddPreExpression,
                            {
                                "// Initialization expression\n",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=0",
                            }
                        },
                        {
                            ProcedureDefinition::Action::Executable::AddPreExpression,
                            {
                                "// Incrementing expression\n",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=2",
                            }
                        },
                        {
                            ProcedureDefinition::Action::Executable::Write,
                            {
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=",
                                "(/* Initialization */ ; ",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=1",
                                " ; /* Incrementation */ )",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=3"
                                //ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                //ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                                //+ ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            }
                        }
                    }

                },
                {   // Rule 2: Error
                    {

                    },
                    {
                        {// Error
                            ProcedureDefinition::Action::Executable::Error,
                            {"For procedure requires only 4 parameters"}
                        }
                    }
                },
            }
        },  // End of Definition ================================================,
       {   // Definition --------------------------------------------------------
           "foreach",
           {   // Rules for Arguments

           },
           {   // Rules for Unspecified Argument

           },
           {   // Rules on End of Call
               {   // Rule 0.5: Error when 0 or 1 or 2 parameters
                   {
                       {
                           ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                           {
                               QString::number(0),
                               QString::number(1),
                               QString::number(2),
                           }
                       }
                   },
                   {
                       {// Error
                           ProcedureDefinition::Action::Executable::Error,
                           {"Foreach procedure requires at least 3 parameters"}
                       }
                   }
               },
               {   // Rule 1

                   {
                       {
                           ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                            {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                       }
                   },
                   {
                       {
                           ProcedureDefinition::Action::Executable::AddUserInteraction,
                           {
                               "Prepare foreach procedure control"
                           }
                       },
                       {
                           ProcedureDefinition::Action::Executable::AddPreExpression,
                           {
                               "// Details about foreach procedure behaviour: https://www.tcl.tk/man/tcl/TclCmd/foreach.html\n",
                               "// First Comment \"LIST\": Variables List -> Second Command \"LIST\": Values List , etc \n",
                               "// LIST\n/*",
                               ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                               ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                               + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                               ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                               "@*/\n// LIST\n/*",
                               ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                               "><0;-2",
                               "*/"
                           }
                       },
                       {
                           ProcedureDefinition::Action::Executable::AddPreExpression,
                           {
                               "// Procedure template\n",
                               "// foreach ()",
                           }
                       },
                       {
                           ProcedureDefinition::Action::Executable::TclParse,
                           {
                               ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                               ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET)
                               + ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                               "_script_parser ",
                               ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                               "=-1",
                               ";"
                           }
                       }
                   }

               },
               {   // Rule 2: Error when -1 parameter isnt BracesStart
                   {

                   },
                   {
                       {// Error
                           ProcedureDefinition::Action::Executable::Error,
                           {"Rules not satisfied: Expected last parameter as BracesStart (script like parameter)"}
                       }
                   }
               },

           }
       },  // End of Definition ================================================,
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
                            ">1",
                            ";"
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
            {   // Rule 2 - Call procedure without arguments
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
                            "();"
                        }
                    }
                }
            },
            {   // Rule 3 - Call procedure with some arguments
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
                            ");"
                        }
                    }
                }
            }
        }
    },
    // End of Definition ================================================,
    {
        "open",
        {   // Dynamic Rules
        },
        {   // On move
        },
        {   // On Finalize

                        {   // Rule 3:
                            {
                                {
                                    ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                                    {QString::number(0)}
                                },
                            }, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"open procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {"open();"}
                                },
                            }
                        },
                        {   // Rule 3:
                            {}, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"open procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "=",
                                     "(",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "@, ",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     ">0",
                                     ");"
                                    }
                                },
                            }
            }
        }

    },
    // End of Definition ================================================,
    {
        "close",
        {   // Dynamic Rules
        },
        {   // On move
        },
        {   // On Finalize

                        {   // Rule 3:
                            {
                                {
                                    ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                                    {QString::number(0)}
                                },
                            }, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"close procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {"close();"}
                                },
                            }
                        },
                        {   // Rule 3:
                            {}, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"close procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "=",
                                     "(",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "@, ",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     ">0",
                                     ");"
                                    }
                                },
                            }
            }
        }

    },
    // End of Definition ================================================,
    {
        "eval",
        {   // Dynamic Rules
        },
        {   // On move
        },
        {   // On Finalize

                        {   // Rule 3:
                            {
                                {
                                    ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                                    {QString::number(0)}
                                },
                            }, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"eval procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {"eval();"}
                                },
                            }
                        },
                        {   // Rule 3:
                            {}, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"eval procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "=",
                                     "(",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "@, ",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     ">0",
                                     ");"
                                    }
                                },
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
                            ");"
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
                            ");"
                        }
                    }
                }
            },
            {   // Rule 3:
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(0)}
                    },
                }, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {"file procedure requires manual modification"}
                    },
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {"file();"}
                    },
                }
            },
            {   // Rule 3:
                {}, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {"file procedure requires manual modification"}
                    },
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         "=",
                         "(",
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         "@, ",
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         ">0",
                         ");"
                        }
                    },
                }
            }
        }

    },
    // End of Definition ================================================,
    {
        "incr",
        {   // On arguments
            {
                0,
                { // Dynamic

                },
                { // On move
                  { // Rule 1
                    {},
                    {
                        { // Action 1
                            ProcedureDefinition::Action::Executable::TclParse,
                            {
                                "________incr_parser_____ $",
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                    ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::TclFormat),
                                ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                "=0",
                              ";"
                            }
                        }
                    }
                  }
                }
            }
        },
        {   // On Unspecified argument

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
                            ";"
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
                            "++;"
                        }
                    }
                }
            },
            {   // Rule 3:
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(0)}
                    },
                }, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {"incr procedure requires manual modification"}
                    },
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {"incr();"}
                    },
                }
            },
            {   // Rule 3:
                {}, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {"incr procedure requires manual modification"}
                    },
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         "=",
                         "(",
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         "@, ",
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         ">0",
                         ");"
                        }
                    },
                },
            }
        }

    },
    // End of Definition ================================================,
    {
        "________incr_parser_____",
        {   // On arguments

        },
        {   // On Unspecified argument

        },
        {   // On Finalize
            {   // Rule 1:
                {   // Condtions
                },
                {   // Actions
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                        }
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
                            ");"
                        }
                    }
                }
            },
                        {   // Rule 3:
                            {
                                {
                                    ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                                    {QString::number(0)}
                                },
                            }, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"llength procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {"llength();"}
                                },
                            }
                        },
                        {   // Rule 3:
                            {}, // No Conditions
                            {
                                {
                                    ProcedureDefinition::Action::Executable::AddUserInteraction,
                                    {"llength procedure requires manual modification"}
                                },
                                {
                                    ProcedureDefinition::Action::Executable::Write,
                                    {
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "=",
                                     "(",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     "@, ",
                                     ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                                     ">0",
                                     ");"
                                    }
                                },
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
                            ");"
                        }
                    }
                }
            },
            {   // Rule 3:
                {
                    {
                        ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                        {QString::number(0)}
                    },
                }, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {"puts procedure requires manual modification"}
                    },
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {"puts();"}
                    },
                }
            },
            {   // Rule 3:
                {}, // No Conditions
                {
                    {
                        ProcedureDefinition::Action::Executable::AddUserInteraction,
                        {"puts procedure requires manual modification"}
                    },
                    {
                        ProcedureDefinition::Action::Executable::Write,
                        {
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         "=",
                         "(",
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         "@, ",
                         ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                         ">0",
                         ");"
                        }
                    },
                }
            }
        }

    },
    // End of Definition ================================================,
    {
        "stc_section",
        {   // Rules for Arguments
            {
                0,
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
            { // Rule 0 - not supported
              {
                  {
                      ProcedureDefinition::Action::Conditional::CompareNumbOfArguments,
                      {QString::number(0), QString::number(1)}
                  },
              }, // No Conditions
              {
                  {
                      ProcedureDefinition::Action::Executable::Error,
                      {"stc_section with 0 or 1 arguments is not supported"}
                  },
              }
            },
            { // Rule 1 - Result
              {
                  TclProcedureInterpreter::newCompareRule(
                  {"RESULT"},
                  {
                      ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                      ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                          ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                      "",
                      "=0"
                  }
                  ),
              }, // No Conditions
              {
                  {
                      ProcedureDefinition::Action::Executable::Write,
                      {
                          "EXPECTED(",
                          ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                          "@, ",
                          ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                          ">1",
                          ");"
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
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::Raw),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "=0",
                            "(",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
                                ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::CaplFormat),
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            "@, ",
                            ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                            ">1",
                            ");"
                        }
                    }
                }
            }
        }
    },
    // End of Definition ================================================,

    /*{
        "for",
        {   // Rules for Arguments
            {
                0,
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to Expression
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
//                            Commented but required
//                           {    // Action 1: Change stat to Expression
//                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
//                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Expression))}
//                           }
                        }
                    },
                    {   // Rule 2: On Ignore
                        {   // Conditions: Ignore
                            {   // Condition 1:  Ignore
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
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
            {
                1,
                {   // Dynamic Rules
                    {   // Rule 1: If lastSavedStat stat == List or EndOfList or FunctionCall or Whitespace -> Do nothing (Break)
                        {
                            {
                             ProcedureDefinition::Action::Conditional::CompareArgumentStat,
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
            {
                2,
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to Expression
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
//                            Commented but required
//                           {    // Action 1: Change stat to Exxpression
//                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
//                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Expression))}
//                           }
                        }
                    },
                    {   // Rule 2: On Ignore
                        {   // Conditions:  Ignore
                            {   // Condition 1:  Ignore
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
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
            {
                3,
                {   // Dynamic Rules
                    {   // Rule 1: On lastSavedStat == List -> Change to CodeBlock
                        {   // Conditions:  If SavedStat stat == List
                            {   // Condition 1:  If SavedStat stat == List
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                            },
                        },
                        {
                           {    // Action 1: Change stat to CodeBlock
                                 ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                           }
                        }
                    },
                    {   // Rule 2: On Ignore -> Ignore
                        {   // Conditions:  Ignore
                            {   // Condition 1:  Ignore
                                ProcedureDefinition::Action::Conditional::CompareArgumentStat,
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
    /*
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
    },*/
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
                                 ProcedureDefinition::Action::Conditional::CompareArgumentStat,
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::BracesStart))}
                             },
                         },
                         {
                            {    // Action 1: Change stat to CodeBlock
                                  ProcedureDefinition::Action::Executable::ChangeLastArgumentStat,
                                 {QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Script))}
                            }
                         }
                     },
                     {   // Rule 2: On Ignore
                         {   // Conditions:  Ignore
                             {   // Condition 1:  Ignore
                                 ProcedureDefinition::Action::Conditional::CompareArgumentStat,
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
       // Rules on End
    defaultRulesForUnknownProcedureDefinition_onEndOfCall

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
    case Target::TclFormat:
       return rawCommand();
    case Target::Stat:
    {
        return QString::number(std::underlying_type_t<Stat>(stat()));
    }
    case Target::CaplFormat:
    {
        return outputCommand();
    }
    case Target::Raw:
    {
        switch(stat()){
        case Stat::Word:
        {
            if(rawCommand().startsWith("\""))
                return rawCommand().sliced(1, rawCommand().size() - 2);
        }
            break;
        case Stat::BracesStart:
        case Stat::BracesStartExprOnly:
        case Stat::DoubleQuotes:
        case Stat::DoubleQuotesExprOnly:
        case Stat::Script:
        {
            if(rawCommand().size() >= 2)
                return rawCommand().sliced(1, rawCommand().size() - 2);
            else{
                return rawCommand();
            }
        }
        case Stat::CommandSubbing:
        {
            if(rawCommand().startsWith("["))
                return rawCommand().sliced(1, rawCommand().size() - 2);
            break;
        }
        case Stat::VariableSubbing:
            if(rawCommand().size() > 2 and rawCommand().at(1) == '{') // ${}
                return rawCommand().sliced(2, rawCommand().size() - 3);
            else
                return outputCommand();
        default:
            break;
        }
        return rawCommand();
    }
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
                   // Get numb of Indexes -> Iterate through arguments of definition and check if no
                   using RulesForArgument = ProcedureDefinition::RulesForArguments::Iterator;
                   if(definition->rulesForArguments.size() == 0){
                       definition->rulesForArguments = defaultDefinition->rulesForArguments;
                   }else{
                       for(RulesForArgument defaultRulesForArgument = defaultDefinition->rulesForArguments.begin();
                           defaultRulesForArgument < defaultDefinition->rulesForArguments.end(); defaultRulesForArgument++)
                       {
                           RulesForArgument rulesForArgument;
                           for(rulesForArgument = definition->rulesForArguments.begin();
                               rulesForArgument < definition->rulesForArguments.end(); rulesForArgument++)
                           {
                                if(defaultRulesForArgument->index == rulesForArgument->index){ // Same Index
                                    rulesForArgument->rules.append(defaultRulesForArgument->rules);
                                    rulesForArgument->rulesOnMoveArgument.append(defaultRulesForArgument->rulesOnMoveArgument);
                                    break;
                                }else{ // Stop condition
                                    if(rulesForArgument->index > defaultRulesForArgument->index){
                                        rulesForArgument = definition->rulesForArguments.insert(defaultRulesForArgument->index, *defaultRulesForArgument);
                                        break;
                                    }else{ // Ignore
                                    }
                                }
                           }
                           if(rulesForArgument == definition->rulesForArguments.end()){ // Not Found - Append cause no higher index has been found
                               definition->rulesForArguments.append(*defaultRulesForArgument);
                           }
                       }
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
                    lastKeyword = Keywords::ConstIterator(&endOfStringKeyword);
                    //return CheckingResult::EndOfString;
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
        lastKeyword = Keywords::ConstIterator(&endOfStringKeyword);
        //return CheckingResult::EndOfString;
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
    if(writeOnlyProcedureActiveIndex.last() == -1 and userConfig.proceduresSettings().isWriteOnlyProcedure(name)){
        writeOnlyProcedureActiveIndex.last() = procedureCalls.size();
        //activateWriteOnlyProcedureMode();
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


Error TCLInterpreter::deinitialize(){
    QString tclCommand = QString();
    if(isError())
        return Error::Error;
    textInterpreter().initialize(tclCommand);
    setProcessingStat(Stat::None);

    while(processingStat() != Stat::EndOfString)
        if(callCurrentInterpreterProcedure() == Error::Error)
            return Error::Error;

    setProcessingStat(Stat::EndOfString);

    if(callInterpretFunction() == Error::Error){
        if(processError() == Error::Error)
            return Error::Error;
    }



    return Error::NoError;
}

Error TCLInterpreter::interpreterProcedure_standard(){
    while(textInterpreter().runSearchingMode() == Result::StatFound){

        if(processUnknownString() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }

        setProcessingStat(textInterpreter().lastKeywordStat());
        if(processingStat() == Stat::EndOfString){
            textInterpreter().deinitialize();
            return Error::NoError;
        }

        if(callInterpretFunction() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }

        if(processingStat() == Stat::EndOfString){
            textInterpreter().deinitialize();
            return Error::NoError;
        }
        /*
        if(commandsController.performDynamicRulesCheck() == Error::Error){
            if(processError() == Error::Error)
                return Error::Error;
        }
        */

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
        /*
        commandsController.performDynamicRulesCheck();
        */
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
        tempStr.append("\n");
    }else{
        if(tempStr.endsWith("\\\n")){
            tempStr.chop(2);
            tempStr.append("\n");
        }else{
            setStandardInterpreterMode();
        }
    }
    if(commandsController.isMainScript())
        addExpressionToMainCodeBlock({"//" + tempStr});
    else
        commandsController.addNewParameter({"//" + tempStr});

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
        // - "<<number>", ex. ">2" - take in reverse all string from lastActionResponse in index range: 2 ...
        // - "><<number>;<number>", ex. "><2;-1" - take all string from lastActionResponse in index range: 2 ... -1 (last)
        // - "<><number>;<number>", ex. "><2;-1" - take in reverse all string from lastActionResponse in index range: 2 ... -1 (last)
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
                    case Rule::ATTRIBUTE:
                    {
                        if(arg->isEmpty())
                            return false;
                        ControllerConfigInfo::Attributes::ConstIterator attribute = userConfig.attributesController().getAttribute(*arg);
                        if(not userConfig.attributesController().isAttributeValid(attribute))
                            return false;
                        dest += attribute.value().value;
                    }
                        break;
                    case Rule::INDEX_OR_FULL_LINE:
                    {
                        if(arg->isEmpty()){
                            // FULL LINE ----------------
                            switch(target){
                            case Target::TclFormat:                                
                                dest += lastProcedureCall().rawParameters().at(0).rawCommand();
                                break;
                            case Target::CaplFormat:
                                dest += lastProcedureCall()._name();
                                break;
                            case Target::Stat:
                            case Target::Raw:
                                dest += lastProcedureCall().rawParameters().front().toString(target);
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
                            case Target::Stat:
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
                        switch(formatRule = static_cast<Rule>(arg->at(0).toLatin1())){
                        case Rule::ARGS_AFTER_INDEX_REVERSE:
                        {
                            // ALL ARGUMENTS IN RANGE  -------------------------------
                            *arg = arg->mid(1);
                            QStringList&& splitted = arg->split(";", Qt::SkipEmptyParts);
                            int indexLowRange = INT_MAX;
                            int indexHighRange = INT_MAX;
                            if(splitted.size() != 2 or
                                    (static_cast<void>(indexLowRange = splitted.at(0).toInt(&ok)), not ok) or
                                    (static_cast<void>(indexHighRange = splitted.at(1).toInt(&ok)), not ok)){
                                return (arg - args.begin());
                            }
                            // Check range
                            // Low Range
                            if(indexLowRange < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                indexLowRange = lastProcedureCall().parametersLength() + indexLowRange; // Index is negative
                            }
                            if(indexLowRange < 0 or indexLowRange >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            indexLowRange++; // Ignore procedure name
                            // High Range
                            if(indexHighRange < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                indexHighRange = lastProcedureCall().parametersLength() + indexHighRange; // Index is negative
                            }
                            if(indexHighRange < 0 or indexHighRange >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            indexHighRange++; // Ignore procedure name
                            if(indexHighRange < 0 or indexHighRange >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            // Preverification
                            if(indexLowRange > indexHighRange){
                                return (arg - args.begin());
                            }
                            // Get Range
                            indexHighRange++; // High index is inclusive
                            switch(target){
                            case Target::CaplFormat:
                            case Target::TclFormat:
                            case Target::Raw:
                            case Target::Stat:
                            {
                                for(Call::Parameters::Iterator responseArg = lastProcedureCall().parameters().begin() + indexLowRange;
                                    responseArg != lastProcedureCall().parameters().begin() + indexHighRange and
                                    responseArg != lastProcedureCall().parameters().end() ; responseArg++)
                                {
                                    dest += responseArg->toString(target) + seperator;
                                    separatorUsed = true;
                                }
                            }
                                break;
                            default:
                                return (arg - args.begin());
                            }
                            // END ALL ARGUMENTS IN RANGE  -------------------------------
                        }
                            break;
                        default:
                        {
                            // ALL ARGUMENTS AFTER INDEX -------------------------------
                            if((static_cast<void>(index = arg->toInt(&ok)), !ok))
                                return (arg - args.begin());
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = lastProcedureCall().parametersLength() + index; // Index is negative
                            }
                            if(index < 0 or index >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            index++; // Ignore procedure name
                            switch(target){
                            case Target::CaplFormat:
                            case Target::TclFormat:
                            case Target::Raw:
                            case Target::Stat:
                            {
                                for(Call::Parameters::Iterator responseArg = lastProcedureCall().parameters().begin() + index; responseArg != lastProcedureCall().parameters().end(); responseArg++){
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
                        }
                    }
                        break;
                    case Rule::ARGS_AFTER_INDEX_REVERSE:
                    {
                        switch(formatRule = static_cast<Rule>(arg->at(0).toLatin1())){
                        case Rule::ARGS_AFTER_INDEX:
                        {
                            // ALL ARGUMENTS IN RANGE  -------------------------------
                            *arg = arg->mid(1);
                            QStringList&& splitted = arg->split(";", Qt::SkipEmptyParts);
                            int indexLowRange = INT_MAX;
                            int indexHighRange = INT_MAX;
                            if(splitted.size() != 2 or
                                    (static_cast<void>(indexLowRange = splitted.at(1).toInt(&ok)), not ok) or
                                    (static_cast<void>(indexHighRange = splitted.at(0).toInt(&ok)), not ok))
                            {
                                return (arg - args.begin());
                            }
                            // Check range
                            // Low Range
                            if(indexLowRange < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                indexLowRange = lastProcedureCall().parametersLength() + indexLowRange; // Index is negative
                            }
                            if(indexLowRange < 0 or indexLowRange >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            indexLowRange++; // Ignore procedure name
                            // High Range
                            if(indexHighRange < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                indexHighRange = lastProcedureCall().parametersLength() + indexHighRange; // Index is negative
                            }
                            if(indexHighRange < 0 or indexHighRange >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            indexHighRange++; // Ignore procedure name
                            // Preverification
                            if(indexLowRange < indexHighRange){
                                return (arg - args.begin());
                            }
                            indexLowRange--; // Low index is inclusive
                            // Get Range
                            switch(target){
                            case Target::CaplFormat:
                            case Target::TclFormat:
                            case Target::Raw:
                            case Target::Stat:
                            {
                                for(Call::Parameters::Iterator responseArg = lastProcedureCall().parameters().begin() + indexHighRange;
                                    responseArg != lastProcedureCall().parameters().begin() + indexLowRange and
                                    responseArg != lastProcedureCall().parameters().begin() - 1; responseArg--)
                                {
                                    dest += responseArg->toString(target) + seperator;
                                    separatorUsed = true;
                                }
                            }
                                break;
                            default:
                                return (arg - args.begin());
                            }
                            // END ALL ARGUMENTS IN RANGE  -------------------------------
                        }
                            break;
                        default:
                        {
                            // ALL ARGUMENTS AFTER INDEX -------------------------------
                            if((static_cast<void>(index = arg->toInt(&ok)), !ok))
                                return (arg - args.begin());
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = lastProcedureCall().parametersLength() + index; // Index is negative
                            }
                            if(index < 0 or index >= lastProcedureCall().parametersLength())
                                return (arg - args.begin());
                            index++; // Ignore procedure name
                            switch(target){
                            case Target::CaplFormat:
                            case Target::TclFormat:
                            case Target::Raw:
                            case Target::Stat:
                            {
                                for(Call::Parameters::Iterator responseArg = lastProcedureCall().parameters().begin() + index;
                                    responseArg != lastProcedureCall().parameters().begin() - 1; responseArg--)
                                {
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
                        }
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


