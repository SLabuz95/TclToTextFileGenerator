#include"TclInterpreter/CommandsCont/commandsCont.hpp"
#include"TclInterpreter/tclToCAPL.hpp"

using CommandsController = TCLCommandsController;

Error CommandsController::addPreExpressionForUserInteraction()
{
    ExecutableActionsParameters parameters =
    {
      QString("// _UI_") + QString::number(tclInterpreter.userInteraction.nextUserInteractionNumber()) + " ",
      ProcedureDefinition::Format::FORMAT_RULE_CALL(),
      "=",
      " ",
      ProcedureDefinition::Format::FORMAT_RULE_CALL(),
       "@ ",
      ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ">0"
    };
    command = QString("/* _UI_") + QString::number(tclInterpreter.userInteraction.nextUserInteractionNumber()) + " */";
    (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<ProcedureDefinition::Action::Executable>>(ProcedureDefinition::Action::Executable::AddPreExpression)]))(parameters);
    tclInterpreter.userInteraction.addNewInfo();
    if(tclInterpreter.isError()){
        return Error::Error;
    }

    return Error::NoError;
}

void CommandsController::activateWriteOnlyProcedureMode()
{
    finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);
    tclInterpreter.activateWriteOnlyProcedureMode();
}

void CommandsController::deactivateWriteOnlyProcedureMode()
{
    finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCaseReport);
    tclInterpreter.deactivateWriteOnlyProcedureMode();
}
