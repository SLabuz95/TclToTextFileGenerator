#ifndef TCLPROCEDURESINTERPRETER_HPP
#define TCLPROCEDURESINTERPRETER_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include<QXmlStreamWriter>
#include"TclInterpreter/SavedStatCont/savedStatCont.hpp"
#include"TclInterpreter/UserInteractionCont/userInteractionCont.hpp"
#include<QMap>
#include"TclInterpreter/ErrorCont/errorCont.hpp"
#include"commandCall.hpp"

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
        class ProdecuresSettings;
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
        // ---------

        static QMap<const QString,  UserInteractionStatus> userInteractionMap;

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

            constexpr static std::underlying_type_t<Stat> specialCallStat2number(const Stat stat){
                // If stat other then CommandSubbing or related, stat = Stat::Size
                // For Stat::Size, special interpret method pointer should be added at the end of the interpret methods list
                // That interpret method will generate error message for wrong stat interpret call
                return (stat < Stat::CommandSubbing)? // Error Case
                    static_cast<std::underlying_type_t<Stat>>(Stat::Size) - static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing)
                :    // Correct Case
                    static_cast<std::underlying_type_t<Stat>>(stat) - static_cast<std::underlying_type_t<Stat>>(Stat::CommandSubbing);
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
        using ConditionalActionsParameters = const Definition::Rule::ActionCall<Definition::Action::Conditional>::Parameters&;
        using ExecutableActionsParameters = const Definition::Rule::ActionCall<Definition::Action::Executable>::Parameters&;

        using ConditionInterpretFunction = ConditionResult (Controller::*)(ConditionalActionsParameters);
        using ExecutableInterpretFunction = void (Controller::*)(ExecutableActionsParameters);
        using ConditionInterpretFunctions = ConditionInterpretFunction[static_cast<std::underlying_type_t<Definition::Action::Conditional>>(Definition::Action::Conditional::Size)];
        using ExecutableInterpretFunctions = ExecutableInterpretFunction[static_cast<std::underlying_type_t<Definition::Action::Executable>>(Definition::Action::Executable::Size)];

        // ----
        using FunctionDefinitionInfo = QMap<uint, QStringList>; // <Numb of Parameters, Examples/ Occurencies>
        using FunctionDefinitions = QMap<QString, FunctionDefinitionInfo>;    // <Name, Info>
        using ProdecuresSettings = ProdecuresSettings;

        using NewProcedureCallFunction =  Error (Controller::* const)(Call::Name);
        using FinalizeProcedureCallFunction = Error (Controller::*)(SavedStat&);
        using CommandCallSpecialInterpretFunction = Error (Controller::*)(const Stat);
        using CommandCallSpecialNewParameterFunction = Error (Controller::*)(const Stat);
        //inline Error emptyCommandCallSpecialInterpret(const Stat){return Error::NoError;}
        // ----

    private:
        // End of Concept Definition |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

        class ProcedureCallFunctions{
    public:
        class CommandCallControlFunctions{
        public:
            //NewProcedureCallFunction newProcedureCall;
            //FinalizeProcedureCallFunction finalizeProcedureCall;
            CommandCallSpecialInterpretFunction interpretCall;
            CommandCallSpecialNewParameterFunction newParameterCall;
        };

        //static const CommandCallSpecialInterpretFunction commandCallSpecialInterprets[];
        static const CommandCallControlFunctions commandCallSpecialFunctions[];

        // Mode Dependent
        static const NewProcedureCallFunction newProcedureCalls[];
        static const FinalizeProcedureCallFunction finalizeProcedureCalls[];


            using Settings = Controller::ProdecuresSettings;

            static inline NewProcedureCallFunction newCallAt(const Settings::InterpreterMode mode){
                return newProcedureCalls[Settings::mode2number(mode)];
            }
            static inline FinalizeProcedureCallFunction finalizeCallAt(const Settings::InterpreterMode mode){
                return finalizeProcedureCalls[Settings::mode2number(mode)];
            }

            /*static inline CommandCallSpecialInterpretFunction interpretCallAt(const Stat stat){
                return commandCallSpecialInterprets[Settings::specialCallStat2number(stat)];
            }
            /*
            static inline CommandCallSpecialInterpretFunction newParameterAt(const Stat stat){
                return commandCallSpecialInterprets[Settings::specialCallStat2number(stat)];
            }*/
        };

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
        NewProcedureCallFunction newProcedureCallFunction = ProcedureCallFunctions::newCallAt(ProdecuresSettings::InterpreterMode::TestCase);
        FinalizeProcedureCallFunction finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);
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

        ProcedureCallFunctions::CommandCallControlFunctions const*
        currentCommandCallFunctions = &ProcedureCallFunctions::
                commandCallSpecialFunctions[ProdecuresSettings::specialCallStat2number(Stat::Size)];
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
    public:
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

        inline Error newProcedureCall(Call::Name name){return (this->*newProcedureCallFunction)(name);}

        template<Controller::ProdecuresSettings::InterpreterMode>
        Error newProcedureCall_mode(Call::Name name);

        inline Error removeProcedureCall(){return procedureCalls.isEmpty()? throwError("TclProcedureInterpreter_Internal: No procedure to remove") :
                                                                            (procedureCalls.removeLast(), Error::NoError);}
        // -- !!!        
        inline Error interpret(const Stat processingStat){
            return (this->*(currentCommandCallFunctions->interpretCall))(processingStat);
        }
        // ---
        Error nextArgument(){}
        Error nextArgumentForSnprintf_priv(Stat stat);

        inline Error finalizeProcedureCall(SavedStat& savedStat){return (this->*finalizeProcedureCallFunction)(savedStat);}

        template<Controller::ProdecuresSettings::InterpreterMode>
        Error finalizeProcedureCall_mode(SavedStat&);

        template<Stat>  // Special Command Call Stat
        Error interpretSpecialCommandCall_mode(const Stat);
        template<Stat>  // New Parameter Call Stat
        Error newParameterSpecialCommandCall_mode(const Stat);
        // Special case for Stat::Size
        Error interpretSpecialCommandCall_throwErrorForWrongStat(const Stat){
            return throwError("TclProcedureInterpreter_Internal: Wrong stat used for command call interpet methods");
        }

        /*static FinalizeProcedureCallFunction finalizeProcedureCalls[
        std::underlying_type_t<Controller::ProdecuresSettings::InterpreterMode>
        (Controller::ProdecuresSettings::InterpreterMode::NumbOfModes)];*/

       // Error finalizeSnprintfCall(ProcedureCall::Parameter&);
        Error dynamicProcedureCheck();
        Error onArgumentProcedureCheck();
        Error dynamicProcedureArgumentCheck_priv();
        Error onArgumentProcedureCheck_priv();
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