#ifndef COMMANDCALLCONFIG_HPP
#define COMMANDCALLCONFIG_HPP

#include "TclInterpreter/CommandsCont/commandsCont.hpp"
#include"TclInterpreter/SavedStatCont/savedStatCont.hpp"

namespace Tcl::Interpreter::Command{
    class Settings{
   public:
        using CallDefinitionInterModeFctPtr =  Error (Controller::* const)(Call::Name);
        using FinalizeCallInterModeFctPtr = Error (Controller::*)(SavedStat&);
        using WriteOnlyProcedures = QStringList;

        enum class InterpreterMode : quint8{
            TestCase,
            TestCaseReport,
            // No Write To File Modes
            NoWriteToFileModes,
            PredefinitionsOnly = NoWriteToFileModes,

            NumbOfModes
        };

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
        InterpreterMode mode_ = InterpreterMode::TestCase;

        static const CallDefinitionInterModeFctPtr callDefinitionInterModeCalls[];
        static const FinalizeCallInterModeFctPtr finalizeCallInterModeCalls[];

    public:
        static inline CallDefinitionInterModeFctPtr newCallAt(const Settings::InterpreterMode mode){
            return callDefinitionInterModeCalls[mode2number(mode)];
        }
        static inline FinalizeCallInterModeFctPtr finalizeCallAt(const Settings::InterpreterMode mode){
            return finalizeCallInterModeCalls[mode2number(mode)];
        }

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


    class CallConfig{

        using InterpretFctPtr = Error (Controller::*)();
        using NewParameterFctPtr = Error (Controller::*)();
        using NewCallFctPtr = Error (Controller::*)();
        using FinalizeCallFunctionFctPtr = Error (Controller::*)();

    public:
        class CommandCallControlFunctions{
        public:
            //NewProcedureCallFunction newProcedureCall;
            //FinalizeProcedureCallFunction finalizeProcedureCall;
            InterpretFctPtr interpretCall;
            NewParameterFctPtr newParameterCall;
            NewCallFctPtr newCall;
            FinalizeCallFunctionFctPtr finalizeCall;
        };
        static CommandCallControlFunctions const* controlFunctionsForStat(const Stat stat){
            return &commandCallSpecialFunctions[Settings::specialCallStat2number(stat)];
        }
    private:
        //static const CommandCallSpecialInterpretFunction commandCallSpecialInterprets[];
        static const CommandCallControlFunctions commandCallSpecialFunctions[];

            /*static inline CommandCallSpecialInterpretFunction interpretCallAt(const Stat stat){
                return commandCallSpecialInterprets[Settings::specialCallStat2number(stat)];
            }
            /*
            static inline CommandCallSpecialInterpretFunction newParameterAt(const Stat stat){
                return commandCallSpecialInterprets[Settings::specialCallStat2number(stat)];
            }*/

    };

}

#endif // COMMANDCALLCONFIG_HPP
