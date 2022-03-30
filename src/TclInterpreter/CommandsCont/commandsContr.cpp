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


template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(Call::Name name){
    using Definition = CommandDefinitions::Iterator;
    Definition definition;
    for(definition = procedureDefinitions.begin(); definition < procedureDefinitions.end(); definition++){
        if(definition->name == name)
            break;
    }
    try {
        if(definition == procedureDefinitions.end()){
            procedureCalls.append(Call(name));
        }else{
            procedureCalls.append(Call(definition));
        }
    }  catch (std::exception& e) {
        return throwError(e.what());
    }

    return Error::NoError;
}
template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCaseReport>(Call::Name name){
    tryToActivateWriteOnlyProcedure(name);
    return newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

template<>
Error TclProcedureInterpreter::newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::PredefinitionsOnly>(Call::Name name){
    return newProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

const TclProcedureInterpreter::NewProcedureCallFunction
TclProcedureInterpreter::ProcedureCallFunctions::newProcedureCalls
[TclProcedureInterpreter::ProdecuresSettings::mode2number(Settings::InterpreterMode::NumbOfModes)] =
{
    &TclProcedureInterpreter::newProcedureCall_mode<Settings::InterpreterMode::TestCase>,
    &TclProcedureInterpreter::newProcedureCall_mode<Settings::InterpreterMode::TestCaseReport>,
    &TclProcedureInterpreter::newProcedureCall_mode<Settings::InterpreterMode::PredefinitionsOnly>,
};


template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(SavedStat &statCommand){
    using ProcedureDefinition = Command::Definition;
    using RulesForArguments = ProcedureDefinition::RulesForArguments;
    using RulesForArgument = ProcedureDefinition::RulesForArguments::Iterator;
    using Rules = ProcedureDefinition::Rules;
    using Rule = ProcedureDefinition::Rules::Iterator;

    const QString ERROR_PREFIX = "TclProcedureInterpreter::finalizeProcedureCall: ";
    command.clear();
    if(procedureCalls.isEmpty()){
        return throwError("No Procedure Calls for Finalize");
    }
    // User Interaction Section ---------------------------
    if(procedureCalls.last().isUserInteractionRequired()){
        if(addPreExpressionForUserInteraction() == Error::Error){
            return throwError(ERROR_PREFIX + tclInterpreter.error());
        }
    } // -----------------------------------------------------
    else{
        finalizeOn = true;
        Call& procedureCall = procedureCalls.last();

        Definition::RulesOnEndOfCall& rulesOnEndOfCall = (procedureCall.rulesOnEndOfCall().isEmpty())? Definition::defaultUnknownProcedureDefinition.rulesOnEndOfProcedureCall : procedureCall.rulesOnEndOfCall();
        bool ruleCondtionsPassed = false;

        using Actions = Rules::value_type::ExecutableActions;
        using Action = Actions::Iterator;
        for(Rule rule = rulesOnEndOfCall.begin(); rule < rulesOnEndOfCall.end(); rule++){
            using Conditions = Rules::value_type::ConditionalActions;
            using Condition = Conditions::Iterator;
            Condition condition;
            ConditionResult conditionResult = ConditionResult::Satisfied;
            for(condition = rule->conditions.begin(); condition < rule->conditions.end(); condition++){
                conditionResult = (this->*(conditionalInterpreterFunctions[static_cast<std::underlying_type_t<Conditions::value_type::ActionType>>(condition->type())]))(condition->parameters());
                if(tclInterpreter.isError()){
                    finalizeOn = false;
                    return throwError(ERROR_PREFIX + tclInterpreter.error());
                }
                if(conditionResult == ConditionResult::Unsatisfied){
                    break;
                }
            }
            if(condition == rule->conditions.end()){
                ruleCondtionsPassed = true;
                for(Action action = rule->actions.begin(); action < rule->actions.constEnd(); action++){
                    (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Actions::value_type::ActionType>>(action->type())]))(action->parameters());
                    if(tclInterpreter.isError()){
                        finalizeOn = false;
                        return throwError(ERROR_PREFIX + tclInterpreter.error());
                    }
                }
                switch(rule->controlFlag){
                //case Rules::value_type::Control::BreakRuleCheckDontExecOnEndActions:
                case Rules::value_type::Control::BreakRuleCheck:
                    rule = rulesOnEndOfCall.end();
                    break;
                case Rules::value_type::Control::NoBreakRuleCheck:
                    ruleCondtionsPassed = false;
                    break;
                default:
                    break;
                }
            }
        }
        if(not ruleCondtionsPassed){
            tclInterpreter.functionDefinitions.addDefinitionNotSatisfiedRules(procedureCall);
        }
    }
    statCommand.setCommand(command);
    finalizeOn = false;

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCaseReport>(SavedStat &){
    finalizeOn = false;

    return Error::NoError;

}

template<>
Error TclProcedureInterpreter::finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::PredefinitionsOnly>(SavedStat &statCommand){

    return finalizeProcedureCall_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(statCommand);

}

const TclProcedureInterpreter::FinalizeProcedureCallFunction
TclProcedureInterpreter::ProcedureCallFunctions::finalizeProcedureCalls
[TclProcedureInterpreter::ProdecuresSettings::mode2number(Settings::InterpreterMode::NumbOfModes)] =
{
    &TclProcedureInterpreter::finalizeProcedureCall_mode<Settings::InterpreterMode::TestCase>,
    &TclProcedureInterpreter::finalizeProcedureCall_mode<Settings::InterpreterMode::TestCaseReport>,
    &TclProcedureInterpreter::finalizeProcedureCall_mode<Settings::InterpreterMode::PredefinitionsOnly>,
};

//
template<>
Error TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::CommandSubbing>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){
    case Stat::Comment:
    {
        // Comment Control required
        // Finialize Procedure call or threat as word
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
    }
        Q_FALLTHROUGH();
    case Stat::Word:
    case Stat::BracesStart:
    case Stat::Braces:
    case Stat::DoubleQuotes:
    case Stat::Whitespace:
    case Stat::VariableSubbingStart:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
    }
        break;
    case Stat::CommandSubbingEnd:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
    }
        break;
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(Stat parameterStat){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(lastProcedureCall().rawParametersLength()){
    // 1. Is number of parameters 0 (no parameter indicates that no procedure name has been defined - new parameter will create parameter for procedure name)
    // States allowed for procedure name:
    // - Word,
    // - ComplexWord (only BackslashSubbing, VariableSubbing (if system ready)),
    // - DoubleQuotes (only BackslashSubbing, VariableSubbing (if system ready)),
    // - BracesStart
    case 0:
    {
        switch(parameterStat){
        case Stat::Word:
            unknownString = tclInterpreter.readCurrentKeyword();
        Q_FALLTHROUGH();
        //case Stat::ComplexWord: "BackslashSubbing or VariableSubbing initialize new procedure ComplexWord"
        case Stat::DoubleQuotes:
        case Stat::BracesStart:
        {
            lastProcedureCall().newParameter(parameterStat, unknownString);
        }
            break;
        default:
            return ErrorController::throwError(ERROR_PREFIX + "Forbidden Stat");
        }
    }
        break;
    // 2. Is number of parameters 1 (1 parameter indicates that new parameter will be second parameter - first parameter is "ready" as procedure name - name for procedure definitions can be assigned)
    case 1:
    {
        switch(parameterStat){
        case Stat::Word:
        {
            unknownString = tclInterpreter.readCurrentKeyword();
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
        }
            break;

        }
    }
        break;
    default:
        switch(parameterStat){
        case Stat::Word:
        {
            unknownString = tclInterpreter.readCurrentKeyword();
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
        }
            break;

        }
    }


    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::BracesStart>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::BracesStart>: ";

    switch(processingStat){
    case Stat::BracesStart:
        // List Control required
    case Stat::Comment:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::Whitespace:
    case Stat::VariableSubbingStart:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
    case Stat::EndOfString:
    case Stat::Semicolon:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::Braces:
    {
        // End of Braces
        // List Control required
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::DoubleQuotes>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::DoubleQuotes>: ";

    switch(processingStat){
    case Stat::BracesStart:
    case Stat::Braces:
    case Stat::Comment:
    case Stat::Word:
    case Stat::Whitespace:
    case Stat::VariableSubbingStart:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
    case Stat::EndOfString:
    case Stat::Semicolon:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::DoubleQuotes:
    {
        // End of Double Quotes
        // List Control required
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::ComplexWord>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::ComplexWord>: ";

    switch(processingStat){
    case Stat::Comment:
    {
        // Comment Control required
        // Finialize Procedure call or threat as word
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
    }
    case Stat::BracesStart:
    case Stat::Braces:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::VariableSubbingStart:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
    {
        // End of Complex Word
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Expression>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Expression>: ";

    switch(processingStat){
    case Stat::Comment:
    {
        // Comment Control required
        // Finialize Procedure call or threat as word
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
    }
    case Stat::BracesStart:
    case Stat::Braces:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::VariableSubbingStart:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
    {
        // End of Complex Word
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Script>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Script>: ";

    switch(processingStat){
    // Like in Main Script
    case Stat::Comment:
    {
        // Just comment like in MainScript
    }
        break;
    case Stat::BracesStart:
    case Stat::Braces:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::VariableSubbingStart:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
    {
        // End of Complex Word
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}



template<Stat>  // Special Command Call Stat
Error interpretSpecialCommandCall_mode(const Stat);

const TclProcedureInterpreter::ProcedureCallFunctions::CommandCallControlFunctions
TclProcedureInterpreter::ProcedureCallFunctions::commandCallSpecialFunctions
[Tcl::Interpreter::Core::numbOfSpecialCommandCallsAndSafeguard()] =
{
    //CommandSubbing
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::CommandSubbing>,
    },
    //BracesStart
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::BracesStart>,
    },
    //DoubleQuotes
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::DoubleQuotes>,
    },
    //ComplexWord
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::ComplexWord>,
    },
    //Expression
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::Expression>,
    },
    //Script
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Script>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::Script>,
    },
    // SafeGuard
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat
    },
};
/*
const TclProcedureInterpreter::CommandCallSpecialInterpretFunction
TclProcedureInterpreter::ProcedureCallFunctions::commandCallSpecialInterprets
[Tcl::Interpreter::Core::numbOfSpecialCommandCallsAndSafeguard()] =
{
    &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Script>,
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat
};
*/
