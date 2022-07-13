#ifndef COMMANDDEF_HPP
#define COMMANDDEF_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include"TclInterpreter/UserInteractionCont/userInteractionCont.hpp"
#include<QList>

namespace Tcl::Interpreter::Command{
    using namespace Tcl::Interpreter::Core;
    using namespace Tcl::Interpreter;
    class Definition{
        // Concept ----------------------------------------------------------------------
        class Action;
        // ----
        enum class ModifierRuleControl : uint;
        enum class ParsingMode;
        // ----
        class Rule;
        class CommandDefinitions;
        struct RulesForArgument;   // Contains Rules for argument Index + Actions On Rules
        // ----
        // End of Concept |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------------------------------
    public:
        using Action = Action;
        using Rule = Rule;
        using Rules = QVector<Rule>;

        using RulesForArgument = RulesForArgument;
        using RulesForArguments = QVector<RulesForArgument>;


    private:
        class Action{
        public:
            // Concept ----------------------------------------------------------------------
            enum class Conditional : int;
            enum class Executable : int;

            // End of Concept |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            // Concept Definition ------------------------------------------------------------------
            enum class Conditional : int{
                CompareNumbOfArguments, // Current numb of Arguments for last procedure call (current definition)
                FCT_Begin = CompareNumbOfArguments,
                Compare,
                FCT_End,// VVVVVVVV Privates factory products not required
                IsLastSavedStat = FCT_End,    // Check Stat for specified SavedStat (Arguments pattern: 1- savedStat number)

                PrivateSize,
                PublicSize = FCT_End,
                End = PrivateSize,
                None = -1,
            };

            enum class Executable : int{
                 Write,
                FCT_Begin = Write,
                TclParse,
                Error,
                AddPreExpression,
                AddUserInteraction,
                CompareAndWrite, // QuickRule
                FCT_End,// VVVVVVVV Privates factory products not required
                FinalizeForEach = FCT_End,
                ChangeLastSavedStat,
                AddFunctionDefinition,
                //AddSnprintf,
                AddPredefinition,

                PrivateSize,
                PublicSize = FCT_End,
                End = PrivateSize,
                None = -1,
            };

        private:

            static const QList<QString> conditionalMap;
            static const QList<QString> executableMap;
            // End of Concept Definition
        public:
            inline static Conditional fromStr_conditional(QString& str){return static_cast<Conditional>(conditionalMap.indexOf(str));}
            inline static Executable fromStr_executable(QString& str){return static_cast<Executable>(executableMap.indexOf(str));}
            inline static const decltype(conditionalMap)& conditionalsNames(){return conditionalMap;}
            inline static const decltype(executableMap)& executablesNames(){return executableMap;}
        };
    /*
        enum class ConditionalAction : uint{
            IfSavedStat,    // List of Saved Stats to Compare With Argument

            STARTS_WITH,        // "<any string>" -  just write,  "" - next argument take arguments in pattern
            //ENDS_WITH,  // "<any string>" -  just write,  "" - next argument take arguments in pattern
            WRITE_TO_TC_INFO,   // (1): (number of stat)  (2 ...): "<any string>" -  just write,  "" - next argument take arguments in pattern
            WRITE,  // "<any string>" -  just write,  "" - next argument take arguments in pattern
            SPLIT,  // (1): (Pattern)
            COMMENT_OUT,
            CHANGE_PHASE,
            //SSTR_SAVE,   // Arguments like write
            //SSTR_SLICE,
            REPLACE_ALL,  // Works only for lineData (raw data) Arguments:(1)-before, (2)-after
            DEBUG,
            COMPARE,
            //COMPARE_REGEX,
            //FORMAT,
            //CONTAINS,
            //REMOVE_SPLIT_RESULT_IF_ENDS_WITH,
            //GET_MSG_BY_SIG_NAME,
            //ADD_VARIABLE,
            INTERPRET

        };
        // ----
        */
    public:
        class Format{
        public:
            // Concept ----------------------------------------------------------------------
            enum class Rule : char;
            enum class Target : qint8;

            // End Of Concept ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            // Concept Definition -------------------------------------------------------------
            enum class Rule : char{
               INDEX_OR_FULL_LINE = '=',
               ARGS_AFTER_INDEX = '>',
               //ARG_IN_RANGE_P1 = '<',
               //ARG_IN_RANGE_P2 = '>',
               //CUT_INDEX = '/',  // / - arguments (1: index to cut, 2: numb of signs to cut)
               //CUT_AFTER_INDEX = 'C',
               SEPARATOR = '@',
               TARGET = 'T',
                None = -1,
               //SPLIT = 'S',
               //SLICED = '\\'
            };
            enum class Target : qint8{
                Raw,    // Orginal Interpreter Read
                TclFormat,
                CaplFormat,
                ProcedureParametersStat,    // Number
                //Command,
                SnprintfFormat,
                Size,
                None = -1,
                //ProcedureParameters,
                //OriginalProcedureParameters,
            };
            // End of Concept Definition |||||||||||||||||||||||||||||||||||||||||||||||||||||||
            // Objects ------------------------------------------------------------------------

            // End of Objects |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            // Functions ---------------------------------------------------------------------

            // End of Functions ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
            // Interface ---------------------------------------------------------------------
            inline static Rule fromStr_rule(QString& str){return static_cast<Rule>(ruleMap.indexOf(str.toLower()));}
            inline static Target fromStr_target(QString& str){return static_cast<Target>(targetMap.indexOf(str.toLower()));}
         private:
            static const QList<QString> ruleMap;
            static const QList<QString> targetMap;
        public:
            static inline std::underlying_type_t<Target> cast_target(const Target t){return static_cast<std::underlying_type_t<Target>>(t);}
            static inline Target castTo_target(const std::underlying_type_t<Target> t){return static_cast<Target>(t);}
            static const QString cast_target_str(const Target t){return QString::number(cast_target(t));}
            static inline std::underlying_type_t<Rule> cast_format_rule(const Rule t){return static_cast<std::underlying_type_t<Rule>>(t);}
            inline static const QString cast_format_rule_str(const Rule t){return QString(cast_format_rule(t));}
            inline static const QString cast_format_rule_str(const Rule t, const QString& data){
                return QString(cast_format_rule(t)) + data;
            }
            inline static const QString FORMAT_RULE_CALL(){return "";}
            inline static void addFormatRule(QStringList& parameters, const Rule rule, const QString& data){
                parameters << FORMAT_RULE_CALL() << cast_format_rule_str(rule, data);
            }
            // End of Interface ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        };
        // ----
    private:
         // -------------------
        class Rule{
        public:
            template<typename _ActionType>
            requires (std::is_same_v<_ActionType, Action::Conditional>
                      or std::is_same_v<_ActionType, Action::Executable>)
            class ActionCall{
            public:
                using ActionType = _ActionType;
                using Parameters = QStringList;

            private:
                ActionType type_;
                Parameters parameters_;

            public:
                ActionCall(){}
                ActionCall(ActionType type, const Parameters& parameters = {}) : type_(type), parameters_(parameters){}

                inline ActionType type()const{return type_;}
                inline Parameters& parameters(){return parameters_;}
                inline std::underlying_type_t<ActionType> cast2underlyingType(){
                    return static_cast<std::underlying_type_t<ActionType>>(type());
                }
                static inline std::underlying_type_t<ActionType> cast2underlyingType(ActionType type){
                    return static_cast<std::underlying_type_t<ActionType>>(type);
                }
            };

            // Concepts --------------------------------------------
            using ConditionalActions =  QVector< ActionCall<Action::Conditional>>;
            using ExecutableActions =  QVector< ActionCall<Action::Executable>>;

            enum class Control : qint8;
            // End of Concepts |||||||||||||||||||||||||||||||||||||||||||||
            // Concept Definition ------------------------------------------
            enum class Control : qint8{
                BreakRuleCheck,
                NoBreakRuleCheck,
                //BreakRuleCheckDontExecOnEndActions,
                Size,
                None = -1,
                _default = BreakRuleCheck
            };
        private:
            static const QList<QString> controlMap;
        public:
            inline static Control fromStr(QString& str){return static_cast<Control>(controlMap.indexOf(str.toLower()));}
            inline static QString toStr(Control flag){return controlMap.at(static_cast<std::underlying_type_t<Control>>(flag));}
            inline static decltype (controlMap)& getRuleControlFlagNames(){
                return controlMap;
            }
            static constexpr Control defaultControlFlag(){return Control::_default;}
            // End of Concept Definitiom |||||||||||||||||||||||||||||||||||||
            // Objects -----------------------------------------------------
            ConditionalActions conditions;
            ExecutableActions actions;
            Control controlFlag = Control::BreakRuleCheck;

            // End of Objects |||||||||||||||||||||||||||||||||||||||||||||||
            // Functions ---------------------------------------------------

            // End of Functions |||||||||||||||||||||||||||||||||||||||||||||
            // Interface ----------------------------------------------------
            Rule(const ConditionalActions& conditions = {}, const ExecutableActions& executables = {}, Control controlFlag = Control::BreakRuleCheck)
                : conditions(conditions), actions(executables), controlFlag(controlFlag) {}
            // End of Interface |||||||||||||||||||||||||||||||||||||||||||||

        };
        using ConditionalActionFunc = Error (Definition::*)();
        using ExecutableActionFunc = Error (Definition::*)();
        // ----
        struct RulesForArgument{
            enum class Status : bool {Specified, Unspecified};
            //const Status status;
            qsizetype index;
            Rules rules;
            Rules rulesOnMoveArgument;
            //Rule::ExecutableActions onNoRules;
        };

        // Properties --------------------------------------------------
    public:
        using RulesOnEndOfCall = Rules;

        using ProcedureName = QString;
        using NumbOfArguments = int;
        using Index = int;
        ProcedureName name;
        UserInteractionStatus userInteraction;
        RulesForArguments rulesForArguments;
        RulesForArgument rulesForUnspecifiedArgument;
        RulesOnEndOfCall rulesOnEndOfProcedureCall;   // Rules

        // End of Properties ||||||||||||||||||||||||||||||||||||||||||||
        // Functions ----------------------------------------------------


        // End of Functions |||||||||||||||||||||||||||||||||||||||||||||
        // Interface -----------------------------------------------------
    public:
        Definition(ProcedureName name = QString(),
                            const RulesForArguments& rulesForArguments = {},
                            const RulesForArgument& rulesForUnspecifiedArgument = {},
                            const RulesOnEndOfCall& rulesOnEnd = {})
            : name(name),
              userInteraction(UserInteractionStatus::NotRequired),
              rulesForArguments(rulesForArguments),
              rulesForUnspecifiedArgument(rulesForUnspecifiedArgument),
              rulesOnEndOfProcedureCall(rulesOnEnd){}
        Definition(ProcedureName name,
                            UserInteractionStatus userInteraction,
                            const RulesForArguments& rulesForArguments = {},
                            const RulesForArgument& rulesForUnspecifiedArgument = {},
                            const RulesOnEndOfCall& rulesOnEnd = {})
            : name(name),
              userInteraction(userInteraction),
              rulesForArguments(rulesForArguments),
              rulesForUnspecifiedArgument(rulesForUnspecifiedArgument),
              rulesOnEndOfProcedureCall(rulesOnEnd){}
        // End of Interface |||||||||||||||||||||||||||||||||||||||||||||
        inline bool isRulesForArgumentsEmpty()const{return rulesForArguments.isEmpty();}
        inline bool isRulesEmpty()const{return rulesForArguments.isEmpty() and rulesForUnspecifiedArgument.rules.isEmpty() and
                    rulesForUnspecifiedArgument.rulesOnMoveArgument.isEmpty() and rulesOnEndOfProcedureCall.isEmpty() ;}

        static Definition defaultUnknownProcedureDefinition;
    };
    inline namespace Types{
        using CommandDefinitions = QList<Definition>;
        using CommandDefinitionIterator = CommandDefinitions::Iterator;
        using CommandDefinitionIterators = QList<CommandDefinitionIterator>;
    };

};
#endif // COMMANDDEF_HPP
