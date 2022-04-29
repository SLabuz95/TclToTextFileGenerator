#include"TclInterpreter/CommandsCont/commandsCont.hpp"
#include"TclInterpreter/CommandsCont/commandCallConfig.hpp"
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
          : userConfig.userDefaultProcedureConfig())//,
//    Commented but required
//    newProcedureCallFunction(ProcedureCallFunctions::newCallAt(userConfig.proceduresSettings().mode())),
//    finalizeProcedureCallFunction(ProcedureCallFunctions::finalizeCallAt(userConfig.proceduresSettings().mode()))
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
//    Commented but required
//    finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCase);
    tclInterpreter.activateWriteOnlyProcedureMode();
}

void Controller::deactivateWriteOnlyProcedureMode()
{
//    Commented but required
//    finalizeProcedureCallFunction = ProcedureCallFunctions::finalizeCallAt(ProdecuresSettings::InterpreterMode::TestCaseReport);
    tclInterpreter.deactivateWriteOnlyProcedureMode();
}


template<>
Error TclProcedureInterpreter::callDefinition_mode<Settings::InterpreterMode::TestCase>(Call::Name name){
    using Definition = CommandDefinitions::Iterator;
    Definition definition;
    for(definition = procedureDefinitions.begin(); definition < procedureDefinitions.end(); definition++){
        if(definition->name == name)
            break;
    }
    try {
        /* Commented but required
        if(definition == procedureDefinitions.end()){
            procedureCalls.append(Call(name));
        }else{
            procedureCalls.append(Call(definition));
        }*/
    }  catch (std::exception& e) {
        return throwError(e.what());
    }

    return Error::NoError;
}
template<>
Error TclProcedureInterpreter::callDefinition_mode<Settings::InterpreterMode::TestCaseReport>(Call::Name name){
    tryToActivateWriteOnlyProcedure(name);
    return callDefinition_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

template<>
Error TclProcedureInterpreter::callDefinition_mode<Settings::InterpreterMode::PredefinitionsOnly>(Call::Name name){
    return callDefinition_mode<UserInputConfig::Settings::InterpreterMode::TestCase>(name);
}

const Settings::CallDefinitionInterModeFctPtr
Settings::callDefinitionInterModeCalls[Settings::mode2number(Settings::InterpreterMode::NumbOfModes)] =
{
    &TclProcedureInterpreter::callDefinition_mode<Settings::InterpreterMode::TestCase>,
    &TclProcedureInterpreter::callDefinition_mode<Settings::InterpreterMode::TestCaseReport>,
    &TclProcedureInterpreter::callDefinition_mode<Settings::InterpreterMode::PredefinitionsOnly>,
};


template<>
Error TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::TestCase>(SavedStat &statCommand){
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
Error TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::TestCaseReport>(SavedStat &){
    finalizeOn = false;

    return Error::NoError;

}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::PredefinitionsOnly>(SavedStat &statCommand){

    return finalizeCall_mode<Settings::InterpreterMode::TestCase>(statCommand);

}

const Settings::FinalizeCallInterModeFctPtr
Settings::finalizeCallInterModeCalls[Settings::mode2number(Settings::InterpreterMode::NumbOfModes)] =
{
    &TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::TestCase>,
    &TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::TestCaseReport>,
    &TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::PredefinitionsOnly>,
};

//

template<>
Error TclProcedureInterpreter::constructor_mode<Stat::Common>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::Common>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::Common>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::Common>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::Common>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::Common>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::Common>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::Comment:
    {
        // Comment Control required
        // Finialize Procedure call or threat as word
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)

    }
        Q_FALLTHROUGH();
    case Stat::Word:
    case Stat::Braces:
    case Stat::Whitespace:
    case Stat::Namespace:
        // Above states can create ComplexWord
    case Stat::CommandSubbingStart:
    case Stat::BracesStart:
    case Stat::DoubleQuotes:
    case Stat::VariableSubbing:
    case Stat::BackslashSubbing:    // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return newCallProcessing();
    case Stat::CommandSubbingEnd:    
    case Stat::EndOfString:
    case Stat::Semicolon:
    {        
        return finalizeCallProcessing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    // Unreachable
    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::CommandSubbing>(){
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
        switch(processingStat()){
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
        case Stat::Whitespace:
        {
            // Ignore whitespace before procedure name
        }
            break;

        default:
            return throwError(ERROR_PREFIX + "Forbidden Stat");
        }
    }
        break;
    // 2. Is number of parameters 1 (1 parameter indicates that new parameter will be second parameter - first parameter is "ready" as procedure name - name for procedure definitions can be assigned)
    default:
    {
        switch(processingStat()){
        case Stat::Comment:
            if(lastProcedureCall().rawParametersLength() == 1 and
                    lastProcedureCall().isLastParameterEmpty())
                // If first parameter is empty (procedure name parameter)
            {
                return throwError(ERROR_PREFIX + "Comment sign as first sign of procedure name");
            }
        case Stat::Word:
        case Stat::Braces:
        case Stat::Namespace:
        case Stat::CommandSubbingEnd:// Called from newCallProcedure only to create new parameter or append Word parameter
        {
            unknownString = tclInterpreter.readCurrentKeyword();            
            if(lastProcedureCall().rawParameters().last().stat() == Stat::Word){
                lastProcedureCall().rawParameters().last().appendCommand(unknownString);
            }else{  // Any other stat is parameter without whitespace after parameter Ex. "abc"abc Expection [puts abc]abc - ComplexWord
                return throwError(ERROR_PREFIX + "Impossible Case");
                //return lastProcedureCall().newParameter(Stat::Word, unknownString);
            }
        }
            break;
        case Stat::DoubleQuotes:
        case Stat::BracesStart:
        case Stat::VariableSubbing:
        case Stat::BackslashSubbing:
        {
            //lastProcedureCall().newParameter(parameterStat, unknownString);
            //return tclInterpreter.newProcedureCall(parameterStat);
        }
            break;
        case Stat::Whitespace:
        {
            // Create new empty Word parameter
            if(not lastProcedureCall().rawParameters().last().isEmpty()){
                switch(lastProcedureCall().rawParameters().last().stat()){
                    case Stat::VariableSubbing:
                    {
                      // Variable controller
                        // If empty Variable
                        // Change to Word with $ sign
                    }
                }
                return newParameterProcessing();
            }
        }
            break;
        default:
            return throwError(ERROR_PREFIX + "Forbidden Stat");
        }
    }
        break;    
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall

        // If last parameter VariableSubbing
        // - If empty, change to Word with $
        // - finally
        // - - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - - Create new Call CommandSubbing
        // if empty Word, then change to CommandSubbing (end of case)
        if(not lastProcedureCall().isLastParameterEmpty()){
            switch(lastProcedureCall().rawParameters().last().stat()){
            case Stat::VariableSubbing:
            {
                // Variable Subbing is Empty
                if(/*VariableSubbing.IsEmpty*/ false){
                    // change to Word with $
                }
            }
            Q_FALLTHROUGH();
            default:
            {
                // - - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
                if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                    return throwError(ERROR_PREFIX + error());
                }
                break;
            }
            // Error Cases
            case Stat::Braces:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            }
        }

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
        return createCall(Stat::CommandSubbing);
        //return newProcedureCall(Stat::CommandSubbing);
    }
    break;
    case Stat::DoubleQuotes:
    case Stat::BracesStart:
    {
        // 1. 0 parameters (Not applicable)
        // - Create new Call DoubleQuotes or BracesStart (processingStat)

        if(lastProcedureCall().isLastParameterEmpty()){
            // Change to processing stat
            // Call Processing State Procedure call
            return createCall(processingStat());
        }else{
            switch(lastProcedureCall().rawParameters().last().stat()){
            case Stat::VariableSubbing:
            {
                // Variable Subbing is Empty
                // If BracesStat , Special variableSubbing control
                // if DoubleQuotes, change to Word with $ sign
                if(/*VariableSubbing.IsEmpty*/ false){
                    if(/*DoubleQuotes*/ false){
                        // change to Word with $
                    }else{ // BracesStat

                    }
                }
            }
            Q_FALLTHROUGH();
            default:
            {
                break;
            }
            // Error Cases
            case Stat::Braces:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            }
        }

        // 3.5. For special control of VariableSubbing and ONLY FOR BRACES state
        // - As VariableSubbing controller, is VariableSubbing parameter empty (OutputCommand is empty or no raw parameters- that means no Braces)
        // 3.5.1. If empty
        // - initialize parameter as Braces controlled - act as below vvv (Braces Call will be initialized only for empty VariableSubbing parameter, otherwise current method would not be called cause of processing Braces Call)
        // - Create new Call for BracesStart

        return newParameterProcessing(); // After ComplexWord creation, new Parameter call will be called for COmplexWord
    }
        break;
    case Stat::VariableSubbing:
    {
        if(lastProcedureCall().rawParametersLength() == 1){
            return throwError(ERROR_PREFIX + "VariableSubbing in procedure name");
        }
        if(lastProcedureCall().isLastParameterEmpty()){
            // Change to VariableSubbing
        }else{
            switch(lastProcedureCall().rawParameters().last().stat()){
            case Stat::VariableSubbing:
            {
                // Variable Subbing is Empty
                if(/*VariableSubbing.IsEmpty*/ false){
                    // change to Word with $
                }
            }
            Q_FALLTHROUGH();
            default:
            {
                break;
            }
            // Error Cases
            case Stat::Braces:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            }
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                return throwError(ERROR_PREFIX + error());
            }
            return newParameter(Stat::VariableSubbing);
        }
        // 2.5.1. For empty VariableSubbing (check parameter with VariableSubbing Controller)
        // - Change to Word with $ sign
        // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
        // - Add VariableSubbing parameter
        // 2.5.2. OtherWise

   }
        break;
    case Stat::BackslashSubbing:
    {
        if(lastProcedureCall().isLastParameterEmpty()){
            // Change to BackslashSubbing + execute special Backslash control proceudre
        }else{
            switch(lastProcedureCall().rawParameters().last().stat()){
            case Stat::VariableSubbing:
            {
                // Variable Subbing is Empty
                if(/*VariableSubbing.IsEmpty*/ false){
                    // change to Word with $
                }
            }
            Q_FALLTHROUGH();
            default:
            {
                break;
            }
            // Error Cases
            case Stat::Braces:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            }
            // execute special Backslash control proceudre
            // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                return throwError(ERROR_PREFIX + error());
            }
            // - Add BackslashSubbing parameter + execute special Backslash control proceudre
            return newParameter(Stat::BackslashSubbing);
        }
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "Finalize Call Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::CommandSubbingEnd:
    {
        if(/* 1.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)*/ false){
            // - Interpret as Word parameter (new parameter)
            return newParameterProcessing();
        }else{
            if(lastProcedureCall().isLastParameterEmpty()){

            }
            // 1.2. If call for CommandSubbing [] checked by Controller is CommandSubbing (Previous call is CommandSubbing, DoubleQuotes or ComplexWord)
            switch(lastProcedureCall().rawParameters().last().stat()){
            case Stat::VariableSubbing:
            {
                // Variable Subbing is Empty
                if(/*VariableSubbing.IsEmpty*/ false){
                    // change to Word with $
                }
            }
            Q_FALLTHROUGH();
            default:
                break;
            }
            // execute special Backslash control proceudre
            // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
            // - Add BackslashSubbing parameter
        // - Just finalize call (default complete call + move to last parameter of previous call)
           return finializeCall();
        }

    }
        break;
    case Stat::Semicolon:
    case Stat::EndOfString:
    {
        // 1. 0
        if(/* 1.1. If call for CommandSubbing [] checked by Controller is not CommandSubbing (no previous call or Script)*/ false){
            if(lastProcedureCall().isLastParameterEmpty()){
                if(lastProcedureCall().rawParameters().length() > 1){
                    // Remove last parameter

                    // - Just finalize call (default complete call)
                }else{

                    return throwError(ERROR_PREFIX + "Impossible case");
                    // - Act as whitespace for 0 parameters (check newParameterProcedure)
                }
            }else{
                switch(lastProcedureCall().rawParameters().last().stat()){
                case Stat::VariableSubbing:
                {
                    // Variable Subbing is Empty
                    if(/*VariableSubbing.IsEmpty*/ false){
                        // change to Word with $
                    }
                }
                Q_FALLTHROUGH();
                default:
                    break;
                }
                // - Just finalize call (default complete call)
            }
        }else{
            if(lastProcedureCall().isLastParameterEmpty()){
                if(lastProcedureCall().rawParameters().length() > 1){
                    // Remove last parameter

                    // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation
                }else{
                    // Possible case - just ignore
                    // - Act as whitespace for 0 parameters (check newParameterProcedure)
                }
            }
            // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation

        }

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::BracesStart>: ";

    switch(processingStat()){
    case Stat::BracesStart:
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
        return newParameterProcessing();
    case Stat::Braces:
    {
        // End of Braces
        // List Control required
        return finalizeCallProcessing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(processingStat()){
    case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        QString interpretedString = QString() /* procedure */;
        // Only allowed backslash subbing for newLine (processed and saved)
        // Otherwise just saved
        unknownString = interpretedString;
        if(lastProcedureCall().rawParametersLength() != 0){
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
        }else{
            return lastProcedureCall().newParameter(Stat::Word, unknownString);
        }
    }
        break;    
    case Stat::VariableSubbing:
    case Stat::Word:
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::Whitespace:
    case Stat::DoubleQuotes:
    case Stat::BracesStart:
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        unknownString = tclInterpreter.readCurrentKeyword();
        lastProcedureCall().rawParameters().last().appendCommand(unknownString);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::BracesStart>: ";
    return throwError(ERROR_PREFIX + "No implementation for new call for Braces");
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "Finalize Call Special Command <Stat::BracesStart>: ";

    switch(processingStat()){
    case Stat::Braces:
    {
        // 1. Check if List/Braces Controller allows to finialize call
        // 1.1. If yes
        // - Just finalize call (default complete call + add to parameter of previous call)
        // 1.2. If no
        // -  Controller will update current state of List
        // - append last raw parameter Word

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::DoubleQuotes>: ";

    switch(processingStat()){
    case Stat::Braces:
    case Stat::Comment:
    case Stat::Word:
    case Stat::Whitespace:
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:
    case Stat::CommandSubbingEnd:
    case Stat::EndOfString:
    case Stat::Semicolon:
        return newParameterProcessing();
    case Stat::CommandSubbingStart:
    case Stat::BracesStart:
        return newCallProcessing();
    case Stat::DoubleQuotes:
        // End of Double Quotes
        return finalizeCallProcessing();
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(processingStat()){
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::Whitespace:
    case Stat::Semicolon:
    case Stat::EndOfString:
    case Stat::BracesStart:
    {// Append if last parameter is Word or VariableSubbing
        unknownString = tclInterpreter.readCurrentKeyword();
        switch(lastProcedureCall().rawParameters().last().stat()){
        case Stat::Word:
        {
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
        }
            break;
        case Stat::VariableSubbing:
        {
            // 1. Variable Subbing controller will decise what to do
            // - If Variable with Braces - create new parameter Word
            // - If Variable empty or "simple" -  extend as much you can, if rest of string exists (returned not empty string) - create new parameter Word
        }
            break;
        case Stat::CommandSubbing:
        case Stat::BackslashSubbing:
        {
            return lastProcedureCall().newParameter(Stat::Word, unknownString);
        }
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat ");
        }
    }
        break;
    case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        QString interpretedString = QString() /* procedure */;

        unknownString = interpretedString;
        // If last parameter is mepty VariableSubbing - Change to Word with $ sign

        return lastProcedureCall().newParameter(processingStat(), interpretedString);
    }
        break;
    case Stat::VariableSubbing:
    {
        // if empty VariableSubbing
        // Change to Word with $ sign
        // finally add new parameter VariableSubbing

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::CommandSubbingStart:
    {
        // if empty parameter, change to CommandSubbing
        if(lastProcedureCall().isLastParameterEmpty()){
            //
        }else{
            if(lastProcedureCall().rawParameters().last().stat() == Stat::VariableSubbing){
                // if empty VariableSubbing, change to Word with $ sign + BracesStart VariableSubbing

            }
        }
        // if empty VariableSubbing, change to Word with $ sign
        // finally new procedure call CommandSubbing
        return createCall(Stat::CommandSubbing);
    }
        break;
    case Stat::BracesStart:
    {

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::DoubleQuotes:
    {       
        if(lastProcedureCall().isLastParameterEmpty()){
            // remove empty parameter
        }else{
            if(/* Empty VariableSubbing*/ false){
                // - Change to Word with sign $

            }
        }

        // - Finialize Call
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::ComplexWord>: ";

    switch(processingStat()){
    case Stat::Comment:
    {
        // Comment Control required
        // Finialize Procedure call or threat as word
        // Call from main TclInterpreter or CommandsCall
        // Verify what type of CommandSubbing it is (for script CommandSubbing, commandSubbingEnd will be used as sign)
    }
    case Stat::Braces:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
    case Stat::CommandSubbingEnd:
        return newParameterProcessing();
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
        // End of Complex Word
        return finalizeCallProcessing();
    case Stat::CommandSubbingStart:
    case Stat::BracesStart:
        return newCallProcessing();
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(processingStat()){
    case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        QString interpretedString = QString() /* procedure */;

        // If empty VariableSubbing, change to Word  with $
        // Otherwise new Parameter

        unknownString = interpretedString;
        return lastProcedureCall().newParameter(processingStat(), interpretedString);
    }
        break;
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    {
        unknownString = tclInterpreter.readCurrentKeyword();

        switch(lastProcedureCall().rawParameters().last().stat()){
        case Stat::Word:
            lastProcedureCall().rawParameters().last().appendCommand(unknownString);
            break;
        case Stat::VariableSubbing:
        {
            // VariableSubbing Controller required
            // If empty VariableSubbing, change to Word with $ sign
            // Otherwise new parameter Word
        }
            break;
        case Stat::CommandSubbing:
        case Stat::BackslashSubbing:
        {
            return lastProcedureCall().newParameter(processingStat(), unknownString);
        }
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat ");
        }
    }
        break;
    case Stat::VariableSubbing:
    {

        // VariableSubbing Controller required
        // For empty VariableSubbing, change to Word with $ sign
        // finally new Parameter

    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::CommandSubbingStart:
    {
        // Last parameter is Empty VariableSubbing, change to Word with $ sign
        // finally new call CommandSubbing + new parameter
        return createCall(Stat::CommandSubbing);
    }
        break;
    case Stat::BracesStart:
    {
        // Last parameter is Empty VariableSubbing, new call Braces
        // If Word, append Word
        // Otherwise new Word Parameter
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // Last parameter is Empty VariableSubbing, new call Braces
        // Finalize and add new empty parameter
        // Recall interpreter procedure TclInterpreter for the same state
    }
        break;
    case Stat::Whitespace:
    {
        // Last parameter is Empty VariableSubbing, new call Braces
        // Finalize and add new empty parameter
    }
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::Expression>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Expression>: ";
    return interpret_mode<Stat::CommandSubbing>();
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::Expression>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    return newParameterProcessing_mode<Stat::CommandSubbing>();
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::Expression>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Expression>: ";
    return newCall_mode<Stat::CommandSubbing>();
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::Expression>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Expression>: ";
    return finalizeCall_mode<Stat::CommandSubbing>();
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Script>: ";

    switch(processingStat()){
    // Like in Main Script
    case Stat::Comment:
    {
        // Comment Control
        // Copy from dev branch + think about BackslashSubbing with newline
    }
        break;
    case Stat::BracesStart:
    case Stat::Word:
    case Stat::DoubleQuotes:
    case Stat::Namespace:
    case Stat::BackslashSubbing:    // Probably only \newLine available
        return newCallProcessing();
    case Stat::Braces:
        return finalizeCallProcessing();
    case Stat::Semicolon:    
    case Stat::Whitespace:
        break;
    // Error cases
    case Stat::CommandSubbingEnd:
        return throwError(ERROR_PREFIX + "CommandSubbingEnd in script");
    case Stat::VariableSubbing:
        return throwError(ERROR_PREFIX + "VariableSubbing in script ");
    case Stat::CommandSubbingStart:
        return throwError(ERROR_PREFIX + "CommandSubbingStart in script");
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    return throwError(ERROR_PREFIX + "No implemantation");
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    return (createCall(Stat::CommandSubbing) == Error::Error
            or interpret() == Error::Error)? Error::Error : Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";
    // If main script, newCall_mode
    // else finialize

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::ExprCommand>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Script>: ";

    switch(processingStat()){
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
        return newParameterProcessing();
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        // End of Complex Word
        return finalizeCallProcessing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::ExprCommand>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;

    switch(processingStat()){
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
        return newProcedureCall(unknownString);
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Forbidden Stat");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::ExprCommand>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
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
        return newParameterProcessing_mode<Stat::CommandSubbing>();
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return newCallProcessing();
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
        return finalizeCallProcessing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::ExprCommand>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
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
        return newParameterProcessing();
    case Stat::CommandSubbingStart:
    {
        // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return newCallProcessing();
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
        return finalizeCallProcessing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::Ignore>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Script>: ";

    switch(processingStat()){
    case Stat::Semicolon:
    case Stat::EndOfString:
    {
        if(removeIgnore() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
    default:
        break;
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::Ignore>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    return throwError(ERROR_PREFIX + "No implemantation");
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::Ignore>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";
    return throwError(ERROR_PREFIX + "No implemantation");
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::Ignore>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::CommandSubbing>: ";
    return throwError(ERROR_PREFIX + "No implemantation");
}


CallConfig::CommandCallControlFunctions
CallConfig::commandCallSpecialFunctions
[Tcl::Interpreter::Core::numbOfSpecialCommandCallsAndSafeguard()] =
{
    //CommandSubbing
    {
        &TclProcedureInterpreter::interpret_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::newCall_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::CommandSubbing>,
    },
    //BracesStart
    {
        &TclProcedureInterpreter::interpret_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::newCall_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::BracesStart>,
    },
    //DoubleQuotes
    {
        &TclProcedureInterpreter::interpret_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::newCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::DoubleQuotes>,
    },
    //ComplexWord
    {
        &TclProcedureInterpreter::interpret_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::newCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::ComplexWord>,
    },
    //Expression
    {
        &TclProcedureInterpreter::interpret_mode<Stat::Expression>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Expression>,
        &TclProcedureInterpreter::newCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::Expression>,
    },
    //Script
    {
        &TclProcedureInterpreter::interpret_mode<Stat::Script>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Script>,
        &TclProcedureInterpreter::newCall_mode<Stat::Script>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::Script>,
    },
    //ExprCommand
    {
        &TclProcedureInterpreter::interpret_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::newCall_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::CommandSubbing>,
    },
    //Ignore
    {
        &TclProcedureInterpreter::interpret_mode<Stat::Ignore>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Ignore>,
        &TclProcedureInterpreter::newCall_mode<Stat::Ignore>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::Ignore>,
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
    &TclProcedureInterpreter::interpret_mode<Stat::CommandSubbing>,
        &TclProcedureInterpreter::interpret_mode<Stat::BracesStart>,
        &TclProcedureInterpreter::interpret_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::interpret_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::interpret_mode<Stat::Expression>,
        &TclProcedureInterpreter::interpret_mode<Stat::Script>,
        &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat
};
*/


Error Controller::createCall(Stat stat, Call::Parameter&& parameter){
    // If Stat id for semi command call stats is correct (If wrong stat , stat id of Stat::Size is returnedgh)
    if(Settings::specialCallStat2number(stat) == Settings::specialCallStat2number(Stat::Size))
        return throwError("Wrong stat for CreateCall procedure. Stat: " + QString::number(TCLInterpreter::cast_stat(stat)));
    procedureCalls.append(Call(stat, parameter));
    return Error::NoError;
}

Error Controller::createCallAndMoveLastParameterToOne(Stat stat){
    if(not procedureCalls.last().isLastParameterEmpty())
        return throwError("CreateAndMove procedure: No parameters or empty parameter");
    return createCall(stat, procedureCalls.last().rawParameters().takeLast());
}


Error Controller::throwError(){return tclInterpreter.throwError();}
Error Controller::throwError(const QString str){return tclInterpreter.throwError(str);}
const QString& Controller::error(){return tclInterpreter.error();}
