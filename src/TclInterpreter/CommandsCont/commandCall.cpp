#include"commandCall.hpp"
#include"TclInterpreter/ErrorCont/errorCont.hpp"

using namespace Tcl::Interpreter::Command;
using namespace Tcl::Interpreter;

Call::Call(Stat stat, Parameter& parameter)
    : stat_(stat), _parameters(1, parameter)
{

}

