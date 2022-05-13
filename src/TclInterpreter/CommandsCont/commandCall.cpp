//#include"commandCall.hpp"
#include"TclInterpreter/ErrorCont/errorCont.hpp"
#include"commandsCont.hpp"

using namespace Tcl::Interpreter::Command;
using namespace Tcl::Interpreter;

Call::Call(Stat stat, Parameter& parameter)
    : stat_(stat), _parameters(1, parameter)
{

}

Error Call::newParameter(Stat stat,QString rawParameter, OutputCommand outputCommand){
    rawCommand().append(lastParameter().toString(Definition::Format::Target::TclFormat) + CallConfig::parameterSeparators(stat));
    _parameters.append({stat, outputCommand, rawParameter});
    return Error::NoError;
}
