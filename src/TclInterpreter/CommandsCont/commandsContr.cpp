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
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return tclInterpreter.newProcedureCall(processingStat);
    }
        break;
    case Stat::CommandSubbingEnd:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
       //return tclInterpreter.finializeProcedureCall(processingStat);
    }
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
        return tclInterpreter.finializeProcedureCall(processingStat);
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
        case Stat::Comment:
            return throwError(ERROR_PREFIX + "Comment sign as first sign of procedure name");
        case Stat::Word:
        case Stat::Braces:
        case Stat::Namespace:
        case Stat::CommandSubbingEnd:
        {
            unknownString = tclInterpreter.readCurrentKeyword();
            lastProcedureCall().newParameter(Stat::Word, unknownString);
        }
            break;
        //case Stat::ComplexWord: "BackslashSubbing or VariableSubbing initialize new procedure ComplexWord"
        case Stat::DoubleQuotes:
        case Stat::BracesStart:
        case Stat::VariableSubbing:
        case Stat::BackslashSubbing:
        {
            //lastProcedureCall().newParameter(parameterStat, unknownString);
            return tclInterpreter.newProcedureCall(parameterStat);
        }
            break;        
        case Stat::Whitespace:
        {
            // Ignore whitespace before procedure name
        }
            break;

        default:
            return ErrorController::throwError(ERROR_PREFIX + "Forbidden Stat");
        }
    }
        break;
    // 2. Is number of parameters 1 (1 parameter indicates that new parameter will be second parameter - first parameter is "ready" as procedure name - name for procedure definitions can be assigned)
    default:
    {
        switch(parameterStat){
        case Stat::Word:
        case Stat::CommandSubbingEnd:
        case Stat::Braces:
        case Stat::Namespace:
        case Stat::Comment:
        {
            unknownString = tclInterpreter.readCurrentKeyword();
            if(lastProcedureCall().rawParameters().last().stat() == Stat::Word){
                lastProcedureCall().rawParameters().last().appendCommand(unknownString);
            }else{  // Any other stat is parameter without whitespace after parameter Ex. "abc"abc Expection [puts abc]abc - ComplexWord
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            }
        }
            break;
        case Stat::DoubleQuotes:
        case Stat::BracesStart:
        case Stat::VariableSubbing:
        case Stat::BackslashSubbing:
        {
            //lastProcedureCall().newParameter(parameterStat, unknownString);
            return tclInterpreter.newProcedureCall(parameterStat);
        }
            break;
        case Stat::Whitespace:
        {
            // Create new empty Word parameter
            if(not (lastProcedureCall().rawParameters().last().stat() == Stat::Word
                    or not lastProcedureCall().rawParameters().last().command().isEmpty()))
            {
                // Dont create if stat is empty Word
                return lastProcedureCall().newParameter(Stat::Word, QString());
            }
        }
            break;
        default:
            return ErrorController::throwError(ERROR_PREFIX + "Forbidden Stat");
        }
    }
        break;    
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::CommandSubbing>(Stat processingStat){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){    
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall

        // 1. 0 or 1 parameters
        // - throw error to find example (CommandSubbingStart in procedure Name)
        // - Warning: If changed ^^^^^^^^^, verify if VariableSubbing can be in procedure name
        // 2. Other numb of Parameters
        // 2.1. Is last parameter empty Word (empty parameter)
        // - Create new Call CommandSubbing
        // 2.2. Otherwise (No Word or not empty Word excluding Script, Braces, DoubleQuotes and ComplexWord)
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Create new Call CommandSubbing
        // 2.3. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 2.4. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 2.5.1. For empty VariableSubbing (check parameter with VariableSubbing Controller)
        // - Change to Word with $ sign
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Create new Call CommandSubbing
        // 2.5.2. OtherWise
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Create new Call CommandSubbing
        return newProcedureCall(Stat::CommandSubbing);
    }
    break;
    case Stat::DoubleQuotes:
    case Stat::BracesStart:
    {
        // 1. 0 parameters
        // - Create new Call DoubleQuotes or BracesStart (processingStat)
        // 2. 1 parameter
        // 2.1. Is last parameter Word
        // 2.1.1. Is empty
        // - throw error and check the case to implement solution (Empty Word in procedure name for processing stats DoubleQuotes and BracesStart)
        // 2.1.2. Not Empty
        // - treat state as Word parameter - append current Word
        // 2.2. Is last parameter VariableSubbing - no example then no implementation for now
        // - throw error to find example (VariableSubbing used as procedure name - please contact author)
        // 2.3. CommandSubbing
        // - throw error to find example (CommandSubbing in procedure name which is forbidden)
        // 2.4. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 2.5. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 2.6. BackslashSubbing
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add new parameter as Word parameter
        // 3. Other number of parameters
        // 3.1. Is last parameter empty Word (empty parameter)
        // - Create new Call DoubleQuotes or BracesStart (processingStat)
        // 3.2. Otherwise (No Word or not empty Word excluding Script, Braces, DoubleQuotes and ComplexWord)
        // - treat state as Word parameter - append current Word
        // 3.3. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 3.4. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 3.5. For special control of VariableSubbing and ONLY FOR BRACES state
        // - As VariableSubbing controller, is VariableSubbing parameter empty (OutputCommand is empty or no raw parameters- that means no Braces)
        // 3.5.1. If empty
        // - initialize parameter as Braces controlled - act as below vvv (Braces Call will be initialized only for empty VariableSubbing parameter, otherwise current method would not be called cause of processing Braces Call)
        // - Create new Call for BracesStart
        // 3.5.2. Otherwise
        // - treat state as Word parameter - append current Word

    }
        break;
    case Stat::VariableSubbing:
    {
        // 1. 0 or 1 parameters
        // - throw error to find example (VariableSubbing in procedure Name)
        // - Warning: If changed ^^^^^^^^^, verify if VariableSubbing can be in procedure name
        // 2. Other number of parameters
        // 2.1. Is last parameter empty Word (empty parameter)
        // - Change to VariableSubbing
        // 2.2. Otherwise (No Word or not empty Word excluding Script, Braces, DoubleQuotes and ComplexWord)
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add new parameter as VariableSubbing parameter
        // 2.3. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 2.4. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 2.5.1. For empty VariableSubbing (check parameter with VariableSubbing Controller)
        // - Change to Word with $ sign
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add VariableSubbing parameter
        // 2.5.2. OtherWise
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add VariableSubbing parameter

    }
        break;
    case Stat::BackslashSubbing:
    {
        // 1. 0
        // - Add new parameter BackslashSubbing
        // 2. 1 parameter
        // 2.1. Is last parameter Word
        // 2.1.1. Is empty
        // - throw error and check the case to implement solution (Empty Word in procedure name for processing stats DoubleQuotes and BracesStart)
        // 2.1.2. Not Empty
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add BackslashSubbing parameter
        // 2.2. Is last parameter VariableSubbing - no example then no implementation for now
        // - throw error to find example (VariableSubbing used as procedure name - please contact author)
        // 2.3. CommandSubbing
        // - throw error to find example (CommandSubbing in procedure name which is forbidden)
        // 2.4. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 2.5. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 2.6. BackslashSubbing
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add BackslashSubbing parameter
        // 3. Other number of parameters
        // 3.1. Is last parameter empty Word (empty parameter)
        // - Add BackslashSubbing parameter
        // 3.2. Otherwise (No Word or not empty Word excluding Script, Braces, DoubleQuotes and ComplexWord)
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add BackslashSubbing parameter
        // 3.3. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 3.4. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 3.5. For VariableSubbing
        // - As VariableSubbing controller, is VariableSubbing parameter empty (OutputCommand is empty or no raw parameters- that means no Braces)
        // 3.5.1. If empty
        // - Change to Word with $ sign
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add BackslashSubbing parameter
        // 3.5.2. Otherwise
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add BackslashSubbing parameter

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::CommandSubbing>(Stat processingStat){
    const QString ERROR_PREFIX = "Finalize Call Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){
    case Stat::CommandSubbingEnd:
    {
        // 1. 0
        // 1.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)
        // - Interpret as Word parameter (new parameter)
        // 1.2. If call for CommandSubbing [] checked by Controller is CommandSubbing (Previous call is CommandSubbing, DoubleQuotes or ComplexWord)
        // - Just finalize call (default complete call + move to last parameter of previous call)
        // 2. 1 parameter
        // 2.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)
        // 2.1.1 If last parameter is Word
        // - Append last parameter with ] sign
        // 2.1.2. Is empty Word
        // - throw error and check the case to implement solution (Empty Word in procedure name for processing stats DoubleQuotes and BracesStart)
        // 2.1.3. Is last parameter VariableSubbing - no example then no implementation for now
        // - throw error to find example (VariableSubbing used as procedure name - please contact author)
        // 2.1.4. CommandSubbing
        // - throw error to find example (CommandSubbing in procedure name which is forbidden)
        // 2.1.5. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 2.1.6. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 2.1.7. BackslashSubbing
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add Word parameter with sign ]
        // 2.2. If call for CommandSubbing [] checked by Controller is CommandSubbing (Previous call is CommandSubbing, DoubleQuotes or ComplexWord)
        // 2.2.1 If last parameter is Word or BackslashSubbing
        // - Just finalize call (default complete call + move to last parameter of previous call)
        // 2.2.2. Is empty Word
        // - throw error and check the case to implement solution (Empty Word in procedure name for processing stats DoubleQuotes and BracesStart)
        // 2.2.3. Is last parameter VariableSubbing - no example then no implementation for now
        // - throw error to find example (VariableSubbing used as procedure name - please contact author)
        // 2.2.4. CommandSubbing
        // - throw error to find example (CommandSubbing in procedure name which is forbidden)
        // 2.2.5. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 2.2.6. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 3. Other number of parameters
        // 3.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)
        // 3.1.1. Is last parameter empty Word (empty parameter)
        // - Append Word with ]
        // 3.1.2. Otherwise (No Word or not empty Word excluding Script, Braces, DoubleQuotes and ComplexWord)
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add BackslashSubbing parameter
        // 3.1.3. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 3.1.4. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 3.1.5. For VariableSubbing
        // - As VariableSubbing controller, is VariableSubbing parameter empty (OutputCommand is empty or no raw parameters- that means no Braces)
        // 3.1.5.1. If empty
        // - Change to Word with $ sign
        // - Append Word with ]
        // 3.1.5.2. Otherwise
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add Word parameter with sign ]
        // 3.2. If call for CommandSubbing [] checked by Controller is CommandSubbing (Previous call is CommandSubbing, DoubleQuotes or ComplexWord)
        // 3.2.1. Is last parameter empty Word (empty parameter)
        // - Remove last Parameter (its empty parameter)
        // - Just finalize call (default complete call + move to last parameter of previous call)
        // 3.2.2. Otherwise (No Word or not empty Word excluding ComplexWord)
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add BackslashSubbing parameter
        // 3.2.3. For Braces, Script and DoubleQuotes
        // - return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        // 3.2.4. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 3.2.5. For VariableSubbing
        // - As VariableSubbing controller, is VariableSubbing parameter empty (OutputCommand is empty or no raw parameters- that means no Braces)
        // 3.2.5.1. If empty
        // - Change to Word with $ sign
        // - Append Word with ]
        // 3.2.5.2. Otherwise
        // - Just finalize call (default complete call + move to last parameter of previous call)
    }
        break;
    case Stat::Semicolon:
    case Stat::EndOfString:
    {
        // 1. 0
        // 1.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)
        // - throw error (created empty call in Script which you try to finalize by EndOfString)
        // 1.2. If call for CommandSubbing [] checked by Controller is CommandSubbing (Previous call is CommandSubbing, DoubleQuotes or ComplexWord)
        // - Act as whitespace for 0 parameters (check newParameterProcedure)
        // 2. 1 parameter
        // 2.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)
        // 2.1.1 If last parameter is Word
        // - Just finalize call (default complete call)
        // 2.1.2. Is empty Word
        // - throw error and check the case to implement solution (Empty Word in procedure name for processing stats DoubleQuotes and BracesStart)
        // 2.1.3. Is last parameter VariableSubbing - no example then no implementation for now
        // - throw error to find example (VariableSubbing used as procedure name - please contact author)
        // 2.1.4. CommandSubbing
        // - throw error to find example (CommandSubbing in procedure name which is forbidden)
        // 2.1.5. For Braces, Script and DoubleQuotes
        // - Just finalize call (default complete call)
        // 2.1.6. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 2.1.7. BackslashSubbing
        // - Just finalize call (default complete call)
        // 2.2. If call for CommandSubbing [] checked by Controller is CommandSubbing (Previous call is CommandSubbing, DoubleQuotes or ComplexWord)
        // 2.2.1 If last parameter is Word or BackslashSubbing
        // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation
        // 2.2.2. Is empty Word
        // - throw error and check the case to implement solution (Empty Word in procedure name for processing stats DoubleQuotes and BracesStart)
        // 2.2.3. Is last parameter VariableSubbing - no example then no implementation for now
        // - throw error to find example (VariableSubbing used as procedure name - please contact author)
        // 2.2.4. CommandSubbing
        // - throw error to find example (CommandSubbing in procedure name which is forbidden)
        // 2.2.5. For Braces, Script and DoubleQuotes
        // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation
        // 2.2.6. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 3. Other number of parameters
        // 3.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)
        // 3.1.1. Is last parameter empty Word (empty parameter)
        // - Remove last Parameter (its empty parameter)
        // - Just finalize call (default complete call)
        // 3.1.2. Otherwise (No Word or not empty Word excluding Script, Braces, DoubleQuotes and ComplexWord)
        // - Just finalize call (default complete call)
        // 3.1.3. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord
        // 3.1.5. For VariableSubbing
        // - As VariableSubbing controller, is VariableSubbing parameter empty (OutputCommand is empty or no raw parameters- that means no Braces)
        // 3.1.5.1. If empty
        // - Change to Word with $ sign
        // - Just finalize call (default complete call)
        // 3.1.5.2. Otherwise
        // - Just finalize call (default complete call)
        // 3.2. If call for CommandSubbing [] checked by Controller is CommandSubbing (Previous call is CommandSubbing, DoubleQuotes or ComplexWord)
        // 3.2.1. Is last parameter empty Word (empty parameter)
        // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation
        // 3.2.2. Otherwise (No Word or not empty Word excluding ComplexWord)
        // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation
        // 3.2.4. For ComplexWord
        // - Impossible Case -  throw error for created new parameter after Whitespace of ComplexWord

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::BracesStart>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::BracesStart>: ";

    switch(processingStat){
    case Stat::BracesStart:
    {
        // List Control required
    }
    case Stat::Comment:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::Whitespace:
    case Stat::VariableSubbing:
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
        return tclInterpreter.finalizeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::BracesStart>(Stat parameterStat){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(parameterStat){
    case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        QString interpretedString = QString() /* procedure */;

        unknownString = interpretedString;
        if(lastProcedureCall().rawParametersLength() != 0){
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
        }else{
            return lastProcedureCall().newParameter(Stat::BackslashSubbing, unknownString);
        }
    }
        break;
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::Whitespace:
    case Stat::DoubleQuotes:
    case Stat::BracesStart:
    case Stat::VariableSubbing:
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        unknownString = tclInterpreter.readCurrentKeyword();
        if(lastProcedureCall().rawParametersLength() != 0
                and lastProcedureCall().rawParameters().last().stat() == Stat::Word){
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
        }else{
            return lastProcedureCall().newParameter(Stat::Word, unknownString);
        }
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::BracesStart>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";
    return throwError(ERROR_PREFIX + "No implementation for new call for Braces");
}

template<>
Error TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::BracesStart>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){
    case Stat::Braces:
    {
        // 1. Check if List/Braces Controller allows to finialize call
        // 1.1. If yes
        // - Just finalize call (default complete call + add to parameter of previous call)
        // 1.2. If no
        // -  Controller will update current state of List
        // - newParameterSpecialCommandCall_mode for Braces

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
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
    case Stat::EndOfString:
    case Stat::Semicolon:
        return newParameterSpecialCommandCall_mode<Stat::DoubleQuotes>(processingStat);
    case Stat::DoubleQuotes:
    {
        // End of Double Quotes
        // List Control required
        return tclInterpreter.finalizeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::DoubleQuotes>(Stat parameterStat){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(parameterStat){
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::Whitespace:
    case Stat::Semicolon:
    case Stat::EndOfString:
    {// Append if last parameter is Word or VariableSubbing
        unknownString = tclInterpreter.readCurrentKeyword();
        if(lastProcedureCall().rawParametersLength() != 0){
            if(lastProcedureCall().rawParameters().last().stat() == Stat::Word
                    or lastProcedureCall().rawParameters().last().stat() == Stat::VariableSubbing ){
                lastProcedureCall().rawParameters().last().appendCommand(unknownString);
            }else{
                return lastProcedureCall().newParameter(Stat::Word, unknownString);
            }
        }else{
            return lastProcedureCall().newParameter(Stat::Word, unknownString);
        }
    }
        break;
    case Stat::CommandSubbingStart:
    case Stat::BracesStart:
    {
        //lastProcedureCall().newParameter(parameterStat, unknownString);
        return tclInterpreter.newProcedureCall(parameterStat);
    }
        break;
    case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        QString interpretedString = QString() /* procedure */;

        unknownString = interpretedString;
        return lastProcedureCall().newParameter(parameterStat, interpretedString);
    }
        break;
    case Stat::VariableSubbing:
    {
        // Empty VariableSubbing == Command.isEmpty and RawParameters.isEmpty
        if(lastProcedureCall().rawParameters().last().stat() == Stat::VariableSubbing
                and lastProcedureCall().rawParameters().last().command().isEmpty()
                and lastProcedureCall().rawParameters().last().rawParameters().isEmpty())
        {   // Change to Word with $ sign + add new parameter VariableSubbing

        }else{
            return lastProcedureCall().newParameter(parameterStat, QString());
        }
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::DoubleQuotes>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){
    case Stat::CommandSubbingStart:
    {

    }
        break;
    case Stat::BracesStart:
    {// Review again
        // 1. If last parameter is Empty VariableSubbing
        // - Call Braces Procedure
        // 2. if Word
        // - Append with Braces controller
        // 3. Otherwise

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::DoubleQuotes>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){
    case Stat::DoubleQuotes:
    {
        // 1. If empty parameter (empty Word)
        // - Remove parameter
        // - Finialize Call
        // 2. If Empty VariableSubbing
        // - Change to Word with sign $
        // - Finialize Call
        // 3. Otherwise
        // - Finialize Call
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
    case Stat::VariableSubbing:
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
        return tclInterpreter.finalizeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::ComplexWord>(Stat parameterStat){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;


    switch(parameterStat){
    case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        QString interpretedString = QString() /* procedure */;

        unknownString = interpretedString;
        return lastProcedureCall().newParameter(parameterStat, interpretedString);
    }
        break;
    case Stat::BracesStart:
    {

    }
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    {
        unknownString = tclInterpreter.readCurrentKeyword();
        if(lastProcedureCall().rawParametersLength() != 0){
            switch(lastProcedureCall().rawParameters().last().stat()){
            case Stat::Word:
                lastProcedureCall().rawParameters().last().appendCommand(unknownString);
                break;
            case Stat::VariableSubbing:
            {
                // VariableSubbing Controller required
                // For now simple solution for braces only
                if(not lastProcedureCall().rawParameters().last().rawParameters().isEmpty()
                        and lastProcedureCall().rawParameters().last().rawParameters().first().stat() == Stat::Braces)
                {
                    return lastProcedureCall().newParameter(parameterStat, unknownString);
                }else{
                    return throwError(ERROR_PREFIX + "No VariableSubbing Controller");
                }
            }
                break;
            case Stat::CommandSubbing:
            case Stat::BackslashSubbing:
            {
                return lastProcedureCall().newParameter(parameterStat, unknownString);
            }
                break;
            default:
                return throwError(ERROR_PREFIX + "Unknown Stat ");
            }
        }else{
            return throwError("No parameters for ComplexWord");
        }
    }
        break;
    case Stat::CommandSubbingStart:
    {
        //lastProcedureCall().newParameter(parameterStat, unknownString);
        return tclInterpreter.newProcedureCall(parameterStat);
    }
        break;
    case Stat::VariableSubbing:
    {
        if(lastProcedureCall().rawParametersLength() != 0){
            switch(lastProcedureCall().rawParameters().last().stat()){
            case Stat::VariableSubbing:
            {
                // VariableSubbing Controller required
                // For now simple solution for braces only
                if(not lastProcedureCall().rawParameters().last().rawParameters().isEmpty()
                        and lastProcedureCall().rawParameters().last().rawParameters().first().stat() == Stat::Braces)
                {
                    return lastProcedureCall().newParameter(parameterStat, unknownString);
                }else{
                    return throwError(ERROR_PREFIX + "No VariableSubbing Controller");
                }
            }
                break;
                // Review
            default:
                return throwError(ERROR_PREFIX + "Unknown Stat ");
            }

        }else{
            return throwError("No parameters for ComplexWord");
        }
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::ComplexWord>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){
    case Stat::CommandSubbingStart:
    {

    }
        break;
    case Stat::VariableSubbing:
    {

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::ComplexWord>(Stat processingStat){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat){
    case Stat::Whitespace:
    {
        // Finialize ComplexWord

    }
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
    case Stat::VariableSubbing:
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
        return tclInterpreter.finalizeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::Expression>(Stat parameterStat){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(parameterStat){
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    {
        unknownString = tclInterpreter.readCurrentKeyword();
        if(lastProcedureCall().rawParameters().last().stat() == Stat::Word){
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
        }else{  // Any other stat is parameter without whitespace after parameter Ex. "abc"abc Expection [puts abc]abc - ComplexWord
            return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        }
    }
        break;
    case Stat::DoubleQuotes:
    case Stat::BracesStart:
    case Stat::VariableSubbing:
    case Stat::BackslashSubbing:
    {
        //lastProcedureCall().newParameter(parameterStat, unknownString);
        return tclInterpreter.newProcedureCall(parameterStat);
    }
        break;
    case Stat::Whitespace:
    {
        // Create new empty Word parameter
        if(not (lastProcedureCall().rawParameters().last().stat() == Stat::Word
                or not lastProcedureCall().rawParameters().last().command().isEmpty()))
        {
            // Dont create if stat is empty Word
            return lastProcedureCall().newParameter(Stat::Word, QString());
        }
    }
        break;
    default:
        return ErrorController::throwError(ERROR_PREFIX + "Forbidden Stat");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::Expression>(Stat processingStat){
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
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return tclInterpreter.newProcedureCall(processingStat);
    }
        break;
    case Stat::CommandSubbingEnd:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
       //return tclInterpreter.finializeProcedureCall(processingStat);
    }
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
        return tclInterpreter.finializeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::Expression>(Stat processingStat){
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
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return tclInterpreter.newProcedureCall(processingStat);
    }
        break;
    case Stat::CommandSubbingEnd:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
       //return tclInterpreter.finializeProcedureCall(processingStat);
    }
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
        return tclInterpreter.finializeProcedureCall(processingStat);
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
        // Comment Control
        // Copy from dev branch + think about BackslashSubbing with newline
    }
        break;
    case Stat::Whitespace:
        break;
    case Stat::CommandSubbingEnd:
        return throwError(ERROR_PREFIX + "CommandSubbingEnd in MainScript or Script");
    case Stat::VariableSubbing:
        return throwError(ERROR_PREFIX + "VariableSubbingStart in MainScript ");
    case Stat::BracesStart:
    case Stat::Braces:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingStart:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // End of Complex Word
        return tclInterpreter.finalizeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::Script>(Stat parameterStat){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(parameterStat){
    case Stat::Braces:
    case Stat::DoubleQuotes:
    case Stat::BracesStart:
    {
        // New Procedure call  + Braces
        // Maybe create new Procedure first, then interpret as Braces
        //if(commandsController.newProcedureCall() == Error::Error)
        //    return throwError(ERROR_PREFIX + error());
    }
    case Stat::Word:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
    {
        unknownString = tclInterpreter.readCurrentKeyword();
        return tclInterpreter.newProcedureCall(unknownString);
    }
        break;
    default:
        return ErrorController::throwError(ERROR_PREFIX + "Forbidden Stat");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::Script>(Stat processingStat){
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
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return tclInterpreter.newProcedureCall(processingStat);
    }
        break;
    case Stat::CommandSubbingEnd:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
       //return tclInterpreter.finializeProcedureCall(processingStat);
    }
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
        return tclInterpreter.finializeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::Script>(Stat processingStat){
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
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
        return newParameterSpecialCommandCall_mode<Stat::CommandSubbing>(processingStat);
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return tclInterpreter.newProcedureCall(processingStat);
    }
        break;
    case Stat::CommandSubbingEnd:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
       //return tclInterpreter.finializeProcedureCall(processingStat);
    }
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // Finialize Procedure call
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
        return tclInterpreter.finializeProcedureCall(processingStat);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

const TclProcedureInterpreter::ProcedureCallFunctions::CommandCallControlFunctions
TclProcedureInterpreter::ProcedureCallFunctions::commandCallSpecialFunctions
[Tcl::Interpreter::Core::numbOfSpecialCommandCallsAndSafeguard()] =
{
    //CommandSubbing
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::CommandSubbing>,
    },
    //BracesStart
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::BracesStart>,
    },
    //DoubleQuotes
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::DoubleQuotes>,
    },
    //ComplexWord
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::ComplexWord>,
    },
    //Expression
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::Expression>,
    },
    //Script
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_mode<Stat::Script>,
        &TclProcedureInterpreter::newParameterSpecialCommandCall_mode<Stat::Script>,
        &TclProcedureInterpreter::newCallSpecialCommandCall_mode<Stat::Script>,
        &TclProcedureInterpreter::finalizeCallSpecialCommandCall_mode<Stat::Script>,
    },
    // SafeGuard
    {
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
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
