#ifndef TCLPROCEDURESINTERPRETER_HPP
#define TCLPROCEDURESINTERPRETER_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include<QXmlStreamWriter>
#include"TclInterpreter/SavedStatCont/savedStatCont.hpp"
#include"TclInterpreter/UserInteractionCont/userInteractionCont.hpp"
#include<QMap>
#include"TclInterpreter/ErrorCont/errorCont.hpp"
#include"commandCall.hpp"
#include"commandCallConfig.hpp"
#include"TclInterpreter/CallReplacerCont/callReplacerCont.hpp"
#include"TclInterpreter/ExprCont/exprCont.hpp"

class UserInputConfig;
namespace Tcl{
    class TCLInterpreter;
};

namespace Tcl::Interpreter::Command{

    using namespace Tcl::Interpreter::Types;
    using namespace Tcl::Interpreter::Core;
    using namespace Tcl;

    class Controller{
        // Concept ----------------------------------------------------------------------
        // ----
        // End of Concept |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------------------------------

    public:
        inline static UserInteractionStatus fromStr(QString& str){return static_cast<UserInteractionStatus>(userInteractionMap.indexOf(QRegularExpression(str, QRegularExpression::CaseInsensitiveOption)));}
        inline static QString toStr(UserInteractionStatus userInteraction){
            return userInteractionMap.at(userInteraction); // Empty String == Not found
        }
     private:
        // Aliases
        Error throwError();
        Error throwError(const QString str);
        const QString& error();
        // ---------

        static const QList<QString> userInteractionMap;

        // ----

        enum ConditionResult : bool{Satisfied, Unsatisfied};
        // ----

    public:
        using ConditionalActionsParameters = const Definition::Rule::ActionCall<Definition::Action::Conditional>::Parameters&;
        using ExecutableActionsParameters = const Definition::Rule::ActionCall<Definition::Action::Executable>::Parameters&;

        using ConditionInterpretFunction = ConditionResult (Controller::*)(ConditionalActionsParameters);
        using ExecutableInterpretFunction = void (Controller::*)(ExecutableActionsParameters);
        using ConditionInterpretFunctions = ConditionInterpretFunction[static_cast<std::underlying_type_t<Definition::Action::Conditional>>(Definition::Action::Conditional::PrivateSize)];
        using ExecutableInterpretFunctions = ExecutableInterpretFunction[static_cast<std::underlying_type_t<Definition::Action::Executable>>(Definition::Action::Executable::PrivateSize)];

        // ----
        using FunctionDefinitionInfo = QMap<uint, QStringList>; // <Numb of Parameters, Examples/ Occurencies>
        using FunctionDefinitions = QMap<QString, FunctionDefinitionInfo>;    // <Name, Info>

        //inline Error emptyCommandCallSpecialInterpret(const Stat){return Error::NoError;}
        // ----

    private:
        // End of Concept Definition |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

        // Objects -------------------------------------------------------------------------------
        TCLInterpreter& tclInterpreter;
        UserInputConfig& userConfig;        
        CommandDefinitions& procedureDefinitions;
        Definition& unknownProcedureDefinition;
        static CommandDefinitions hardcodedProcedureDefinitions;
        static CommandDefinitions defaultProcedureDefinitions;
        OutputCommand command;
        Calls procedureCalls;
        Call::Parameters rawParameterStats;
        CallReplacer::Controller callReplacerController;
        ExprController::Controller exprController;
        //UserInteraction& userInteraction;
        //Preexpressions& preexpressions;

        bool finalizeOn = false;
        Settings::CallDefinitionInterModeFctPtr callDefinitionFunction = Settings::newCallAt(Settings::InterpreterMode::TestCase);
        Settings::FinalizeCallInterModeFctPtr finalizeCallFunction = Settings::finalizeCallAt(Settings::InterpreterMode::TestCase);

        //CommandCallSpecialInterpret commandCallSpecialInterpret = &Controller::emptyCommandCallSpecialInterpret;

        Calls::size_type writeOnlyProcedureActiveIndex = -1;
        Calls::size_type ignoreModeActiveIndex = -1;

        //Command command;

        static ConditionInterpretFunctions conditionalInterpreterFunctions;
        static ExecutableInterpretFunctions executableInterpretFunctions;

        //Call::SquareBracketLevel squareBracketsLevel = 0;

        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Functions -----------------------------------------------------------------------------

        template<Definition::Rule::ConditionalActions::value_type::ActionType stat>
        ConditionResult executeConditionalAction(ConditionalActionsParameters);
        template<Definition::Rule::ExecutableActions::value_type::ActionType stat>
        void executeAction(ExecutableActionsParameters);

        Error addPreExpressionForUserInteraction(QString);


        CallConfig callConfig;
        CallConfig::CommandCallControlFunctions const* currentCommandCallFunctions = nullptr;
    //    {
    //        ExecutableActionsParameters parameters =
    //        {
    //          QString("// _UI_") + QString::number(tclInterpreter.userInteraction().nextUserInteractionNumber()) + " ",
    //          Definition::Format::FORMAT_RULE_CALL(),
    //          "=",
    //          " ",
    //          Definition::Format::FORMAT_RULE_CALL(),
    //           "@ ",
    //          Definition::Format::FORMAT_RULE_CALL(),
    //            ">0"
    //        };
    //        command = QString("/* _UI_") + QString::number(tclInterpreter.userInteraction().nextUserInteractionNumber()) + " */";
    //        (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Definition::Action::Executable>>(Definition::Action::Executable::AddPreExpression)]))(parameters);
    //        tclInterpreter.userInteraction().addNewInfo();
    //        if(tclInterpreter.isError()){
    //            return Error::Error;
    //        }

    //        return Error::NoError;
    //    }
        // End of Functions ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Interface ----------------------------------------------------------------------------
        Error createCallAndMoveLastParameterToOne(Stat);
        Error performRulesCheckForNewParameter();
        Error onArgumentProcedureCheck();
        Error dynamicRulesCheck();
        Stat processingStat()const;//{return tclInterpreter.processingStat();}
        bool backslashSubbingActive = true;

    public:
        Error processCallsForError();
        Error performDynamicRulesCheck();
        //Error createCall(Stat, Call::Parameter&&);
        Error createCall(Stat, Call::Parameter&& = Call::Parameter());
        void createCallInIgnoreMode(Stat);
        void removeCallInIgnoreMode();
        Error addNewParameter();
        Error addNewParameter(QString);
        Error addNewParameter(Call& call);
        Error addNewParameter(Stat);
        Error addNewParameter(Stat, QString , OutputCommand = OutputCommand());
        Error addFinalizedCallParameter();
        void startVariableSubbing();
        bool isVariableSubbingProcessingJustActivated();
        Error processVariableSubbing();
        bool isFirstSignOk(QString str);
        //Error performVariableSubbingRecoveryProcedure(){};
        Error prepareSnprintf();
        Error processBackslashSubbing();
        inline void activateBackslashSubbing(){backslashSubbingActive = true;}
        inline void deactivateBackslashSubbing(){backslashSubbingActive = false;}

        Error finalizeCall();
        Calls::size_type ignoreModeIndex()const{
            return ignoreModeActiveIndex;
        }
        void moveIgnore(){
            ignoreModeActiveIndex--;
            removeCallInIgnoreMode();
        }

        void removeIgnore(){
            deactivateIgnoreMode();
            if(not procedureCalls.isEmpty())
                removeCallInIgnoreMode();
            updateCurrentCallProcedures();
        }

        Controller(TCLInterpreter& tclInterpreter, UserInputConfig& userConfig);

        inline bool isMainScript()const{
            return procedureCalls.isEmpty();
        }

        inline bool isNotCommandSubbingMainScriptOnly()const{
            Q_ASSERT_X(not procedureCalls.isEmpty(), "isNotCommandSubbingMainScriptOnly", "Method called for empty procedure calls list");
            return procedureCalls.size() == 1; // prelast call is Script
        }

        inline bool isNotCommandSubbingInComplexWord()const{
            Q_ASSERT_X(not procedureCalls.isEmpty(), "isNotCommandSubbingInComplexWord", "Method called for empty procedure calls list");
            return procedureCalls.size() == 2 or (procedureCalls.rbegin() + 2)->stat() == Stat::Script; // prex2last call is Script
        }

        inline bool isNotCommandSubbing()const{
            Q_ASSERT_X(not procedureCalls.isEmpty(), "isNotCommandSubbing", "Method called for empty procedure calls list");
            return procedureCalls.size() == 1 or (procedureCalls.rbegin() + 1)->stat() == Stat::Script; // prelast call is Script
        }

        inline bool isCommandSubbingCall()const{
            return not isNotCommandSubbing();
        }

        inline void updateCurrentCallProcedures(){
            currentCommandCallFunctions = callConfig.controlFunctionsForStat((procedureCalls.isEmpty())? Stat::Script : lastProcedureCall().stat());
        }

        inline void updateCurrentCallProcedures(Stat stat){
            currentCommandCallFunctions = callConfig.controlFunctionsForStat(stat);
        }


        // WriteOnlyProcedures
        void tryToActivateWriteOnlyProcedure(Call::Name& name);
        //inline bool isWriteOnlyProcedureActive()const{return writeOnlyProcedureActiveIndex != -1;}
        inline void tryToDeactivateWriteOnlyProcedure(){
            if(writeOnlyProcedureActiveIndex == procedureCalls.size()){
                writeOnlyProcedureActiveIndex = -1;
                deactivateWriteOnlyProcedureMode();
            }
        }
        // ---------------------------
        void activateWriteOnlyProcedureMode();
        void deactivateWriteOnlyProcedureMode();        
        inline void activateIgnoreMode(){
            ignoreModeActiveIndex = numberOfProcedureCalls();
        }
        inline void deactivateIgnoreMode(){
            ignoreModeActiveIndex = -1;
        }
        inline bool isIgnoreModeActive(){
            return ignoreModeActiveIndex != -1;
        }

        template<Settings::InterpreterMode>
        Error callDefinition_mode(Call::Name name);
        template<Settings::InterpreterMode>
        Error finalizeCall_mode();

        inline Error callDefinition(Call::Name name){return (this->*callDefinitionFunction)(name);}
        inline Error finalizeProcedureCall(){return (this->*finalizeCallFunction)();}
        inline Error interpret(){
            return (this->*(currentCommandCallFunctions->interpretCall))();
        }
        inline Error newParameterProcessing(){
            return (this->*(currentCommandCallFunctions->newParameterProcessing))();
        }
        inline Error newCallProcessing(){
            return (this->*(currentCommandCallFunctions->newCall))();
        }
        inline Error finalizeCallProcessing(){
            return (this->*(currentCommandCallFunctions->finalizeCall))();
        }
        inline Error constructor(){
            return (this->*(currentCommandCallFunctions->constructor))();
        }
        inline Error newParameter(){
            return (this->*(currentCommandCallFunctions->newParameter))();
        }
        inline Error destructor(){
            return (this->*(currentCommandCallFunctions->destructor))();
        }
        template<Stat>  // Special Command Call Stat
        Error interpret_mode();
        template<Stat>  // New Parameter Processing Call Stat
        Error newParameterProcessing_mode();
        template<Stat>  // Special Command Call Stat
        Error newCall_mode();
        template<Stat>  // New Parameter Call Stat
        Error finalizeCall_mode();
        template<Stat>  // Constructor Call Stat
        Error constructor_mode();
        template<Stat>  // New Parameter Call Stat
        Error newParameter_mode();
        template<Stat>  // Destructor Call Stat
        Error destructor_mode();
        // Special case for Stat::Size
        Error interpretSpecialCommandCall_throwErrorForWrongStat(){
            return throwError("TclProcedureInterpreter_Internal: Wrong stat used for command call interpet methods");
        }

        inline Calls::size_type numberOfProcedureCalls()const{return procedureCalls.size();}
        inline Call& lastProcedureCall(){return procedureCalls.last();}
        inline Call& preLastProcedureCall(){return *(procedureCalls.rbegin() + 1);}

        // Action Functions
        QStringList::size_type createAndAssignString(QString&, QStringList);
        QString toString(SavedStat&, Definition::Format::Target);
        // ----

        // Function to create ConditionalAction compare
        static const Definition::Rule::ConditionalActions::value_type newCompareRule(const QStringList stringsToCompare, const QStringList format) ;
        // End of Interface |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        static void addDefaultProcedureDefinitionsToUserProcedureDefintions(UserInputConfig& userDefinitions);

        bool isPredefinitionMode();

    };
};

#endif // TCLPROCEDURESINTERPRETER_HPP
