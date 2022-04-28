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
        inline static UserInteractionStatus fromStr(QString& str){return userInteractionMap.value(str.toLower(), UserInteractionStatus::None);}
        inline static QString toStr(UserInteractionStatus userInteraction){
            return userInteractionMap.key(userInteraction); // Empty String == Not found
        }
     private:
        // Aliases
        inline static Error throwError(){return ErrorController::throwError();}
        inline static Error throwError(const QString str){return ErrorController::throwError(str);}
        inline static const QString& error(){return ErrorController::error();}
        // ---------

        static QMap<const QString,  UserInteractionStatus> userInteractionMap;

        // ----

        enum ConditionResult : bool{Satisfied, Unsatisfied};
        // ----

    public:
        using ConditionalActionsParameters = const Definition::Rule::ActionCall<Definition::Action::Conditional>::Parameters&;
        using ExecutableActionsParameters = const Definition::Rule::ActionCall<Definition::Action::Executable>::Parameters&;

        using ConditionInterpretFunction = ConditionResult (Controller::*)(ConditionalActionsParameters);
        using ExecutableInterpretFunction = void (Controller::*)(ExecutableActionsParameters);
        using ConditionInterpretFunctions = ConditionInterpretFunction[static_cast<std::underlying_type_t<Definition::Action::Conditional>>(Definition::Action::Conditional::Size)];
        using ExecutableInterpretFunctions = ExecutableInterpretFunction[static_cast<std::underlying_type_t<Definition::Action::Executable>>(Definition::Action::Executable::Size)];

        // ----
        using FunctionDefinitionInfo = QMap<uint, QStringList>; // <Numb of Parameters, Examples/ Occurencies>
        using FunctionDefinitions = QMap<QString, FunctionDefinitionInfo>;    // <Name, Info>

        //inline Error emptyCommandCallSpecialInterpret(const Stat){return Error::NoError;}
        // ----

    private:
        // End of Concept Definition |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

        // Objects -------------------------------------------------------------------------------
        TCLInterpreter& tclInterpreter;
        CommandDefinitions& procedureDefinitions;
        Definition& unknownProcedureDefinition;
        static CommandDefinitions defaultProcedureDefinitions;
        OutputCommand command;
        Calls procedureCalls;
        Call::Parameters rawParameterStats;
        //UserInteraction& userInteraction;
        //Preexpressions& preexpressions;

        bool finalizeOn = false;
        //NewProcedureCallFunction newProcedureCallFunction = ProcedureCallFunctions::newCallAt(ProdecuresSettings::InterpreterMode::TestCase);
        //FinalizeProcedureCallFunction finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);

        //CommandCallSpecialInterpret commandCallSpecialInterpret = &Controller::emptyCommandCallSpecialInterpret;

        Calls::size_type writeOnlyProcedureActiveIndex = -1;

        //Command command;

        static ConditionInterpretFunctions conditionalInterpreterFunctions;
        static ExecutableInterpretFunctions executableInterpretFunctions;

        Call::SquareBracketLevel squareBracketsLevel = 0;

        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        // Functions -----------------------------------------------------------------------------

        template<Definition::Rule::ConditionalActions::value_type::ActionType stat>
        ConditionResult executeConditionalAction(ConditionalActionsParameters);
        template<Definition::Rule::ExecutableActions::value_type::ActionType stat>
        void executeAction(ExecutableActionsParameters);

        Error addPreExpressionForUserInteraction();


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

        inline Stat processingStat()const;//{return tclInterpreter.processingStat();}
    public:
        //Error createCall(Stat, Call::Parameter&&);
        Error createCall(Stat, Call::Parameter&& = Call::Parameter());
        Controller(TCLInterpreter& tclInterpreter, UserInputConfig& userConfig);

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
    //    {
    //        finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);
    //        tclInterpreter.activateWriteOnlyProcedureMode();
    //    }

        void deactivateWriteOnlyProcedureMode();
    //   {
    //        finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCaseReport);
    //        tclInterpreter.deactivateWriteOnlyProcedureMode();
    //    }
/*
        inline Error newProcedureCall(Call::Name name){return (this->*newProcedureCallFunction)(name);}
*/
        template<Settings::InterpreterMode>
        Error callDefinition_mode(Call::Name name);
        template<Settings::InterpreterMode>
        Error finalizeCall_mode(SavedStat&);
/*
        inline Error removeProcedureCall(){return procedureCalls.isEmpty()? throwError("TclProcedureInterpreter_Internal: No procedure to remove") :
                                                                     (procedureCalls.removeLast(), Error::NoError);}
        // -- !!!        


        // ---
        Error nextArgument(){}
        Error nextArgumentForSnprintf_priv(Stat stat);

        inline Error finalizeProcedureCall(SavedStat& savedStat){return (this->*finalizeProcedureCallFunction)(savedStat);}


*/
        inline Error interpret(){
            return (this->*(currentCommandCallFunctions->interpretCall))();
        }

        inline Error newParameterProcessing(){
            return (this->*(currentCommandCallFunctions->newParameterCall))();
        }

        inline Error newCallProcessing(){
            return (this->*(currentCommandCallFunctions->newCall))();
        }

        inline Error finalizeCallProcessing(){
            return (this->*(currentCommandCallFunctions->finalizeCall))();
        }


        template<Stat>  // Special Command Call Stat
        Error interpretSpecialCommandCall_mode();
        template<Stat>  // New Parameter Call Stat
        Error newParameterSpecialCommandCall_mode();
        template<Stat>  // Special Command Call Stat
        Error newCallSpecialCommandCall_mode();
        template<Stat>  // New Parameter Call Stat
        Error finalizeCallSpecialCommandCall_mode();
        // Special case for Stat::Size
        Error interpretSpecialCommandCall_throwErrorForWrongStat(){
            return throwError("TclProcedureInterpreter_Internal: Wrong stat used for command call interpet methods");
        }
/*
        static FinalizeProcedureCallFunction finalizeProcedureCalls[
        std::underlying_type_t<Controller::ProdecuresSettings::InterpreterMode>
        (Controller::ProdecuresSettings::InterpreterMode::NumbOfModes)];

       // Error finalizeSnprintfCall(ProcedureCall::Parameter&);
        Error dynamicProcedureCheck();
        Error onArgumentProcedureCheck();
        Error dynamicProcedureArgumentCheck_priv();
        Error onArgumentProcedureCheck_priv();
*/
        const QString lastProcedureName()const{return ((procedureCalls.isEmpty())? QString() : procedureCalls.last().name());}
        inline Calls::size_type numberOfProcedureCalls()const{return procedureCalls.size();}
        inline Call& lastProcedureCall(){return procedureCalls.last();}
        inline Call& prelastProcedureCall(){return *(procedureCalls.end() - 2);}

        // Action Functions
        QStringList::size_type createAndAssignString(QString&, QStringList);
        QString toString(SavedStat&, Definition::Format::Target);
        // ----

        // Function to create ConditionalAction compare
        static const Definition::Rule::ConditionalActions::value_type newCompareRule(const QStringList stringsToCompare, const QStringList format) ;
        // End of Interface |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
        static void addDefaultProcedureDefinitionsToUserProcedureDefintions(UserInputConfig& userDefinitions);

    };
};

#endif // TCLPROCEDURESINTERPRETER_HPP
