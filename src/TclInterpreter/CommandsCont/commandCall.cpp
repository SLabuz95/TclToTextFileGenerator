//#include"commandCall.hpp"
#include"TclInterpreter/ErrorCont/errorCont.hpp"
#include"commandsCont.hpp"

using namespace Tcl::Interpreter::Command;
using namespace Tcl::Interpreter;


Call::Call(Stat stat) : stat_(stat){}

Call::Call(Stat stat, Parameter& parameter, ProcedureDefinitionIterator definition , ParameterIndex index)
    : stat_(stat), _parameters(1, parameter), _procedureDefinition(definition), currentParameterIndex(index)
{

}

Error Call::newParameter(Stat stat,QString rawParameter, OutputCommand outputCommand){
    if(rawParametersLength() == 0)
    {
        //rawCommand().clear();
    }else{
        rawCommand().append(lastParameter().toString(Definition::Format::Target::TclFormat) + CallConfig::parameterSeparators(stat));
    }
    _parameters.append({stat, outputCommand, rawParameter});
    return Error::NoError;
}
