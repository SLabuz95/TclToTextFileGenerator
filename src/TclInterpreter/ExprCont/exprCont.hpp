#ifndef EXPRCONT_HPP
#define EXPRCONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"

namespace Tcl::Interpreter::Command{
    class Controller;
    class Call;
}

namespace Tcl::Interpreter::Command::ExprController{
    using namespace Tcl::Interpreter::Core;
    // Replacer Call controller
    // Call rules to eventually modify behaviour of interpreter
    // - Usable only in CommandSubbing calls
    // - Use only replacer actions
    // - check result and proceed
    class Controller{
    public:
        Controller()
        {}
        using CommandController = Tcl::Interpreter::Command::Controller;
        using CommandCall = Tcl::Interpreter::Command::Call;


    protected:

    };

}

#endif // EXPRCONT_HPP
