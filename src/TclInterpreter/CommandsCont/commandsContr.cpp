#include"TclInterpreter/CommandsCont/commandsCont.hpp"
#include"TclInterpreter/tclToCAPL.hpp"
#include"Tcl2Capl/controller.hpp"

using namespace Tcl;

Command::Controller::Controller(TCLInterpreter& tclInterpreter, UserInputConfig& userConfig) :
    tclInterpreter(tclInterpreter),
    procedureDefinitions(
        (userConfig.userProcedureConfig().isEmpty())?
            defaultProcedureDefinitions
          : userConfig.userProcedureConfig()),
    unknownProcedureDefinition(
        (userConfig.userDefaultProcedureConfig().isRulesEmpty())?
            Definition::defaultUnknownProcedureDefinition
          : userConfig.userDefaultProcedureConfig()),
    newProcedureCallFunction(ProcedureCallFunctions::newCallAt(userConfig.proceduresSettings().mode())),
    finalizeProcedureCallFunction(ProcedureCallFunctions::finalizeCallAt(userConfig.proceduresSettings().mode()))
{}

Error Controller::addPreExpressionForUserInteraction()
{
    ExecutableActionsParameters parameters =
    {
      QString("// _UI_") + QString::number(tclInterpreter.userInteraction.nextUserInteractionNumber()) + " ",
      Definition::Format::FORMAT_RULE_CALL(),
      "=",
      " ",
      Definition::Format::FORMAT_RULE_CALL(),
       "@ ",
      Definition::Format::FORMAT_RULE_CALL(),
        ">0"
    };
    command = QString("/* _UI_") + QString::number(tclInterpreter.userInteraction.nextUserInteractionNumber()) + " */";
    (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Definition::Action::Executable>>(Definition::Action::Executable::AddPreExpression)]))(parameters);
    tclInterpreter.userInteraction.addNewInfo();
    if(tclInterpreter.isError()){
        return Error::Error;
    }

    return Error::NoError;
}

void Controller::activateWriteOnlyProcedureMode()
{
    finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);
    tclInterpreter.activateWriteOnlyProcedureMode();
}

void Controller::deactivateWriteOnlyProcedureMode()
{
    finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCaseReport);
    tclInterpreter.deactivateWriteOnlyProcedureMode();
}
