#include"callReplacerCont.hpp"
#include"TclInterpreter/CommandsCont/commandsCont.hpp"

using namespace Tcl::Interpreter::Command::CallReplacer;
using Error = Tcl::Interpreter::Core::Error;

Error Controller::performReplacerRuleCheckForNewCall(CommandController& commandController, ControllerData& controllerData){
    if(commandController.numberOfProcedureCalls() > 0 and
            commandController.lastProcedureCall().stat() == Stat::CommandSubbing)
    {
        // 1. Set State in replacerController
        // 2. Peform Dynamic Rule Check
        // 3. Check if parameter stat changed
        data_ = ControllerData{controllerData.stat()};

        if(commandController.performDynamicRulesCheck() == Error::Error){
            return Error::Error;
        }

        controllerData = data_;
    }
    return Error::NoError;
}
