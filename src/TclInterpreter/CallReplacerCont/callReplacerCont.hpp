#ifndef CALLREPLACERCONT_HPP
#define CALLREPLACERCONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"

namespace Tcl::Interpreter::Command{
    class Controller;
    class Call;
}

namespace Tcl::Interpreter::Command::CallReplacer{
    using namespace Tcl::Interpreter::Core;
    // Replacer Call controller
    // Call rules to eventually modify behaviour of interpreter
    // - Usable only in CommandSubbing calls
    // - Use only replacer actions
    // - check result and proceed
    class Controller{
    public:
        Controller()
            : data_(Stat::None)
        {}
        using CommandController = Tcl::Interpreter::Command::Controller;
        using CommandCall = Tcl::Interpreter::Command::Call;

        class ControllerData{
            ControllerData() = delete;
            Stat stat_ = Stat::None;
        public:
            ControllerData(Stat stat) : stat_(stat){}
            const Stat& stat()const{return stat_;}
            void changeStat(const Stat& newStat){stat_ = newStat;}

        };

        Error performReplacerRuleCheckForNewCall(CommandController&, ControllerData&);
        ControllerData& data(){return data_;}
        bool isActive()const{return active;}
    protected:
        ControllerData data_;
        bool active = false;
    };

}

#endif // CALLREPLACERCONT_HPP
