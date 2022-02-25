#ifndef TCLTOCAPL_HPP
#define TCLTOCAPL_HPP


#include"tclinterpreterpriv.hpp"
#include<QtGlobal>
#include<QXmlStreamWriter>
//#include"Tcl2Capl/caplFunctionDefiniitions.hpp


class CAPLFunctionDefinitions;
using CAPLFunctionDefinitionsRef = CAPLFunctionDefinitions&;
class UserInputConfig;

class TCLInterpreter : private TCLInterpreterPriv{
public:
    using TCLInterpreterPriv::Stat;
private:
    // Concept ----------------------------------------------------------------------
    using Stats = QVector<Stat>;
    // ----
    using TCLInterpreterPriv::SavedStat;
    // ---- // Class IO
    using TCLInterpreterPriv::TclCommand;
    using TCLInterpreterPriv::CAPLCommand;
    using TCLInterpreterPriv::CAPLCommands;
    // ----
    using InterpretFunction = Error (TCLInterpreter::*)();
    // ----
    class TCLProceduresInterpreter;
    // ----
    class TextInterpreter;
    // ----
    class IgnoreMessage;
    using IgnoreMessages = QVector<IgnoreMessage>;
    // ----

public:
    using TextInterpreter = TextInterpreter;
    using TCLProceduresInterpreter = TCLProceduresInterpreter;
    using TCLInterpreterPriv::Error;
private:
    // End Of Concept ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    // Concept Definition ------------------------------------------------------------------

public:
    using InterpretFunctions = InterpretFunction[static_cast<std::underlying_type_t<Stat>>(Stat::Size)];
private:
    // ----    
    class TCLProceduresInterpreter{
        // Concept ----------------------------------------------------------------------
        class ProdecuresSettings;

        class ProcedureDefinition;
        // ----
        class ProcedureCall;
        using ProcedureCalls = QVector<ProcedureCall>;

        // End of Concept |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------------------------------
    public:
        using ProcedureDefinition = ProcedureDefinition;
        using ProcedureDefinitions = QVector<ProcedureDefinition>;
        using ProcedureCall = ProcedureCall;
        using UserInteraction = UserInteraction;
        using UserInteractionStatus = UserInteraction::Status;
        inline static UserInteractionStatus fromStr(QString& str){return userInteractionMap.value(str.toLower(), UserInteractionStatus::None);}
        inline static QString toStr(UserInteractionStatus userInteraction){
            return userInteractionMap.key(userInteraction); // Empty String == Not found
        }
     private:
        static QMap<const QString,  UserInteractionStatus> userInteractionMap;
        using ProcedureDefinitionIterator = ProcedureDefinitions::Iterator;
        using ProcedureDefinitionIterators = QVector<ProcedureDefinitions::Iterator>;
        class ProcedureDefinition{
            // Concept ----------------------------------------------------------------------
            class Action;
            // ----
            enum class ModifierRuleControl : uint;
            enum class ParsingMode;
            // ----
            class Rule;
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
                enum class Conditional : uint;
                enum class Executable : uint;

                // End of Concept |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
                // Concept Definition ------------------------------------------------------------------
                enum class Conditional : uint{
                    CompareNumbOfArguments, // Current numb of Arguments for last procedure call (current definition)
                    FCT_Begin = CompareNumbOfArguments,
                    IsLastSavedStat,    // Check Stat for specified SavedStat (Arguments pattern: 1- savedStat number)
                    Compare,


                    Size,
                    End = Size,
                    FCT_End = End,
                    None,
                };

                enum class Executable : uint{
                     Write,
                    FCT_Begin = Write,
                    TclParse,
                    Error,
                    ChangeLastSavedStat,
                    AddFunctionDefinition,
                    AddPreExpression,
                    AddUserInteraction,
                    FCT_End,// VVVVVVVV Privates factory products not required
                    FinalizeForEach = FCT_End,
                    AddSnprintf,
                    AddPredefinition,

                    Size,
                    End = Size,
                    None,
                };
                inline static Conditional fromStr_conditional(QString& str){return conditionalMap.value(str.toLower(), Conditional::None);}
                inline static Executable fromStr_executable(QString& str){return executableMap.value(str.toLower(), Executable::None);}
             private:
                static  QMap<const QString, Conditional> conditionalMap;
                static  QMap<const QString, Executable> executableMap;
                // End of Concept Definition
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
                enum class Target : quint8;

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
                    None = '\0',
                   //SPLIT = 'S',
                   //SLICED = '\\'
                };
                enum class Target : quint8{
                    Raw,    // Orginal Interpreter Read
                    TclFormat,
                    CaplFormat,
                    ProcedureParametersStat,    // Number
                    Command,
                    SnprintfFormat,
                    None,
                    //ProcedureParameters,
                    //OriginalProcedureParameters,
                };
                // End of Concept Definition |||||||||||||||||||||||||||||||||||||||||||||||||||||||
                // Objects ------------------------------------------------------------------------

                // End of Objects |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
                // Functions ---------------------------------------------------------------------

                // End of Functions ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
                // Interface ---------------------------------------------------------------------
                inline static Rule fromStr_rule(QString& str){return ruleMap.value(str.toLower(), Rule::None);}
                inline static Target fromStr_target(QString& str){return targetMap.value(str.toLower(), Target::None);}
             private:
                static  QMap<const QString,  Rule> ruleMap;
                static  QMap<const QString,  Target> targetMap;
            public:
                static inline std::underlying_type_t<Target> cast_target(const Target t){return static_cast<std::underlying_type_t<Target>>(t);}
                static const QString cast_target_str(const Target t){return QString::number(cast_target(t));}
                static inline std::underlying_type_t<Rule> cast_format_rule(const Rule t){return static_cast<std::underlying_type_t<Rule>>(t);}
                static const QString cast_format_rule_str(const Rule t){return QString(cast_format_rule(t));}
                static const QString FORMAT_RULE_CALL(){return "";}
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
                    ActionCall(ActionType type, Parameters parameters = {}) : type_(type), parameters_(parameters){}

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

                enum class Control : quint8;
                // End of Concepts |||||||||||||||||||||||||||||||||||||||||||||
                // Concept Definition ------------------------------------------
                enum class Control : quint8{                    
                    BreakRuleCheck,
                    NoBreakRuleCheck,
                    //BreakRuleCheckDontExecOnEndActions,
                    None,
                    _Default = BreakRuleCheck
                };
            private:
                static QMap<QString,  Control> controlMap;
            public:                
                inline static Control fromStr(QString& str){return controlMap.value(str.toLower(), Control::None);}
                inline static QString toStr(Control flag){return controlMap.key(flag);}
                inline static decltype (controlMap.keys()) getRuleControlFlagNames(){
                    return controlMap.keys();
                }
                static constexpr Control defaultControlFlag(){return Control::_Default;}
                // End of Concept Definitiom |||||||||||||||||||||||||||||||||||||
                // Objects -----------------------------------------------------
                ConditionalActions conditions;
                ExecutableActions actions;
                Control controlFlag = Control::BreakRuleCheck;

                // End of Objects |||||||||||||||||||||||||||||||||||||||||||||||
                // Functions ---------------------------------------------------

                // End of Functions |||||||||||||||||||||||||||||||||||||||||||||
                // Interface ----------------------------------------------------
                Rule(ConditionalActions conditions = {}, ExecutableActions executables = {}, Control controlFlag = Control::BreakRuleCheck)
                    : conditions(conditions), actions(executables), controlFlag(controlFlag) {}
                // End of Interface |||||||||||||||||||||||||||||||||||||||||||||

            };
            using ConditionalActionFunc = Error (TCLProceduresInterpreter::*)();
            using ExecutableActionFunc = Error (TCLProceduresInterpreter::*)();
            // ----
            struct RulesForArgument{
                enum class Status : bool {Specified, Unspecified};
                //const Status status;
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
            uint currentLevel = 0;

            // End of Properties ||||||||||||||||||||||||||||||||||||||||||||
            // Functions ----------------------------------------------------


            // End of Functions |||||||||||||||||||||||||||||||||||||||||||||
            // Interface -----------------------------------------------------
        public:
            ProcedureDefinition(ProcedureName name = QString(),
                                RulesForArguments rulesForArguments = {},
                                RulesForArgument rulesForUnspecifiedArgument = {},
                                RulesOnEndOfCall rulesOnEnd = {})
                : name(name),
                  userInteraction(UserInteractionStatus::NotRequired),
                  rulesForArguments(rulesForArguments),
                  rulesForUnspecifiedArgument(rulesForUnspecifiedArgument),
                  rulesOnEndOfProcedureCall(rulesOnEnd){}
            ProcedureDefinition(ProcedureName name,
                                UserInteractionStatus userInteraction,
                                RulesForArguments rulesForArguments = {},
                                RulesForArgument rulesForUnspecifiedArgument = {},
                                RulesOnEndOfCall rulesOnEnd = {})
                : name(name),
                  userInteraction(userInteraction),
                  rulesForArguments(rulesForArguments),
                  rulesForUnspecifiedArgument(rulesForUnspecifiedArgument),
                  rulesOnEndOfProcedureCall(rulesOnEnd){}
            // End of Interface |||||||||||||||||||||||||||||||||||||||||||||
            inline void nextLevel(){currentLevel++;}
            inline Error previousLevel(){return ((currentLevel == 0)?
                                                     (currentLevel--, Error::NoError) :
                                                     (throwError("Too Low Level"), Error::Error));}
            inline bool isRulesForArgumentsEmpty()const{return rulesForArguments.isEmpty();}
            inline bool isRulesEmpty()const{return rulesForArguments.isEmpty() and rulesForUnspecifiedArgument.rules.isEmpty() and
                        rulesForUnspecifiedArgument.rulesOnMoveArgument.isEmpty() and rulesOnEndOfProcedureCall.isEmpty() ;}

        };
        class ProcedureCall{
            public:
            // Concept -----------------------------------------------------
            using Name = ProcedureDefinition::ProcedureName;
            using PreExpression = CAPLCommand;
            using PreExpressions = QVector<CAPLCommand>;
            using SquareBracketLevel = uint;
            class Parameter;
            using Parameters = QVector<Parameter>;

            // End of Concepts ||||||||||||||||||||||||||||||||||||||||||||
            // Concept Definition ------------------------------------------
            // ----
            class Parameter{

                SavedStat _savedStat;
                //UserInteraction _userInteraction = UserInteraction::NotRequired;
                Parameters rawParameterStats;
            public:

                Parameter(SavedStat savedStat) : _savedStat(savedStat){}
                Parameter(SavedStat savedStat, ProcedureCall& procedureCall) : _savedStat(savedStat){
                    rawParameterStats = {Parameter(Stat::String, procedureCall.name())};
                    rawParameterStats += procedureCall.rawParameters();
                }
                Parameter(const Stat stat, CAPLCommand caplCommand) : _savedStat{stat, caplCommand}{}
                //Parameter(SavedStat savedStat) : _savedStat(savedStat){}

                inline Stat stat()const{return _savedStat.stat();}
                inline QString caplCommand()const{return _savedStat.caplCommand();}
                //inline void setUserIteractionRequired(){_userInteraction = UserInteraction::Required;}
                inline void setStat(Stat stat){_savedStat.setStat(stat);}
                inline void setCaplCommand(CAPLCommand caplCommand){_savedStat.setCAPLCommand(caplCommand);}
                //inline UserInteraction userIteraction(){return _userInteraction;}
                inline SavedStat& savedStat(){return _savedStat;}
                inline const Parameters& rawParameters()const{return rawParameterStats;}

                QString toString(ProcedureDefinition::Format::Target);
                inline void clearMemory(){
                    using Parameter = Parameters::Iterator;
                    _savedStat.clearMemory();
                    for(Parameter param = rawParameterStats.begin() ; param < rawParameterStats.end(); param++)
                        param->clearMemory();
                }

            };
            // End of Concept Definition ||||||||||||||||||||||||||||||||||
            // Objects ----------------------------------------------------
            protected:
                Name _name;
                Parameters _parameters;
                Parameters _rawParameters;
                ProcedureDefinitionIterator _procedureDefinition;
                //SquareBracketLevel _squareBracketLevel;
                //UserInteraction _userInteraction;

            // End of Objects ||||||||||||||||||||||||||||||||||||||||||||
            // Functions --------------------------------------------------

            // End of Functions |||||||||||||||||||||||||||||||||||||||||||
            // Interface -------------------------------------------------
            public:
                ProcedureCall(QString name)
                    : _name(name), _procedureDefinition(&defaultUnknownProcedureDefinition)
                {
                    if(name.isEmpty())
                        throw std::runtime_error("New Procedure Call without name and default procedure definition");
                }

                ProcedureCall(ProcedureDefinitionIterator procedureDefinition)
                    : _procedureDefinition(procedureDefinition)/*, _userInteraction(_procedureDefinition->userInteraction)*/
                {}

                //ProcedureCall& operator=(ProcedureCall&& call){_name = call._name; _arguments = call._arguments;}
                inline void nextArgument(Parameter& arg){_parameters.append(arg); _rawParameters.append(arg);}
                inline Parameters::size_type parametersLength()const{return _parameters.size();}
                inline Parameters::size_type lastArgumentIndex()const{return parametersLength();}
                inline QString name()const{return (_name.isEmpty())? _procedureDefinition->name : _name;}
                inline ProcedureDefinition::RulesForArguments::Iterator lastRulesForArgument_dynamicCheck()const{
                    return (lastArgumentIndex() < _procedureDefinition->rulesForArguments.size())?
                                _procedureDefinition->rulesForArguments.begin() + lastArgumentIndex() :
                                _procedureDefinition->rulesForArguments.end();}
                inline ProcedureDefinition::RulesForArguments::Iterator lastRulesForArgument_onMoved()const{
                    return (lastArgumentIndex() - 1 < _procedureDefinition->rulesForArguments.size())?
                                _procedureDefinition->rulesForArguments.begin() + lastArgumentIndex() - 1 :
                                _procedureDefinition->rulesForArguments.end();}
                inline ProcedureDefinition::RulesForArguments::Iterator rulesForUnspecifiedArgument()const{
                    return &_procedureDefinition->rulesForUnspecifiedArgument;}
                inline Error isRulesInRange(ProcedureDefinition::RulesForArguments::Iterator rules)const{
                    return (rules < _procedureDefinition->rulesForArguments.constEnd())?
                                Error::NoError :
                                Error::Error;}
                inline ProcedureDefinition::RulesOnEndOfCall& rulesOnEndOfCall()const{
                    return _procedureDefinition->rulesOnEndOfProcedureCall;
                }

                inline bool isRulesForArgumentsEmpty()const{return _procedureDefinition->isRulesForArgumentsEmpty();}
                inline bool isRulesEmpty()const{return _procedureDefinition->isRulesEmpty();}
                inline ProcedureCall::Parameters::reference lastParameter(){return _parameters.last();}
                //inline bool canBeFinalized(SquareBracketLevel level){return level == _squareBracketLevel;}
                inline Parameters& parameters(){return _parameters;}
                //inline QString generateCAPLFunctionDefinitionExample(){return QString("// ") + name() + "(" + generateCAPLFunctionDefinitionExampleParameters() + ")";}
                inline QString generateCAPLFunctionDefinitionExample(){return QString("") + name() + " " + generateCAPLFunctionDefinitionExampleParameters() + "";}
                inline QString generateCAPLFunctionDefinitionExampleParameters(){
                    QString str;
                    for(Parameters::Iterator parameter = parameters().begin(); parameter < _parameters.end(); parameter++)
                        str.append(parameter->toString(ProcedureDefinition::Format::Target::TclFormat) + " ");
                    str.chop(QStringLiteral(" ").length());
                    return str;
                }
                inline QList<Stat> parametersStats(){
                    QList<Stat> stats(parameters().size());
                    for(Parameters::Iterator parameter = parameters().begin(); parameter < _parameters.end(); parameter++)
                        stats[parameter - parameters().begin()] = parameter->stat();
                    return stats;//QList<Stat>(stats.begin(), stats.end());
                }
                inline Parameters& rawParameters(){return _rawParameters;}
                inline bool isUserInteractionRequired()const{return /*_userInteraction == UserInteraction::Required;*/ _procedureDefinition->userInteraction == UserInteractionStatus::Required;}
                inline void clearMemory(){
                    using Parameter = Parameters::Iterator;
                    for(Parameter param = _rawParameters.begin(); param < _rawParameters.end(); param++)
                        param->clearMemory();
                }
                QString tryToAddEndOfExpressionSign()const{
                    if(name() == "expr_parser")
                        return QString();
                    if( not _parameters.isEmpty()){
                        switch(_parameters.last().stat()){
                        case Stat::EndOfCodeBlock:
                            return QString("\n");
                        }
                    }
                    return QString(";\n");
                }
                // End of Interface |||||||||||||||||||||||||||||||||||||||||||||
        };
        // ----

        enum ConditionResult : bool{Satisfied, Unsatisfied};
        // ----
        class ProdecuresSettings{
        public:
            using WriteOnlyProcedures = QStringList;

            enum class InterpreterMode : quint8{
                TestCase,
                TestCaseReport,
                // No Write To File Modes
                NoWriteToFileModes,
                PredefinitionsOnly = NoWriteToFileModes,

                NumbOfModes
            } mode_ = InterpreterMode::TestCase;

            constexpr static std::underlying_type_t<InterpreterMode> mode2number(InterpreterMode mode){
                return static_cast<std::underlying_type_t<InterpreterMode>>(mode);
            }
        protected:
            WriteOnlyProcedures writeOnlyProcedures_;

        public:
            inline InterpreterMode mode(){return mode_;}
            inline bool isWriteToFileMode(){return mode_ < InterpreterMode::NoWriteToFileModes; }
            inline void appendWriteOnlyProcedure(WriteOnlyProcedures procedure){
                writeOnlyProcedures_.append(procedure);
                writeOnlyProcedures_.removeDuplicates();
                writeOnlyProcedures_.replaceInStrings("\t", "");
                writeOnlyProcedures_.replaceInStrings(" ", "");
            }
            inline void setMode(InterpreterMode newValue){mode_ = newValue;}
            inline bool isWriteOnlyProcedure(WriteOnlyProcedures::Type procedure){return writeOnlyProcedures_.contains(procedure);}
            // TEMP _Debug_Only
            inline WriteOnlyProcedures& writeOnlyProcedures(){return writeOnlyProcedures_;}
            inline void toXmlContent(QXmlStreamWriter& xmlWriter){
                // CONFIG_TAG
                xmlWriter.writeTextElement("writeOnlyProcedures", writeOnlyProcedures().join("\n"));
            }
        };
    public:
        using ConditionalActionsParameters = const ProcedureDefinition::Rule::ActionCall<ProcedureDefinition::Action::Conditional>::Parameters&;
        using ExecutableActionsParameters = const ProcedureDefinition::Rule::ActionCall<ProcedureDefinition::Action::Executable>::Parameters&;

        using ConditionInterpretFunction = ConditionResult (TCLProceduresInterpreter::*)(ConditionalActionsParameters);
        using ExecutableInterpretFunction = void (TCLProceduresInterpreter::*)(ExecutableActionsParameters);
        using ConditionInterpretFunctions = ConditionInterpretFunction[static_cast<std::underlying_type_t<ProcedureDefinition::Action::Conditional>>(ProcedureDefinition::Action::Conditional::Size)];
        using ExecutableInterpretFunctions = ExecutableInterpretFunction[static_cast<std::underlying_type_t<ProcedureDefinition::Action::Executable>>(ProcedureDefinition::Action::Executable::Size)];

        // ----
        using CAPLFunctionDefinitionInfo = QMap<uint, QStringList>; // <Numb of Parameters, Examples/ Occurencies>
        using CAPLFunctionDefinitions = QMap<QString, CAPLFunctionDefinitionInfo>;    // <Name, Info>
        using ProdecuresSettings = ProdecuresSettings;

        using NewProcedureCallFunction =  Error (TCLProceduresInterpreter::* const)(ProcedureCall::Name);
        using FinalizeProcedureCallFunction = Error (TCLProceduresInterpreter::*)(SavedStat&);
        // ----

    private:
        // End of Concept Definition |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

        // Objects -------------------------------------------------------------------------------
        TCLInterpreter& tclInterpreter;
        ProcedureDefinitions& procedureDefinitions;
        static ProcedureDefinitions defaultProcedureDefinitions;
        ProcedureDefinition& unknownProcedureDefinition;
        static ProcedureDefinition defaultUnknownProcedureDefinition;
        CAPLCommand command;
        ProcedureCalls procedureCalls;
        ProcedureCall::Parameters rawParameterStats;
        //UserInteraction& userInteraction;
        //Preexpressions& preexpressions;

        bool finalizeOn = false;
        NewProcedureCallFunction newProcedureCallFunction = ProcedureCallFunctions::newCallAt(ProdecuresSettings::InterpreterMode::TestCase);
        FinalizeProcedureCallFunction finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);
        ProcedureCalls::size_type writeOnlyProcedureActiveIndex = -1;

        //CAPLCommand caplCommand;

        static ConditionInterpretFunctions conditionalInterpreterFunctions;
        static ExecutableInterpretFunctions executableInterpretFunctions;

        ProcedureCall::SquareBracketLevel squareBracketsLevel = 0;

        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Functions -----------------------------------------------------------------------------

        template<ProcedureDefinition::Rule::ConditionalActions::value_type::ActionType stat>
        ConditionResult executeConditionalAction(ConditionalActionsParameters);
        template<ProcedureDefinition::Rule::ExecutableActions::value_type::ActionType stat>
        void executeAction(ExecutableActionsParameters);

        inline Error addPreExpressionForUserInteraction(){
            ExecutableActionsParameters parameters =
            {
              QString("// _UI_") + QString::number(tclInterpreter.userInteraction().nextUserInteractionNumber()) + " ",
              ProcedureDefinition::Format::FORMAT_RULE_CALL(),
              "=",
              " ",
              ProcedureDefinition::Format::FORMAT_RULE_CALL(),
               "@ ",
              ProcedureDefinition::Format::FORMAT_RULE_CALL(),
                ">0"
            };
            command = QString("/* _UI_") + QString::number(tclInterpreter.userInteraction().nextUserInteractionNumber()) + " */";
            (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<ProcedureDefinition::Action::Executable>>(ProcedureDefinition::Action::Executable::AddPreExpression)]))(parameters);
            tclInterpreter.userInteraction().addNewInfo();
            if(tclInterpreter.isError()){
                return Error::Error;
            }

            return Error::NoError;
        }
        // End of Functions ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Interface ----------------------------------------------------------------------------
    public:
        TCLProceduresInterpreter(TCLInterpreter& tclInterpreter, UserInputConfig& userConfig);

        // WriteOnlyProcedures
        inline void tryToActivateWriteOnlyProcedure(ProcedureCall::Name& name);
        //inline bool isWriteOnlyProcedureActive()const{return writeOnlyProcedureActiveIndex != -1;}
        inline void tryToDeactivateWriteOnlyProcedure(){
            if(writeOnlyProcedureActiveIndex == procedureCalls.size()){
                writeOnlyProcedureActiveIndex = -1;
                deactivateWriteOnlyProcedureMode();
            }
        }
        // ---------------------------
        inline void activateWriteOnlyProcedureMode(){
            finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);
            tclInterpreter.activateWriteOnlyProcedureMode();
        }

        inline void deactivateWriteOnlyProcedureMode(){
            finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCaseReport);
            tclInterpreter.deactivateWriteOnlyProcedureMode();
        }

        inline Error newProcedureCall(ProcedureCall::Name name){return (this->*newProcedureCallFunction)(name);}

        template<TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode>
        Error newProcedureCall_mode(ProcedureCall::Name name);

        class ProcedureCallFunctions{

            static const NewProcedureCallFunction newProcedureCalls[
                    std::underlying_type_t<TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode>
                    (TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode::NumbOfModes)];
            static const FinalizeProcedureCallFunction finalizeProcedureCalls[
                    std::underlying_type_t<TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode>
                    (TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode::NumbOfModes)];
            using Settings = TCLProceduresInterpreter::ProdecuresSettings;
        public:
            static inline NewProcedureCallFunction newCallAt(Settings::InterpreterMode mode){
                return newProcedureCalls[Settings::mode2number(mode)];
            }
            static inline FinalizeProcedureCallFunction finalizeCallAt(Settings::InterpreterMode mode){
                return finalizeProcedureCalls[Settings::mode2number(mode)];
            }
        };

        inline Error removeProcedureCall(){return procedureCalls.isEmpty()? throwError("TclProcedureInterpreter_Internal: No procedure to remove") :
                                                                            (procedureCalls.removeLast(), Error::NoError);}
        Error nextArgument();
        Error nextArgumentForSnprintf_priv(Stat stat);

        inline Error finalizeProcedureCall(SavedStat& savedStat){return (this->*finalizeProcedureCallFunction)(savedStat);}

        template<TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode>
        Error finalizeProcedureCall_mode(SavedStat&);

        static FinalizeProcedureCallFunction finalizeProcedureCalls[
        std::underlying_type_t<TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode>
        (TCLProceduresInterpreter::ProdecuresSettings::InterpreterMode::NumbOfModes)];

       // Error finalizeSnprintfCall(ProcedureCall::Parameter&);
        Error dynamicProcedureCheck();
        Error onArgumentProcedureCheck();
        Error dynamicProcedureArgumentCheck_priv();
        Error onArgumentProcedureCheck_priv();
        const QString lastProcedureName()const{return ((procedureCalls.isEmpty())? QString() : procedureCalls.last().name());}
        inline ProcedureCalls::size_type numberOfProcedureCalls()const{return procedureCalls.size();}
        inline ProcedureCall& lastProcedureCall(){return procedureCalls.last();}
        inline ProcedureCall& prelastProcedureCall(){return *(procedureCalls.end() - 2);}
        //inline bool canLastProcedureCallBeFinalized(){return procedureCalls.last().canBeFinalized(squareBracketsLevel);}

        // Action Functions
        QStringList::size_type createAndAssignString(QString&, QStringList);
        QString toString(SavedStat&, ProcedureDefinition::Format::Target);
        // ----

        // Function to create ConditionalAction compare
        static const ProcedureDefinition::Rule::ConditionalActions::value_type newCompareRule(const QStringList stringsToCompare, const QStringList format) ;
        // End of Interface |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        static void addDefaultProcedureDefinitionsToUserProcedureDefintions(UserInputConfig& userDefinitions);

    };

    class TextInterpreter
    {
        // Concept -----------------------------------------------------
        using String = QString;
        using Char = QChar;
        using Chars = QVector<Char>;
        using CharPosition = QString::const_iterator;
        using TCLStat = TCLInterpreter::Stat;

        struct Keyword;
        using Keywords =  QVector<Keyword>;

        enum class ReadIgnoreResult : bool;
        enum class CheckingResult : uint;
        enum class CheckKeywordResult : bool;
        enum class Result : uint;

    public:
        using Result = Result;
        using Stat = Stat;
        using CheckingResult = CheckingResult;
        using ReadIgnoreResult = ReadIgnoreResult;
        using KeywordsMap = QVector<Keywords>;
    private:
        // End of Concepts ||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------

        struct Keyword{
            enum class UnknownStringRule : bool{
              Forbidden,
              Permitted
            };
            const QString keyword;
            using Stats = QVector<TCLStat>;
            const Stats stats;
            const UnknownStringRule unknownStringRule = UnknownStringRule::Permitted;
        };

        enum class ReadIgnoreResult : bool{
            Satisfied = true,
            EndOfString = false
        };

        enum class CheckingResult : uint{
            StatFound,
            StatNotFound,

            EndOfString
        };

        enum class CheckKeywordResult : bool{
            Found,
            NotFound
        };

        enum class Result : uint{
           StatFound,
           //WaitForContinue,
           EndOfString,
           Error,
        };

        // End of Concept Definition ||||||||||||||||||||||||||||||||||
        // Objects ----------------------------------------------------
        Stat __stat;
        int lastKeywordLength = 0;
        Keywords::ConstIterator lastKeyword;
        const Keyword endOfStringKeyword = {"\n", {TCLStat::EndOfString}, Keyword::UnknownStringRule::Permitted};

        String savedStr; // Appended at end of String
        Chars ignoreReadUntilCondtions;
        CharPosition savedChar; // Used to save first character of substring which didnt match
        CharPosition savedFirstKeywordCharPos;
        CharPosition currentChar;
        CharPosition beginOfString;
        CharPosition endOfString;
        Result result = Result::StatFound;
    public:
        static KeywordsMap keywordsMap;
    private:
        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||
        // Functions --------------------------------------------------
        ReadIgnoreResult readIgnoreUntilPriv();
        ReadIgnoreResult readIgnoreIfPriv();
        CheckingResult checkingPriv();
        CheckingResult checkKeyword(int&);

        Result interpret();
        inline Char readCharacter(CharPosition chPos)const{return *chPos;}
        // End of Functions |||||||||||||||||||||||||||||||||||||||||||
        // Interface -------------------------------------------------
    public:
       ~TextInterpreter(){}

        inline Keyword::Stats lastKeywordStats()const{return lastKeyword->stats;}
        Error initialize(const QString&);
        Error deinitialize();

        inline Result runSearchingMode(){return interpret();}

        inline QString readLastKeyword(){return QString(savedFirstKeywordCharPos, currentChar - savedFirstKeywordCharPos);}
        inline bool isUnknownString(){return savedFirstKeywordCharPos - savedChar > 0;}
        inline QString readUnknownString(){return QString(savedChar, savedFirstKeywordCharPos - savedChar);}
        inline Error throwErrorIfUnknownStringForForbiddenRule(){
            return (lastKeyword->unknownStringRule == Keyword::UnknownStringRule::Forbidden
                    && isUnknownString())?  Error::Error : Error::NoError;
        }
        inline QString restOfString()const{return QString(currentChar, endOfString - currentChar);}
        inline QString readTclCommand()const{return QString(beginOfString, endOfString - beginOfString);}
        inline bool isCurrentChar(){return currentChar < endOfString;}
        inline CharPosition currentCharForSpecialSign(){return currentChar;}
        inline void incrementCurrentCharDueToSpecialSign(){currentChar++;}
        // End of Interface |||||||||||||||||||||||||||||||||||||||||||||

    };
    // ----
    class Variables{
        // Concept --------------
        using Name = QString;
        using SetValue = QString;
        class Value{
        public:
            using InitValue = QString;
            using CurrentValue = QString;
        protected:
            InitValue _initValue;
            CurrentValue _currentValue;
        public:
            Value(InitValue initValue) : _initValue(initValue){}
            InitValue initValue(){return _initValue;}
            CurrentValue currentValue(){return (_currentValue.isEmpty())? _initValue : _currentValue;}
            void setValue(CurrentValue currentValue){_currentValue = currentValue;}
        };
        using VariablesPriv = QMap<Name, Value>;
        VariablesPriv variables;
        // ----------------------
        // Interface
     public:
        void setValue(Name name, SetValue setValue){
            VariablesPriv::Iterator variable = variables.find(name);
            if(variable == variables.end()){    // Not Found
                variables.insert(name, Value(setValue));
            }else{
                variable->setValue(setValue);
            }
        }
        // End of Interface
    };
    // ---

    class IgnoreMessage{
    public:
        IgnoreMessage(QString message, QString fullLineData, QString restOfLineData)
            : message(message), fullLineData(fullLineData), restOfLineData(restOfLineData){}
    private:
        QString message;
        QString fullLineData;
        QString restOfLineData;
    public:
        QString toString(){return "<b>Message:</b> " + message + "<br>\n<b>Line:</b> " + fullLineData + "<br>\n<b>Rest of Line:</b> " + restOfLineData;}
    };
    // ----

    using RemoveProcedureCallFunction = Error (TCLInterpreter::*)();

    // End of Concept Definition
    // Objects ----------------------------------------------------
    protected:
        CAPLFunctionDefinitionsRef caplFunctionDefinitions;
        //Variables variables;
        TextInterpreter textInterpreter;
        TCLProceduresInterpreter tclProceduresInterpreter;
        UserInputConfig& userConfig;

        Stats proccessingStats{};
        Stats pendingProccessingStats{}; // Saved stats to use in next initialization

        static InterpretFunctions interpretFunctions;
        IgnoreMessages ignoreMessages;
        RemoveProcedureCallFunction removeProcedureCallFunction = &TCLInterpreter::removeProcedureCall_standard;        
        bool _whitespace = false;

    // End of Objects ||||||||||||||||||||||||||||||||||||||||||||
    // Functions --------------------------------------------------

        template<Stat stat>
        Error interpret();

        inline Error callInterpretFunction(Stat stat = Stat::Size){
            if(checkWhitespace() == Error::NoError){
                if(isSavedStatsEmpty())
                    return throwError("Empty Stats after Whitespace");

            }else{
                return Error::Error;
            }
            if(stat != Stat::Size){
                proccessingStats.append(stat);
            }
            return (this->*(interpretFunctions[static_cast<std::underlying_type_t<Stat>>(proccessingStats.takeLast())]))();
        }

        inline Error processUnknownString(){
           return (textInterpreter.isUnknownString())? callInterpretFunction(Stat::UnknownString) : Error::NoError;
        }

        inline void addPendingProcessingStat(Stats stats){pendingProccessingStats.append(stats);}
        inline void addPendingProcessingStat(Stat stat){pendingProccessingStats.append(stat);}
        inline Error checkWhitespace(){
            return ((_whitespace = (lastSavedStat().stat() == Stat::Whitespace)))?
                        removeLastStat() :
                        Error::NoError;
        }
        inline bool isWhitespaceOccured()const{return _whitespace;}
        //Error interpretFunctionCall(qsizetype functionCallPos);
        inline bool isError(){return not _error.isEmpty();}

        Error moveArgumentToFunctionCall();
        Error moveArgumentToSnprintf_priv(const Stat);
    // End of Functions |||||||||||||||||||||||||||||||||||||||||||
        inline bool isLastProcedureCallExpr(){return tclProceduresInterpreter.numberOfProcedureCalls() > 0 and tclProceduresInterpreter.lastProcedureName() == "expr";}

        inline Error moveArgumentToSnprintf(){return moveArgumentToSnprintf_priv(Stat::Snprintf);}
        inline Error moveArgumentToPendingSnprintf(){return moveArgumentToSnprintf_priv(Stat::PendingSnprintf);}

        inline bool isPrelastSavedStat()const{return savedStatsBegin() <= savedStatsEnd() - 2;}
        inline SavedStat& prelastSavedStat(){return *(savedStats().end() - 2);}

        void addIgnoreMessage(IgnoreMessage message){ignoreMessages.append(message);}
        Error removeIgnore(){return (isSavedStatsEmpty())?
                            throwError("TCLInterpreter::removeIgnore: No stats") : (removeLastStat(), Error::NoError);}



        Error newProcedureCall(TCLProceduresInterpreter::ProcedureCall::Name name);
        inline Error removeProcedureCall(){return (this->*removeProcedureCallFunction)();}
        inline Error removeProcedureCall_standard(){
            const QString ERROR_PREFIX = "TCLInterpreter::removeProcedureCall: ";
            if(tclProceduresInterpreter.numberOfProcedureCalls() == 0)
                return throwError(ERROR_PREFIX + "No procedure calls");

            if(tclProceduresInterpreter.removeProcedureCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
            return Error::NoError;
        }
        Error removeProcedureCall_writeOnlyProcedure();
        Error finalizeProcedureCall();
        Error finalizeSnprintfCall();

        Error processError(){addIgnoreMessage(IgnoreMessage(error(), textInterpreter.readTclCommand(), textInterpreter.restOfString()));
                             if(processSavedStatsForError() == Error::Error)
                                 return Error::Error;
                             clearError();
                            return Error::NoError;}
        Error processSavedStatsForError();
        inline void clearError(){_error.clear();}
        Error saveStatWithParsingControl(SavedStat);
        Error removeLastSavedStatWithParsingControl();
        Error takeLastSavedStatWithParsingControl(SavedStat&);

        void addEmptyLine();
    // Interface -------------------------------------------------
    public:
        TCLInterpreter(UserInputConfig& userConfig, CAPLFunctionDefinitionsRef caplFunctionDefinitions);

        inline bool isComplete()const{return savedStatsSize() == 1;}

        inline const QString& error(){return _error;}
        Error toCAPL(TclCommand&);
        //Error addReadyCommand(TclCommand&);
        inline void activateWriteOnlyProcedureMode(){
            removeProcedureCallFunction = &TCLInterpreter::removeProcedureCall_writeOnlyProcedure;
            addExpressionToCodeBlockFunction = &TCLInterpreterPriv::addExpressionToCodeBlock_writeOnlyProcedure;
        }

        inline void deactivateWriteOnlyProcedureMode(){
            removeProcedureCallFunction = &TCLInterpreter::removeProcedureCall_standard;
            addExpressionToCodeBlockFunction = &TCLInterpreterPriv::addExpressionToCodeBlock_standard;

        }

        static bool checkInterpretFunctions();
        inline bool anyErrors(){return ignoreMessages.size();}
        void printErrorReport(QFile& reportFile, QString inputFileName);
        void printErrorReport(QString& inputFileName);
        CAPLCommand& readCaplCommand(){return TCLInterpreterPriv::readCaplCommand();}
        inline CAPLCommand printPredefinitions(){return predefinitionsControl().getPredefinitionsStr();}

        static inline std::underlying_type_t<Stat> cast_stat(const Stat t){return static_cast<std::underlying_type_t<Stat>>(t);}
        static inline QString cast_stat_str(const Stat t){return QString::number(cast_stat(t));}
        inline IgnoreMessages::size_type getErrorsNumber()const{return ignoreMessages.size();}
        inline PredefinitionsControl::Predefinitions& predefinitions(){return predefinitionsControl().getPredefinitions();}

        bool isPredefinitionMode();
    // End of Interface |||||||||||||||||||||||||||||||||||||||||||||

     // Interpret Functions


    // ----------------------------

};


#endif // TCLTOCAPL_HPP
