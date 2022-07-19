#include"callReplacerCont.hpp"
#include"TclInterpreter/CommandsCont/commandsCont.hpp"

using namespace Tcl::Interpreter::Command::CallReplacer;
using Error = Tcl::Interpreter::Core::Error;

Error Controller::performReplacerRuleCheckForNewCall(CommandController& commandController, ControllerData& controllerData){
    if(commandController.numberOfProcedureCalls() > 0 and
            commandController.lastProcedureCall().stat() == Stat::CommandSubbing)
    {
        // ! Activate - In ex. IsLastSavedStat action for active callReplacer compare argument with CallReplacer stat otherwise with lastProcedureParameter
        active = true;
        // 1. Set State in replacerController
        // 2. Peform Dynamic Rule Check
        // 3. Check if parameter stat changed
        // ! Dectivate - In ex. IsLastSavedStat action for active callReplacer compare argument with CallReplacer stat otherwise with lastProcedureParameter
        data_ = ControllerData{controllerData.stat()};

        if(commandController.performDynamicRulesCheck() == Error::Error){
            active = false;
            return Error::Error;
        }

        controllerData = data_;
        active = false;
    }
    return Error::NoError;
}
