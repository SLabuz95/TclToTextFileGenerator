#include"TclInterpreter/CommandsCont/commandsCont.hpp"
#include"TclInterpreter/CommandsCont/commandCallConfig.hpp"
#include"TclInterpreter/tclToCAPL.hpp"
#include"Tcl2Capl/controller.hpp"
#include"TclInterpreter/ListsCont/listsCont.hpp"

using namespace Tcl;

Command::Controller::Controller(TCLInterpreter& tclInterpreter, UserInputConfig& userConfig) :
    tclInterpreter(tclInterpreter),
    userConfig(userConfig),
    procedureDefinitions(
        (userConfig.userProcedureConfig().isEmpty())?
            defaultProcedureDefinitions
          : userConfig.userProcedureConfig()),
    unknownProcedureDefinition(
        (userConfig.userDefaultProcedureConfig().isRulesEmpty())?
            Definition::defaultUnknownProcedureDefinition
          : userConfig.userDefaultProcedureConfig()),
   callDefinitionFunction(Settings::newCallAt(userConfig.proceduresSettings().mode())),
    finalizeCallFunction(Settings::finalizeCallAt(userConfig.proceduresSettings().mode()))
{
    updateCurrentCallProcedures();
}

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

    finalizeCallFunction = Settings::finalizeCallAt(Settings::InterpreterMode::TestCase);
    tclInterpreter.activateWriteOnlyProcedureMode();
}

void Controller::deactivateWriteOnlyProcedureMode()
{
    finalizeCallFunction = Settings::finalizeCallAt(Settings::InterpreterMode::TestCaseReport);
    tclInterpreter.deactivateWriteOnlyProcedureMode();
}


template<>
Error TclProcedureInterpreter::callDefinition_mode<Settings::InterpreterMode::TestCase>(Call::Name name){
    using Definition = CommandDefinitions::Iterator;
    Definition definition;    

    for(definition = hardcodedProcedureDefinitions.begin(); definition < hardcodedProcedureDefinitions.end(); definition++){
        if(definition->name == name)
            break;
    }

    if(definition == hardcodedProcedureDefinitions.end()){
        for(definition = procedureDefinitions.begin(); definition < procedureDefinitions.end(); definition++){
            if(definition->name == name)
                break;
        }
        if(definition == procedureDefinitions.end()){
            lastProcedureCall().setDefinition(&unknownProcedureDefinition);
        }else{
            lastProcedureCall().setDefinition(definition);
        }
    }else{
        lastProcedureCall().setDefinition(definition);
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
Error TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::TestCase>(){
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
    finalizeOn = false;

    // If not commandSubbing (procedureCall in Script) and outputCommand is not Empty
    if(isNotCommandSubbing()){
        if(lastProcedureCall().lastParameter().stat() != Stat::Script){
            lastProcedureCall().outputCommand().append(";");
        }
        if(lastProcedureCall().outputCommand().indexOf(QRegularExpression("\\R[\\s]{0,}\\z")) == -1)
        {
            // Pattern not found
            lastProcedureCall().outputCommand().append("\n");
        }
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::TestCaseReport>(){
    finalizeOn = false;

    return Error::NoError;

}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Settings::InterpreterMode::PredefinitionsOnly>(){

    return finalizeCall_mode<Settings::InterpreterMode::TestCase>();

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
    case Stat::Namespace:
        // Above states can create ComplexWord
    case Stat::CommandSubbingStart:
    case Stat::BracesStart:
    case Stat::DoubleQuotes:
    case Stat::VariableSubbing:
    case Stat::BackslashSubbing:    // New procedure call (TclInterpreter will add savedStat as TclInterpreter)
        // Call from main TclInterpreter or CommandsCall
        return newCallProcessing();
    case Stat::Whitespace:
        return newParameterProcessing();
    case Stat::CommandSubbingEnd:
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Braces:
        return finalizeCallProcessing();
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }
    // Unreachable
    //return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    QString unknownString;
    {
        switch(processingStat()){
        case Stat::Comment:
            if( lastProcedureCall().isLastParameterEmpty()
                    and lastProcedureCall().rawParametersLength() == 1 )
                // If first parameter is empty (procedure name parameter)
            {
                return throwError(ERROR_PREFIX + "Comment sign as first sign of procedure name");
            }
        case Stat::Word:
        case Stat::Braces:
        case Stat::Namespace:
        case Stat::CommandSubbingEnd:// Called from newCallProcedure only to create new parameter or append Word parameter
        case Stat::DoubleQuotes:
        case Stat::BracesStart:
        {
            if(lastProcedureCall().isLastParameterEmpty()
                    or lastProcedureCall().rawParameters().last().stat() == Stat::Word)
            {
                unknownString = tclInterpreter.readCurrentKeyword();
                if(addNewParameter(unknownString) == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{  // Any other stat is parameter without whitespace after parameter Ex. "abc"abc Expection [puts abc]abc - ComplexWord
                switch(lastProcedureCall().rawParameters().last().stat()){
                // Error Cases
                case Stat::BracesStart:
                case Stat::DoubleQuotes:
                case Stat::Script:
                    return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
                case Stat::ComplexWord:
                    return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
                default:
                    break;
                }
                // execute special Backslash control proceudre
                // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
                if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                    return throwError(ERROR_PREFIX + error());
                }
                return interpret();
            }
        }
            break;
        case Stat::Whitespace:
        {
            // Create new empty Word parameter
            return addNewParameter();
        }
        default:
            return throwError(ERROR_PREFIX + "Forbidden Stat");
        }
    }
    /*    break;
    }
*/

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::CommandSubbing>: ";

    switch(processingStat()){
    case Stat::Word:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    {
        if(lastProcedureCall().isLastParameterEmpty()
                or lastProcedureCall().rawParameters().last().stat() == Stat::Word){
            return newParameterProcessing();
        }else{
            switch(lastProcedureCall().rawParameters().last().stat()){
            // Error Cases
            case Stat::BracesStart:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            case Stat::ComplexWord:
                return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
            default:
                break;
            }
            // execute special Backslash control proceudre
            // - Create New Call ComplexWord with first parameter (move current parameter to first parameter of ComplexWord)
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                return throwError(ERROR_PREFIX + error());
            }
            return interpret();
        }
    }
    case Stat::CommandSubbingStart:
    {
        if(lastProcedureCall().rawParametersLength() == 1){
            return throwError(ERROR_PREFIX + "CommandSubbing in procedure name");
        }
        if(not lastProcedureCall().isLastParameterEmpty()){

            switch(lastProcedureCall().rawParameters().last().stat()){

            // Error Cases
            case Stat::BracesStart:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));

            case Stat::ComplexWord:
                return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
            default:
            {
                break;
            }
            }
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error
                    or interpret() == Error::Error)
            {
                return throwError(ERROR_PREFIX + error());
            }

        }else{
            return createCall(Stat::CommandSubbing);
        }
    }
        break;
    case Stat::DoubleQuotes:
    case Stat::BracesStart:
    {
        if(lastProcedureCall().isLastParameterEmpty()){
            // Change to processing stat
            // Call Processing State Procedure call
            return createCall(processingStat());
        }else{
            switch(lastProcedureCall().rawParameters().last().stat()){
            // Error Cases
            case Stat::BracesStart:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            case Stat::ComplexWord:
                return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
            default:
                return newParameterProcessing();
            }
        }
    }
    case Stat::VariableSubbing:
    {
        if(lastProcedureCall().rawParametersLength() == 1){
            return throwError(ERROR_PREFIX + "VariableSubbing in procedure name");
        }
        if(not lastProcedureCall().isLastParameterEmpty()){
            switch(lastProcedureCall().rawParameters().last().stat()){            
                // Error Cases
                case Stat::BracesStart:
                case Stat::DoubleQuotes:
                case Stat::Script:
                    return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
                case Stat::ComplexWord:
                    return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
                default:
                    break;
            }            
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error
                    or interpret() == Error::Error)
            {
                return throwError(ERROR_PREFIX + error());
            }
        }else{
            startVariableSubbing();
        }
   }
        break;
    case Stat::BackslashSubbing:
    {
        if(not lastProcedureCall().isLastParameterEmpty()){
            switch(lastProcedureCall().rawParameters().last().stat()){            
            // Error Cases
            case Stat::ComplexWord:
                return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
            default:
                break;
            }
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error
                    or interpret() == Error::Error)
            {
                return throwError(ERROR_PREFIX + error());
            }
        }else{
            if(processBackslashSubbing() == Error::Error)
                return throwError(ERROR_PREFIX + error());
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
        if(isNotCommandSubbing()){
            // - Interpret as Word parameter (new parameter)
            return newParameterProcessing();
        }else{
            return finalizeCall();
        }
    }
    case Stat::Braces:
    {
        if(isNotCommandSubbingMainScriptOnly()){
            return newParameterProcessing();
        }else{
            if((procedureCalls.rbegin() + 1)->stat() == Stat::Script){  // Prelast stat == Script
                if(finalizeCall() == Error::Error
                        or interpret() == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{
                return newParameterProcessing();
            }
        }
    }
        break;
    case Stat::Semicolon:
    case Stat::EndOfString:
    {
        if(isNotCommandSubbing()){
            if(lastProcedureCall().isLastParameterEmpty() and lastProcedureCall().rawParameters().length() <= 1){
                return throwError(ERROR_PREFIX + "Impossible case");
                // - Act as whitespace for 0 parameters (check newParameterProcedure)
            }
            // - Just finalize call (default complete call)
            return finalizeCall();
        }else{
            if(lastProcedureCall().isLastParameterEmpty()){
                if(lastProcedureCall().rawParameters().length() > 1){
                    // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation
                    return throwError(ERROR_PREFIX + "Finalize (true CommandSubbing) by EndOfString or Semicolon");
                }else{
                    // Possible case - just ignore
                    // - Act as whitespace for 0 parameters (check newParameterProcedure)
                    return throwError(ERROR_PREFIX + "Empty Command Subbing  []");
                }
            }
            // - throw error (try to finalize (true CommandSubbing) by EndOfString) - no implemntation
            return throwError(ERROR_PREFIX + "Finalize (true CommandSubbing) by EndOfString or Semicolon");
        }
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::constructor_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::CommandSubbing>: ";

    if(isNotCommandSubbing() and not isMainScript()){
        tclInterpreter.predefinitionsController.newGroup(procedureCalls.count());
    }

    callConfig.setCommandSubbingNewParameter_procedureNameMode();
    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbingStart>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::CommandSubbingStart>: ";

    // CHECK FOR FINALIZE OF SPECIFIC CALLS vvvvvv
    // Verify if procedure name is "possible" (Only Word are OK. For ComplexWord, Braces, DoubleQuotes and BackslashSubbing you need to check params)
    // Impossible procedure name contains:
    // - CommandSubbing
    // - VariableSubbing
    // - BackslashSubbing (with special meaning) ???? Or NO

    // Create simplfied procedure name and assign to OutputCommand

    qDebug() << lastProcedureCall().lastParameter().rawCommand();
    qDebug() << lastProcedureCall().lastParameter().outputCommand();
    if(callDefinition(lastProcedureCall()._name()) == Error::Error)
        return throwError(ERROR_PREFIX + error());

    callConfig.setCommandSubbingNewParameter_parametersMode();

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::CommandSubbing>: ";

    switch(lastProcedureCall().lastParameter().stat()){
    case Stat::Word:
    case Stat::BackslashSubbing:
    {
        lastProcedureCall().lastParameter().setOutputCommand("\"" + lastProcedureCall().lastParameter().rawCommand() + "\"");
    }
        break;
    case Stat::VariableSubbing:
    {
        lastProcedureCall().lastParameter().setRawCommand("$" + lastProcedureCall().lastParameter().rawCommand());
    }
        break;    
    default:
        break;
    }


    if(performRulesCheckForNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    return Error::NoError;
}

void CallConfig::setCommandSubbingNewParameter_procedureNameMode(){
    commandCallSpecialFunctions[Settings::specialCallStat2number(Stat::CommandSubbing)].newParameter =
        &TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbingStart>;
}

void CallConfig::setCommandSubbingNewParameter_parametersMode(){
    commandCallSpecialFunctions[Settings::specialCallStat2number(Stat::CommandSubbing)].newParameter =
        &TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbing>;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::Common>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    lastProcedureCall().parameters().removeLast(); // Shall be empty after previous procedure

    if(finalizeProcedureCall() == Error::Error)
        return throwError(ERROR_PREFIX + error());

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
        return finalizeCallProcessing();
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
        if(processBackslashSubbing() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;    
    case Stat::BracesStart:
        if(tclInterpreter.listController().incrementListLevel() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    break; case Stat::VariableSubbing:
    case Stat::Word:
    case Stat::CommandSubbingStart:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::Whitespace:
    case Stat::DoubleQuotes:
    case Stat::EndOfString:
    case Stat::Semicolon:
    {
        unknownString = tclInterpreter.readCurrentKeyword();
        switch(lastProcedureCall().lastParameter().stat()){
        case Stat::Word:
        case Stat::BackslashSubbing:
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat");
        }
        if(addNewParameter(unknownString) == Error::Error)
            return throwError(ERROR_PREFIX + error());
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
        if(tclInterpreter.listController().incrementListLevel() == Error::Error)
            return throwError(ERROR_PREFIX + error());
        if(tclInterpreter.listController().isListClosed()){ // - Control deactived or list closed - finalize
            if(finalizeCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());            
            // If prelast parameter is not complete VariableSubbing

        }else{
            if(addNewParameter(QString("}")) == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::constructor_mode<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::BracesStart>: ";
    if(tclInterpreter.listController().incrementListLevel() == Error::Error)
        return throwError(ERROR_PREFIX + error());
    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::BracesStart>: ";


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::BracesStart>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::BracesStart>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    lastProcedureCall().parameters().removeLast(); // Shall be empty after previous procedure

    // If first parameter is VariableSubbing, change call to VariableSubbing ,remove that parameter and activate bracesListControl
    if(not lastProcedureCall().parameters().isEmpty()){
        if(lastProcedureCall().parameters().front().stat() == Stat::VariableSubbing){
            lastProcedureCall().parameters().removeFirst();
            lastProcedureCall().changeStat(Stat::VariableSubbing);
            tclInterpreter.listController().activateListLevelControl();

        }
    }
    lastProcedureCall().setOutputCommand(lastProcedureCall().rawCommand());
    lastProcedureCall().setRawCommand("{" + lastProcedureCall().rawCommand() + "}");

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
    case Stat::BracesStart:
        return newParameterProcessing();
    case Stat::CommandSubbingStart:
        return newCallProcessing();
    case Stat::DoubleQuotes:
        // End of Double Quotes
        return finalizeCallProcessing();
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
        switch(lastProcedureCall().lastParameter().stat()){
        case Stat::Word:
        case Stat::BackslashSubbing:
        case Stat::VariableSubbing:
        case Stat::CommandSubbing:
        {
            if(addNewParameter(unknownString) == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat ");
        }
    }
        break;
    case Stat::BackslashSubbing:
    {
        switch(lastProcedureCall().lastParameter().stat()){
        case Stat::Word:
        case Stat::BackslashSubbing:
        case Stat::VariableSubbing:
        case Stat::CommandSubbing:
        {
            // Interpret then add as parameter
            if(processBackslashSubbing() == Error::Error)
                return throwError(ERROR_PREFIX + error());
        }
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat ");
        }
    }
        break;
    case Stat::VariableSubbing:
    {        
        switch(lastProcedureCall().lastParameter().stat()){
        case Stat::Word:
        case Stat::BackslashSubbing:
        case Stat::VariableSubbing:
        case Stat::CommandSubbing:
        {
            startVariableSubbing();
        }
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Stat ");
        }
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
        if(addNewParameter() == Error::Error
                or  createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
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
        if(finalizeCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::constructor_mode<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::DoubleQuotes>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::DoubleQuotes>: ";

    switch(lastProcedureCall().lastParameter().stat()){
    case Stat::VariableSubbing:
    {
        lastProcedureCall().lastParameter().setRawCommand("$" + lastProcedureCall().lastParameter().rawCommand());
    }
        break;
    default:
        break;
    }

    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::destructor_mode<Stat::DoubleQuotes>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::DoubleQuotes>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    lastProcedureCall().parameters().removeLast(); // Shall be empty after previous procedure

    lastProcedureCall().setRawCommand("\"" + lastProcedureCall().rawCommand() + "\"");

    if(prepareSnprintf() == Error::Error)
        return throwError(ERROR_PREFIX + error());

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
    case Stat::BackslashSubbing:
    case Stat::BracesStart:
        return newParameterProcessing();
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
    case Stat::CommandSubbingEnd:
        // End of Complex Word
        return finalizeCallProcessing();
    case Stat::CommandSubbingStart:
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
        if(processBackslashSubbing() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::DoubleQuotes:
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Braces:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::BracesStart:
    {
        unknownString = tclInterpreter.readCurrentKeyword();

        if(addNewParameter(unknownString) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::VariableSubbing:
    {
        startVariableSubbing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }


    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::ComplexWord>: ";

    switch(processingStat()){
    case Stat::CommandSubbingStart:
    {
        if(addNewParameter() == Error::Error
                or createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::ComplexWord>: ";

    switch(processingStat()){
    case Stat::CommandSubbingEnd:
        if(isNotCommandSubbingInComplexWord())
            return newParameterProcessing();
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
    {
        // Finalize and add new empty parameter
        if(finalizeCall() == Error::Error
                or interpret() == Error::Error){
           return throwError(ERROR_PREFIX + error());
        }
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::constructor_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::ComplexWord>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::ComplexWord>: ";

    switch(lastProcedureCall().lastParameter().stat()){
    case Stat::Word:
    case Stat::BackslashSubbing:
    {
        lastProcedureCall().lastParameter().setOutputCommand("\"" + lastProcedureCall().lastParameter().rawCommand() + "\"");
    }
        break;
    case Stat::VariableSubbing:
    {
        lastProcedureCall().lastParameter().setRawCommand("$" + lastProcedureCall().lastParameter().rawCommand());
    }
        break;
    default:
        break;
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::ComplexWord>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::ComplexWord>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    lastProcedureCall().parameters().removeLast(); // Shall be empty after previous procedure

    if(prepareSnprintf() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::BracesStartExprOnly>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::BracesStartExprOnly>: ";
    switch(processingStat()){
    case Stat::BackslashSubbing:
        return throwError(ERROR_PREFIX + "Backslash subbing is forbidden in expr command");
    case Stat::Comment:
    case Stat::Word:
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::CommandSubbingEnd:
    case Stat::Semicolon:
    case Stat::EndOfString:
    case Stat::Whitespace:
        return newParameterProcessing();
    case Stat::CommandSubbingStart:
    case Stat::BracesStart:
    case Stat::DoubleQuotes:
        return newCallProcessing();
    case Stat::Braces:
        return finalizeCallProcessing();
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::BracesStartExprOnly>(){
    const QString ERROR_PREFIX = "New Parameter Error BracesStartExprOnly: ";
    QString unknownString;

    switch(processingStat()){
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::Whitespace:
    case Stat::Semicolon:
    case Stat::EndOfString:
    {
        // Append if last parameter is Word or VariableSubbing
        unknownString = tclInterpreter.readCurrentKeyword();
        if(addNewParameter(unknownString) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    /*case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        if(processBackslashSubbing() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;*/
    case Stat::VariableSubbing:
    {
        startVariableSubbing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }    

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::BracesStartExprOnly>(){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::BracesStartExprOnly>: ";

    switch(processingStat()){
    case Stat::BracesStart:
    case Stat::DoubleQuotes:
    {
        if(addNewParameter() == Error::Error
                or createCall(processingStat()) == Error::Error)
        return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::CommandSubbingStart:
    {
        if(addNewParameter() == Error::Error
                or  createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::BracesStartExprOnly>(){
    const QString ERROR_PREFIX = "Finalize Call Special Command <Stat::BracesStartExprOnly>: ";

    switch(processingStat()){
    case Stat::Braces:
    {
        if(finalizeCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::constructor_mode<Stat::BracesStartExprOnly>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::BracesStartExprOnly>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::BracesStartExprOnly>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::BracesStartExprOnly>: ";

    using ExecutableActionType = Definition::Rule::ExecutableActions::Type::ActionType;
    (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<ExecutableActionType>>(ExecutableActionType::ExprProcessParameter)]))({});

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::BracesStartExprOnly>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::BracesStartExprOnly>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    lastProcedureCall().parameters().removeLast(); // Shall be empty after previous procedure

    // If first parameter is VariableSubbing, change call to VariableSubbing ,remove that parameter and activate bracesListControl
    /*if(not lastProcedureCall().parameters().isEmpty()){
        if(lastProcedureCall().parameters().front().stat() == Stat::VariableSubbing){
            lastProcedureCall().parameters().removeFirst();
            lastProcedureCall().changeStat(Stat::VariableSubbing);
            tclInterpreter.listController().activateListLevelControl();

        }
    }*/
    /*lastProcedureCall().setOutputCommand(lastProcedureCall().rawCommand());
    lastProcedureCall().setRawCommand("{" + lastProcedureCall().rawCommand() + "}");*/

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::DoubleQuotesExprOnly>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::DoubleQuotesExprOnly>: ";
    switch(processingStat()){
    case Stat::BackslashSubbing:
    case Stat::Comment:
    case Stat::Word:
    case Stat::VariableSubbing:
    case Stat::Namespace:
    case Stat::CommandSubbingEnd:
    case Stat::Semicolon:
    case Stat::EndOfString:
    case Stat::Whitespace:
    case Stat::Braces:
        return newParameterProcessing();
    case Stat::CommandSubbingStart:
    case Stat::BracesStart:
        return newCallProcessing();
    case Stat::DoubleQuotes:
        return finalizeCallProcessing();
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::DoubleQuotesExprOnly>(){
    const QString ERROR_PREFIX = "New Parameter Error DoubleQuotesExprOnly: ";
    QString unknownString;

    switch(processingStat()){
    case Stat::Word:
    case Stat::CommandSubbingEnd:
    case Stat::Namespace:
    case Stat::Comment:
    case Stat::Whitespace:
    case Stat::Semicolon:
    case Stat::EndOfString:
    case Stat::Braces:
    {
        // Append if last parameter is Word or VariableSubbing
        unknownString = tclInterpreter.readCurrentKeyword();
        if(addNewParameter(unknownString) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::BackslashSubbing:
    {
        // Interpret then add as parameter
        if(processBackslashSubbing() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::VariableSubbing:
    {
        startVariableSubbing();
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::DoubleQuotesExprOnly>(){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::DoubleQuotesExprOnly>: ";

    switch(processingStat()){
    case Stat::BracesStart:
    {
        if(addNewParameter() == Error::Error
                or createCall(processingStat()) == Error::Error)
        return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::CommandSubbingStart:
    {
        if(addNewParameter() == Error::Error
                or  createCall(Stat::CommandSubbing) == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::DoubleQuotesExprOnly>(){
    const QString ERROR_PREFIX = "Finalize Call Special Command <Stat::DoubleQuotesExprOnly>: ";

    switch(processingStat()){
    case Stat::DoubleQuotes:
    {
        if(finalizeCall() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Unknown Stat ");
    }

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::DoubleQuotesExprOnly>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::DoubleQuotesExprOnly>: ";

    switch(lastProcedureCall().lastParameter().stat()){
    case Stat::VariableSubbing:
    {
        lastProcedureCall().lastParameter().setRawCommand("$" + lastProcedureCall().lastParameter().rawCommand());
    }
        break;
    default:
        break;
    }

    using ExecutableActionType = Definition::Rule::ExecutableActions::Type::ActionType;
    (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<ExecutableActionType>>(ExecutableActionType::ExprProcessParameter)]))({});

    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::destructor_mode<Stat::DoubleQuotesExprOnly>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::DoubleQuotesExprOnly>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::ComplexWordExprOnly>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::ComplexWordExprOnly>: ";

    switch(lastProcedureCall().lastParameter().stat()){
    case Stat::Word:
    case Stat::BackslashSubbing:
    {
        lastProcedureCall().lastParameter().setOutputCommand("\"" + lastProcedureCall().lastParameter().rawCommand() + "\"");
    }
        break;
    case Stat::VariableSubbing:
    {
        lastProcedureCall().lastParameter().setRawCommand("$" + lastProcedureCall().lastParameter().rawCommand());
    }
        break;
    default:
        break;
    }

    using ExecutableActionType = Definition::Rule::ExecutableActions::Type::ActionType;
    (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<ExecutableActionType>>(ExecutableActionType::ExprProcessParameter)]))({});

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::ComplexWordExprOnly>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::ComplexWordExprOnly>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    return Error::NoError;
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
        QString tempStr = tclInterpreter.textInterpreter().restOfString();
        if(tempStr.endsWith("\\")){
            tempStr.chop(1);
            tclInterpreter.setCommentInterpreterMode();
        }
        if(isMainScript())
            tclInterpreter.addExpressionToMainCodeBlock({"//" + tempStr + "\n"});
        else
            tclInterpreter.addExpressionToCodeBlock({"//" + tempStr + "\n"});

        tclInterpreter.setProcessingStat(Stat::EndOfString);
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
    case Stat::EndOfString:
        if(isMainScript()){
            if(userConfig.proceduresSettings().mode() != Settings::InterpreterMode::TestCaseReport){
                tclInterpreter.addExpressionToMainCodeBlock({"\n"});
            }
        }else{  // Add new Line
            if(userConfig.proceduresSettings().mode() == Settings::InterpreterMode::TestCase){
                lastProcedureCall().lastParameter().rawCommand().append("\n");
            }
        }
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
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Script>: ";

    return (createCall(Stat::CommandSubbing) == Error::Error
            or interpret() == Error::Error)? Error::Error : Error::NoError;
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::Script>: ";
    // If main script, newCall_mode
    if(isMainScript()){
        return newCallProcessing();
    }else{
        return finalizeCall();
    }

    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::constructor_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::Script>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::Script>: ";

    switch(lastProcedureCall().lastParameter().stat()){
    case Stat::Word:
    {
        lastProcedureCall().rawCommand().append(lastProcedureCall().lastParameter().rawCommand());
        lastProcedureCall().outputCommand().append(lastProcedureCall().lastParameter().rawCommand());
    }
        break;
    case Stat::CommandSubbing:
    {
        if(not tclInterpreter.preexpressions().isEmpty()){ // Not Empty
            if(tclInterpreter.preexpressions().size() > 0){
                lastProcedureCall().outputCommand().append(tclInterpreter.preexpressions().join("\n") + "\n");
            }
            //lastSavedStat().appendCommand("\n");
            tclInterpreter.preexpressions().clear();
        }
        lastProcedureCall().rawCommand().append(lastProcedureCall().lastParameter().rawCommand() + "\n");
        lastProcedureCall().outputCommand().append(lastProcedureCall().lastParameter().outputCommand());
    }
        break;
    default:
        return throwError(ERROR_PREFIX + "Script parameter is invalid");
    }

    lastProcedureCall().parameters().removeLast();
    if( lastProcedureCall().newParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::Script>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::Script>: ";

    if(addNewParameter() == Error::Error)
        return throwError(ERROR_PREFIX + error());

    lastProcedureCall().parameters().removeLast(); // Shall be empty after previous procedure

    lastProcedureCall().setOutputCommand("{\n" + lastProcedureCall().outputCommand() + "}");
    lastProcedureCall().setRawCommand("{" + lastProcedureCall().rawCommand() + "}");
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
        break;
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


template<>
Error TclProcedureInterpreter::constructor_mode<Stat::Ignore>(){
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::Ignore>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::destructor_mode<Stat::Ignore>(){
    const QString ERROR_PREFIX = "Destructor Special Command <Stat::Ignore>: ";

    // Maybe toString with Raw target - createRawString?

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::interpret_mode<Stat::VariableSubbing>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::VariableSubbing>: ";
    if(isVariableSubbingProcessingJustActivated())
    {
        switch(processingStat()){
        case Stat::BracesStart:
        {
            // Variable Subbing confirmed -> Return to "true" call to confirm previous parameter -> return back to VariableSubbing
            updateCurrentCallProcedures();
            if(addNewParameter() == Error::Error){
                // Error recovery is not nessary
                return throwError(ERROR_PREFIX + error());
            }
            startVariableSubbing();
            if(createCall(Stat::BracesStart) == Error::Error
                    or addNewParameter(Stat::VariableSubbing) == Error::Error
                    or addNewParameter() == Error::Error)
            {
                updateCurrentCallProcedures(); // Error Recovery
                return throwError(ERROR_PREFIX + error());
            }
            deactivateBackslashSubbing();   // Backslash subbing doesnt work for VariableSubbing Braces pattern
            break;
        }
        default:
            if(lastProcedureCall().lastParameter().stat() == Stat::Word
                    and not lastProcedureCall().lastParameter().isEmpty()) // Not empty Word
            {
                if(not isFirstSignOk(tclInterpreter.readCurrentKeyword())) // First incorrect - Dont create VariableSubbing
                {   // Save current ProcessingStat -> Set processingStat = Word , savedStr = "$" and setSavedKeywordMode
                    // Update and interpret for above conditions
                    // Set processingStat = savedProcessingStat and setStandardKeywordMode
                    // Interpret current processingStat
                    Stat currentProcessingStat = processingStat();
                    tclInterpreter.setProcessingStat(Stat::Word);
                    tclInterpreter.saveKeyword("$");
                    tclInterpreter.setSavedKeywordReadMode();
                    updateCurrentCallProcedures();
                    if(interpret() == Error::Error){
                        tclInterpreter.setStandardReadKeywordMode();
                        return throwError(ERROR_PREFIX + error());
                    }
                    tclInterpreter.setProcessingStat(currentProcessingStat);
                    tclInterpreter.setStandardReadKeywordMode();
                    if(interpret() == Error::Error){
                        tclInterpreter.setStandardReadKeywordMode();
                        return throwError(ERROR_PREFIX + error());
                    }
                    break; // Processing is over
                }
            }
            // Variable Subbing confirmed -> Return to "true" call to confirm previous parameter -> return back to VariableSubbing
            updateCurrentCallProcedures();
            if(addNewParameter(Stat::VariableSubbing) == Error::Error){
                // Error recovery is not nessary
                return throwError(ERROR_PREFIX + error());
            }
            startVariableSubbing();
            if(processVariableSubbing() == Error::Error)
            {
                updateCurrentCallProcedures(); // Error Recovery
                return throwError(ERROR_PREFIX + error());
            }
        }
        tclInterpreter.variableController().setJustActivated(false);
    }else{
        if(processVariableSubbing() == Error::Error){
            updateCurrentCallProcedures(); // Error Recovery
            return throwError(ERROR_PREFIX + error());
        }
    }
    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::VariableSubbing>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    return throwError(ERROR_PREFIX + "No implementation for new parameter for VariableSubbing");
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::VariableSubbing>(){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::VariableSubbing>: ";
    return throwError(ERROR_PREFIX + "No implementation for new call for Braces");
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::VariableSubbing>(){
    const QString ERROR_PREFIX = "Finalize Call Special Command <Stat::VariableSubbing>: ";
    return throwError(ERROR_PREFIX + "No implementation for finalize call for VariableSubbing");
}


template<>
Error TclProcedureInterpreter::interpret_mode<Stat::BackslashSubbing>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::BackslashSubbing>: ";
    QString specialSignStr = "\\\n";
    bool loopBreak = false;
    while(tclInterpreter.textInterpreter().isCurrentChar() and not loopBreak){    // Character available - no end of string
        switch(tclInterpreter.textInterpreter().currentCharForSpecialSign().toLatin1()){
        case ' ':
        case '\t':
            specialSignStr.append(tclInterpreter.textInterpreter().currentCharForSpecialSign());
            tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
            break;
        default:
            loopBreak = true;
            break;
        }
    }

    if(addNewParameter() == Error::Error/*
            or addNewParameter(Stat::BackslashSubbing, specialSignStr) == Error::Error*/)
    {
        updateCurrentCallProcedures();
        tclInterpreter.setStandardInterpreterMode(); // Recovery
        return throwError(ERROR_PREFIX + error());
    }
    updateCurrentCallProcedures();

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameterProcessing_mode<Stat::BackslashSubbing>(){
    const QString ERROR_PREFIX = "New Parameter Error: ";
    return throwError(ERROR_PREFIX + "No implementation for new parameter for BackslashSubbing");
}

template<>
Error TclProcedureInterpreter::newCall_mode<Stat::BackslashSubbing>(){
    const QString ERROR_PREFIX = "New Call Special Command <Stat::BackslashSubbing>: ";
    return throwError(ERROR_PREFIX + "No implementation for new call for Braces");
}

template<>
Error TclProcedureInterpreter::finalizeCall_mode<Stat::BackslashSubbing>(){
    const QString ERROR_PREFIX = "Finalize Call Special Command <Stat::BackslashSubbing>: ";
    return throwError(ERROR_PREFIX + "No implementation for finalize call for BackslashSubbing");
}


CallConfig::CallConfig()
    :commandCallSpecialFunctions
    {
         //CommandSubbing
         {
             &TclProcedureInterpreter::interpret_mode<Stat::CommandSubbing>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::CommandSubbing>,
             &TclProcedureInterpreter::newCall_mode<Stat::CommandSubbing>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::CommandSubbing>,
             &TclProcedureInterpreter::constructor_mode<Stat::CommandSubbing>,
             &TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbingStart>,
             &TclProcedureInterpreter::destructor_mode<Stat::CommandSubbing>,
         },
         //BracesStart
         {
             &TclProcedureInterpreter::interpret_mode<Stat::BracesStart>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::BracesStart>,
             &TclProcedureInterpreter::newCall_mode<Stat::BracesStart>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::BracesStart>,
             &TclProcedureInterpreter::constructor_mode<Stat::BracesStart>,
             &TclProcedureInterpreter::newParameter_mode<Stat::BracesStart>,
             &TclProcedureInterpreter::destructor_mode<Stat::BracesStart>,
         },
         //DoubleQuotes
         {
             &TclProcedureInterpreter::interpret_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::newCall_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::constructor_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
             &TclProcedureInterpreter::destructor_mode<Stat::DoubleQuotes>,
         },
         //ComplexWord
         {
             &TclProcedureInterpreter::interpret_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::newCall_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::constructor_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
             &TclProcedureInterpreter::destructor_mode<Stat::ComplexWord>,
         },/*
         //Expression
         {
             &TclProcedureInterpreter::interpret_mode<Stat::Expression>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Expression>,
             &TclProcedureInterpreter::newCall_mode<Stat::Expression>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::Expression>,
             &TclProcedureInterpreter::constructor_mode<Stat::Common>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
             &TclProcedureInterpreter::destructor_mode<Stat::Common>,
         },*/
         //Script
         {
             &TclProcedureInterpreter::interpret_mode<Stat::Script>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Script>,
             &TclProcedureInterpreter::newCall_mode<Stat::Script>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::Script>,
             &TclProcedureInterpreter::constructor_mode<Stat::Script>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Script>,
             &TclProcedureInterpreter::destructor_mode<Stat::Script>,
         },
         //BracesStartExprOnly
         {
             &TclProcedureInterpreter::interpret_mode<Stat::BracesStartExprOnly>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::BracesStartExprOnly>,
             &TclProcedureInterpreter::newCall_mode<Stat::BracesStartExprOnly>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::BracesStartExprOnly>,
             &TclProcedureInterpreter::constructor_mode<Stat::BracesStartExprOnly>,
             &TclProcedureInterpreter::newParameter_mode<Stat::BracesStartExprOnly>,
             &TclProcedureInterpreter::destructor_mode<Stat::BracesStartExprOnly>,
         },
         //DoubleQuotesExprOnly
         {
             &TclProcedureInterpreter::interpret_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::newCall_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::constructor_mode<Stat::DoubleQuotes>,
             &TclProcedureInterpreter::newParameter_mode<Stat::DoubleQuotesExprOnly>,
             &TclProcedureInterpreter::destructor_mode<Stat::DoubleQuotesExprOnly>,
         },
         //ComplexWordExprOnly
         {
             &TclProcedureInterpreter::interpret_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::newCall_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::constructor_mode<Stat::ComplexWord>,
             &TclProcedureInterpreter::newParameter_mode<Stat::ComplexWordExprOnly>,
             &TclProcedureInterpreter::destructor_mode<Stat::ComplexWordExprOnly>,
         },
             /*
         //ExprCommand
         {
             &TclProcedureInterpreter::interpret_mode<Stat::ExprCommand>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::ExprCommand>,
             &TclProcedureInterpreter::newCall_mode<Stat::ExprCommand>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::ExprCommand>,
             &TclProcedureInterpreter::constructor_mode<Stat::Common>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
             &TclProcedureInterpreter::destructor_mode<Stat::Common>,
         },*/
         //Ignore
         {
             &TclProcedureInterpreter::interpret_mode<Stat::Ignore>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Ignore>,
             &TclProcedureInterpreter::newCall_mode<Stat::Ignore>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::Ignore>,
             &TclProcedureInterpreter::constructor_mode<Stat::Ignore>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
             &TclProcedureInterpreter::destructor_mode<Stat::Ignore>,
         },
         //VariableSubbing
         {
             &TclProcedureInterpreter::interpret_mode<Stat::VariableSubbing>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::VariableSubbing>,
             &TclProcedureInterpreter::newCall_mode<Stat::VariableSubbing>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::VariableSubbing>,
             &TclProcedureInterpreter::constructor_mode<Stat::Common>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
             &TclProcedureInterpreter::destructor_mode<Stat::Common>,
         },
         //BackslashSubbing
         {
             &TclProcedureInterpreter::interpret_mode<Stat::BackslashSubbing>,
             &TclProcedureInterpreter::newParameterProcessing_mode<Stat::BackslashSubbing>,
             &TclProcedureInterpreter::newCall_mode<Stat::BackslashSubbing>,
             &TclProcedureInterpreter::finalizeCall_mode<Stat::BackslashSubbing>,
             &TclProcedureInterpreter::constructor_mode<Stat::Common>,
             &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
             &TclProcedureInterpreter::destructor_mode<Stat::Common>,
         },
         // SafeGuard
         {
             &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
             &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
             &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
             &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
             &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
             &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
             &TclProcedureInterpreter::interpretSpecialCommandCall_throwErrorForWrongStat,
         },
    }
{

}


const QString
CallConfig::commandCallParameterSeparators
[Tcl::Interpreter::Core::numbOfSpecialCommandCallsAndSafeguard()] =
{
    //CommandSubbing
    QString(" "),
    //BracesStart
    QString(),
    //DoubleQuotes
    QString(),
    //ComplexWord
    QString(),
    /*
    //Expression
    {
        &TclProcedureInterpreter::interpret_mode<Stat::Expression>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Expression>,
        &TclProcedureInterpreter::newCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },*/
    //Script
    QString(), // Specified at newParameter call
    /*
    //ExprCommand
    {
        &TclProcedureInterpreter::interpret_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::newCall_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },*/
    //Ignore
     QString(),
    //VariableSubbing
    QString(),
    //BackslashSubbing
    QString(),
    // SafeGuard
    QString()
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
    // Preverification
    // If Stat id for semi command call stats is correct (If wrong stat , stat id of Stat::Size is returnedgh)
    if(Settings::specialCallStat2number(stat) == Settings::specialCallStat2number(Stat::Size))
        return throwError("Wrong stat for CreateCall procedure. Stat: " + QString::number(TCLInterpreter::cast_stat(stat)));

    // Replacer Call controller
    // Call rules to eventually modify behaviour of interpreter
    // - Usable only in CommandSubbing calls
    // - Use only replacer actions
    // - check result and proceed
    using CallReplacerData = CallReplacer::Controller::ControllerData;
    CallReplacerData callReplacerData(stat);
    callReplacerController.performReplacerRuleCheckForNewCall(*this, callReplacerData);
    stat = callReplacerData.stat();
    // ------------------------------------

    if(stat != Stat::CommandSubbing and not procedureCalls.empty()){
            // TODO: ?????????? First brush up why, if no effects Debug maybe
            procedureCalls.append(Call(stat, parameter, lastProcedureCall().definition(),(parameter.isEmpty())? lastProcedureCall().lastArgumentIndex() : lastProcedureCall().lastArgumentIndex() + 1));
   }else{
        procedureCalls.append(Call(stat, parameter));
    }
    // Change current methods for Call processing
    updateCurrentCallProcedures();
    if(constructor() == Error::Error){
        // Failed to initialize Call - Remove last call +  update call procedures
        // Other recovery procedures shall be perform in constructor procedure
        //procedureCalls.removeLast();
        //updateCurrentCallProcedures();
        return Error::Error;
    }

    return Error::NoError;
}

Error Controller::createCallAndMoveLastParameterToOne(Stat stat){
    if(procedureCalls.last().isLastParameterEmpty())
        return throwError("CreateAndMove procedure: No parameters or empty parameter");
    return createCall(stat, lastProcedureCall().rawParameters().takeLast());
}


Error Controller::throwError(){return tclInterpreter.throwError();}
Error Controller::throwError(const QString str){return tclInterpreter.throwError(str);}
const QString& Controller::error(){return tclInterpreter.error();}

bool Controller::isVariableSubbingProcessingJustActivated(){
    return tclInterpreter.variableController().isJustActivated();
}

Error Controller::processVariableSubbing(){
    QString str = tclInterpreter.readCurrentKeyword();
    tclInterpreter.variableController().processVariableSubbingParameter(procedureCalls.last().lastParameter(), str);

    if(not str.isEmpty()){
        // Finalize VariableSubbing processing (update procedures)
        updateCurrentCallProcedures();
        // If first sign is ( sign, throw error to analyze case of $<name>(..) VaraibleSubbing Pattern
        if(str.front() == '('){
            return throwError("$<name>(..) VariableSubbing Pattern detected - contact with author of app");
        }

        // If last parameter(VariableSubbing) is empty, recovery procedure is required
        if(procedureCalls.last().isLastParameterEmpty()){ // Variable Subbing Empty
            return throwError("Impossible case. Empty Variable Subbing");
        }else{
            // Otherwise New parameter required
            if(processingStat() == Stat::Word){
                tclInterpreter.saveKeyword(str);
                tclInterpreter.setSavedKeywordReadMode();
                if(interpret() == Error::Error){
                    tclInterpreter.setStandardReadKeywordMode();
                    return throwError(error());
                }
                tclInterpreter.setStandardReadKeywordMode();
            }else{
                if(interpret() == Error::Error){
                    return throwError(error());
                }
            }
        }
    }

    return Error::NoError;
}

bool Controller::isFirstSignOk(QString str){
    return tclInterpreter.variableController().isFirstSignOk(str);
}

void Controller::startVariableSubbing(){
    updateCurrentCallProcedures(Stat::VariableSubbing);
    tclInterpreter.variableController().setJustActivated(true);
}

Error Controller::processBackslashSubbing(){
    QString specialSignStr;
    bool characterIncrementRequired = true;
    // Check and prepare specialSignStr
    if(backslashSubbingActive == false){
        return addNewParameter(QString("\\"));
    }

    if(tclInterpreter.textInterpreter().isCurrentChar()) // If not accesible -> End of string that means EndOfLine-BackslashSubbing is required
    {
        switch(lastProcedureCall().lastParameter().stat()){
        // Error Cases
        case Stat::BracesStart:
        case Stat::DoubleQuotes:
        case Stat::Script:
            return throwError("No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
        case Stat::ComplexWord:
            return throwError("Impossible case. Unfinished ComplexWord.");
        default:
            break;
        }

        if(not procedureCalls.isEmpty() and procedureCalls.last().stat() == Stat::BracesStart){
            return addNewParameter(QString("\\"));
        }
        switch (tclInterpreter.textInterpreter().currentCharForSpecialSign().toLatin1()) {
        case 'x': // x01
        {
            specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
            tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
            if(tclInterpreter.textInterpreter().isCurrentChar() and     // 0
                    QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-9a-fA-F]")) != -1)
            {
                specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                if(tclInterpreter.textInterpreter().isCurrentChar() and  // 1
                        QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-9a-fA-F]")) != -1)
                {
                    specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                    if(addNewParameter() == Error::Error
                            or addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr,
                                               QString("\\") + specialSignStr) == Error::Error)
                    {
                        return Error::Error;
                    }
                }else{
                    if(addNewParameter() == Error::Error
                            or addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr,
                                               QString("\\") + specialSignStr) == Error::Error)
                    {
                        return Error::Error;
                    }
                    characterIncrementRequired = false;
                }
            }else{
                if(addNewParameter(specialSignStr) == Error::Error)
                {
                    return Error::Error;
                }
                characterIncrementRequired = false;
            }
        }
            break;
        case 'u': // u0123
        {   // 0
            specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
            tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
            if(tclInterpreter.textInterpreter().isCurrentChar() and // 0
                    QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-9a-fA-F]")) != -1)
            {
                specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                if(tclInterpreter.textInterpreter().isCurrentChar() and // 1
                        QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-9a-fA-F]")) != -1)
                {
                    specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                    tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                    if(tclInterpreter.textInterpreter().isCurrentChar() and // 2
                            QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-9a-fA-F]")) != -1)
                    {
                        specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                        tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                        if(tclInterpreter.textInterpreter().isCurrentChar() and // 3
                                QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-9a-fA-F]")) != -1)
                        {
                            specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                            if(addNewParameter() == Error::Error
                                    or addNewParameter(Stat::BackslashSubbing,
                                                   QString("\\") + specialSignStr,
                                                       QString("\\") + specialSignStr) == Error::Error)
                            {
                                return Error::Error;
                            }
                        }else{
                            if(addNewParameter() == Error::Error
                                    or addNewParameter(Stat::BackslashSubbing,
                                                   QString("\\") + specialSignStr,
                                                       QString("\\") + specialSignStr) == Error::Error)
                            {
                                return Error::Error;
                            }
                            characterIncrementRequired = false;
                        }
                    }else{
                        if(addNewParameter() == Error::Error
                                or addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                                   QString("\\") + specialSignStr) == Error::Error)
                        {
                            return Error::Error;
                        }
                        characterIncrementRequired = false;
                    }
                }else{
                    if(addNewParameter() == Error::Error
                            or addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr,
                                               QString("\\") + specialSignStr) == Error::Error)
                    {
                        return Error::Error;
                    }
                    characterIncrementRequired = false;
                }
            }else{
                if(addNewParameter(specialSignStr) == Error::Error)
                {
                    return Error::Error;
                }
                characterIncrementRequired = false;
            }
        }
            break;
        case 'a':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 'v':
        case 't':
        case '\\':
           if(addNewParameter() == Error::Error
                   or addNewParameter(Stat::BackslashSubbing,
                                   QString("\\") + tclInterpreter.textInterpreter().currentCharForSpecialSign(),
                                      QString("\\") + tclInterpreter.textInterpreter().currentCharForSpecialSign()) == Error::Error)
           {
               return Error::Error;
           }
            break;
        default:
            // Check if octal form - \ooo - check tcl syntax documantation for details
            if(tclInterpreter.textInterpreter().isCurrentChar() and // 0
                    QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-3]")) != -1)
            { // Octal - change current character / max. range /[0-3][0-7][0-7]
                specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                if(tclInterpreter.textInterpreter().isCurrentChar() and  // 1
                        QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-7]")) != -1)
                {
                    specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                    tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                    if(tclInterpreter.textInterpreter().isCurrentChar() and  // 2
                            QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-7]")) != -1)
                    {
                        specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                        if(addNewParameter() == Error::Error
                                or addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                                   QString("\\") + specialSignStr) == Error::Error)
                        {
                            return Error::Error;
                        }
                    }else{
                        if(addNewParameter() == Error::Error
                                or addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                                   QString("\\") + specialSignStr) == Error::Error)
                        {
                            return Error::Error;
                        }
                        characterIncrementRequired = false;
                    }
                }else{
                    if(addNewParameter() == Error::Error
                            or addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr,
                                               QString("\\") + specialSignStr) == Error::Error)
                    {
                        return Error::Error;
                    }
                    characterIncrementRequired = false;
                }
            }else{  // Save as BackslashSubbing - RawParameter '\\<currentSign>' -  Parameter '<currentSign>'
                if(tclInterpreter.textInterpreter().isCurrentChar() and // 0
                        QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[4-7]")) != -1)
                { // / max. range /[4-7][0-7]
                    specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                    tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                    if(tclInterpreter.textInterpreter().isCurrentChar() and  // 1
                            QString(tclInterpreter.textInterpreter().currentCharForSpecialSign()).indexOf(QRegularExpression("[0-7]")) != -1)
                    {
                        specialSignStr += tclInterpreter.textInterpreter().currentCharForSpecialSign();
                        if(addNewParameter() == Error::Error
                                or addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                                   QString("\\") + specialSignStr) == Error::Error)
                        {
                            return Error::Error;
                        }
                    }else{
                        if(addNewParameter() == Error::Error
                                or addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                                   QString("\\") + specialSignStr) == Error::Error)
                        {
                            return Error::Error;
                        }
                        characterIncrementRequired = false;
                    }
                }else{
                    if(addNewParameter(tclInterpreter.textInterpreter().currentCharForSpecialSign()) == Error::Error)
                    {
                        return Error::Error;
                    }
                }
            }
         }
        if(characterIncrementRequired == true)
            tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
    }else{
        tclInterpreter.setBackslashSubbingInterpreterMode();
        updateCurrentCallProcedures(Stat::BackslashSubbing);
        tclInterpreter.setProcessingStat(Stat::EndOfString);
    }
    return Error::NoError;
}

Stat Controller::processingStat()const{return tclInterpreter.processingStat();}

Error Controller::addNewParameter(){    // To create empty parameter
    if(not lastProcedureCall().isLastParameterEmpty()){
        return (newParameter() == Error::NoError
                and lastProcedureCall().newParameter() == Error::NoError)?
                    Error::NoError : Error::Error;
    }
    return Error::NoError;
}

Error Controller::addNewParameter(QString wordParameterStr)    // To create Word parameter or append Word parameter
{
    if(lastProcedureCall().rawParameters().last().stat() == Stat::Word){
       lastProcedureCall().lastParameter().appendCommand(wordParameterStr);
    }else{
        return (newParameter() == Error::NoError
                and lastProcedureCall().newParameter(wordParameterStr) == Error::NoError)?
                    Error::NoError : Error::Error;
    }
    return Error::NoError;
}

Error Controller::addNewParameter(Stat stat)    // To create VariableSubbing
{
    if(lastProcedureCall().isLastParameterEmpty()){
        return (/*newParameter() == Error::NoError
                  and*/ lastProcedureCall().replaceLastParameter(stat, QString(), QString()) == Error::NoError)?
                    Error::NoError : Error::Error;
    }else{
        return (newParameter() == Error::NoError
                and lastProcedureCall().newParameter(stat, QString()) == Error::NoError)?
                    Error::NoError : Error::Error;
    }
}

Error Controller::addNewParameter(Stat stat, QString rawCommand, OutputCommand outputCommand)    // To create BackslashSubbing
{
    if(lastProcedureCall().isLastParameterEmpty()){
        return (lastProcedureCall().replaceLastParameter(stat, rawCommand, outputCommand) == Error::NoError
                /*and newParameter() == Error::NoError*/)?
                    Error::NoError : Error::Error;
    }else{
        return (lastProcedureCall().newParameter(stat, rawCommand, outputCommand) == Error::NoError
                /*and newParameter() == Error::NoError*/)?
                    Error::NoError : Error::Error;
    }
}

inline Error Controller::addNewParameter(Call& call){
    if(lastProcedureCall().isLastParameterEmpty()){
        return (lastProcedureCall().replaceLastParameter(call.stat(), call.rawCommand(), call.outputCommand()) == Error::NoError
                /*and newParameter() == Error::NoError*/)?
                    Error::NoError : Error::Error;
    }else{
        return (/*newParameter() == Error::NoError
                and */lastProcedureCall().newParameter(call.stat(), call.rawCommand(), call.outputCommand()) == Error::NoError
                /*and */)?
                    Error::NoError : Error::Error;
    }
}


Error Controller::addFinalizedCallParameter(){
    if(procedureCalls.size() > 1){
        Call&& lastCall = procedureCalls.takeLast();

        updateCurrentCallProcedures();

        if(lastProcedureCall().stat() == Stat::CommandSubbing)
        {
            if(lastProcedureCall().definition() == nullptr){
                // Procedure name processing
                callConfig.setCommandSubbingNewParameter_procedureNameMode();
            }else{
                // Parameters processing
                callConfig.setCommandSubbingNewParameter_parametersMode();
            }
        }
        if(addNewParameter(lastCall) == Error::Error){
            return Error::Error;
        }
    }else{
        Call&& lastCall = procedureCalls.takeLast();
        updateCurrentCallProcedures();
        tclInterpreter.addPreexpressionsToMainCodeBlock();
        tclInterpreter.addExpressionToMainCodeBlock({lastCall.outputCommand()}, "");
    }
    return Error::NoError;
}

Error Controller::finalizeCall(){
    return (destructor() == Error::NoError
            and addFinalizedCallParameter() == Error::NoError)?
                Error::NoError : Error::Error;
}

Error Controller::performRulesCheckForNewParameter(){
    const QString ERROR_PREFIX = QString("InternalCall: performRulesCheckForNewParameter() : ");
    if(procedureCalls.isEmpty() or
                procedureCalls.last().isRulesEmpty() or
                procedureCalls.last().isUserInteractionRequired())
            return Error::NoError;
    return (onArgumentProcedureCheck() == Error::Error)? throwError(ERROR_PREFIX + error()) : Error::NoError;
}

Error TclProcedureInterpreter::onArgumentProcedureCheck(){
    using RulesForArguments = Definition::RulesForArguments;
    using RulesForArgument = Definition::RulesForArguments::Iterator;
    using Rules = Definition::Rules;
    using Rule = Definition::Rules::Iterator;
    Call& procedureCall = procedureCalls.last();
    RulesForArguments::Iterator rulesForArgument = procedureCall.lastRulesForArgument_onMoved();
    if(procedureCall.isRulesInRange(rulesForArgument) == Error::Error){
        rulesForArgument = procedureCall.rulesForUnspecifiedArgument();
    }
    bool ruleCondtionsPassed = false;
    using Actions = Rules::value_type::ExecutableActions;
    using Action = Actions::Iterator;
    for(Rule rule = rulesForArgument->rulesOnMoveArgument.begin(); rule < rulesForArgument->rulesOnMoveArgument.end(); rule++){
        using Conditions = Rules::value_type::ConditionalActions;
        using Condition = Conditions::Iterator;
        Condition condition;
        ConditionResult conditionResult = ConditionResult::Satisfied;
        for(condition = rule->conditions.begin(); condition < rule->conditions.end(); condition++){
            conditionResult = (this->*(conditionalInterpreterFunctions[static_cast<std::underlying_type_t<Conditions::value_type::ActionType>>(condition->type())]))(condition->parameters());
            if(tclInterpreter.isError()){
                return Error::Error;
            }
            if(conditionResult == ConditionResult::Unsatisfied){
                break;
            }
        }
        if(condition == rule->conditions.end()){
            ruleCondtionsPassed = true;
            for(Action action = rule->actions.begin(); action < rule->actions.end(); action++){
                (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Actions::value_type::ActionType>>(action->type())]))(action->parameters());
                if(tclInterpreter.isError()){
                    return Error::Error;
                }
            }
            switch(rule->controlFlag){
            //case Rules::value_type::Control::BreakRuleCheckDontExecOnEndActions:
              //  return Error::Error;
            case Rules::value_type::Control::BreakRuleCheck:
                rule = rulesForArgument->rulesOnMoveArgument.end();
                break;
            case Rules::value_type::Control::NoBreakRuleCheck:
                ruleCondtionsPassed = false;
                break;
            default:
                break;
            }
        }
    }
    return Error::NoError;
}

bool TclProcedureInterpreter::isPredefinitionMode(){
    return userConfig.proceduresSettings().mode() == Settings::InterpreterMode::PredefinitionsOnly;
}

Error TclProcedureInterpreter::performDynamicRulesCheck(){
    if(procedureCalls.isEmpty() or
            lastProcedureCall().definition() == nullptr or
            procedureCalls.last().isRulesEmpty() or
            procedureCalls.last().isUserInteractionRequired())
        return Error::NoError;
    return dynamicRulesCheck();
}

Error TclProcedureInterpreter::dynamicRulesCheck(){
    using RulesForArguments = Definition::RulesForArguments;
    using RulesForArgument = Definition::RulesForArguments::Iterator;
    using Rules = Definition::Rules;
    using Rule = Definition::Rules::Iterator;
    Call& procedureCall = procedureCalls.last();
    RulesForArgument rulesForArgument = procedureCall.lastRulesForArgument_dynamicCheck();
    if(procedureCall.isRulesInRange(rulesForArgument) == Error::Error /*or
            rulesForArgument->status == RulesForArguments::Type::Status::Unspecified*/){
        rulesForArgument = procedureCall.rulesForUnspecifiedArgument();
    }
    bool ruleCondtionsPassed = false;
    using Actions = Rules::value_type::ExecutableActions;
    using Action = Actions::Iterator;
    for(Rule rule = rulesForArgument->rules.begin(); rule < rulesForArgument->rules.end(); rule++){
        using Conditions = Rules::value_type::ConditionalActions;
        using Condition = Conditions::Iterator;
        Condition condition;
        ConditionResult conditionResult = ConditionResult::Satisfied;
        for(condition = rule->conditions.begin(); condition < rule->conditions.end(); condition++){
            conditionResult = (this->*(conditionalInterpreterFunctions[static_cast<std::underlying_type_t<Conditions::value_type::ActionType>>(condition->type())]))(condition->parameters());
            if(tclInterpreter.isError()){
                return Error::Error;
            }
            if(conditionResult == ConditionResult::Unsatisfied){
                break;
            }
        }
        if(condition == rule->conditions.end()){
            ruleCondtionsPassed = true;
            for(Action action = rule->actions.begin(); action < rule->actions.end(); action++){
                (this->*(executableInterpretFunctions[static_cast<std::underlying_type_t<Actions::value_type::ActionType>>(action->type())]))(action->parameters());
                if(tclInterpreter.isError()){
                    return Error::Error;
                }
            }
            switch(rule->controlFlag){
            //case Rules::value_type::Control::BreakRuleCheckDontExecOnEndActions:
                //return Error::NoError;
            case Rules::value_type::Control::BreakRuleCheck:
                rule = rulesForArgument->rules.end();
                break;
            case Rules::value_type::Control::NoBreakRuleCheck:
                ruleCondtionsPassed = false;
                break;
            default:
                break;
            }
        }
    }
    return Error::NoError;
}

Error TclProcedureInterpreter::processCallsForError(){
    while(not procedureCalls.empty()){
        switch(lastProcedureCall().stat()){
        case Stat::Script:
            updateCurrentCallProcedures(Stat::Ignore);
            return Error::NoError;
        case Stat::CommandSubbing:
        case Stat::DoubleQuotes:
        case Stat::ComplexWord:
// Commented but required
//            if(removeProcedureCall() == Error::Error)
                return throwError("TCLInterpreter::processSavedStatsForError: " + error());
        default:
// Commented but required
//            if( == Error::Error)
                return throwError("TCLInterpreter::processSavedStatsForError: " + error() );
        }
    }
    return throwError("TCLInterpreter::processSavedStatsForError: No stats");
}

Error Controller::prepareSnprintf(){
    const QString ERROR_PREFIX = "prepareSnprintf procedure: ";

    // Case of \<newLine> processing
    if(not lastProcedureCall().parameters().empty()){
        switch(lastProcedureCall().parameters().first().stat()){
        // Error Cases
        case Stat::BracesStart:
        case Stat::DoubleQuotes:
        case Stat::Script:
        {
            lastProcedureCall().changeStat(lastProcedureCall().parameters().first().stat());
            lastProcedureCall().setOutputCommand(lastProcedureCall().parameters().first().outputCommand());
            lastProcedureCall().setRawCommand(lastProcedureCall().parameters().first().rawCommand() +
                                              lastProcedureCall().parameters().at(1).rawCommand());
        }
            return Error::NoError;
        default:
            break;
        }
    }

    const QString snprintfVarName = tclInterpreter.snprintfController.getNextSnprintfVarName();
    // Prepare preexpression
    bool snprintfRequired = false;
    QString format;
    QString arguments;
    QString preexpresion = QStringLiteral("snprintf(") + snprintfVarName + QStringLiteral(", elcount(") + snprintfVarName + QStringLiteral("), \"");
    Call::Parameters &parameters = lastProcedureCall().parameters();
    for(Call::Parameters::Iterator parameter = parameters.begin();
        parameter < parameters.end(); parameter++)
    {
        switch(parameter->stat()){
        case Stat::Word:
        case Stat::BackslashSubbing:
            format += parameter->rawCommand();
            break;
        case Stat::VariableSubbing:
        case Stat::CommandSubbing:
            format += "%s";
            arguments += ", " + parameter->outputCommand();
            snprintfRequired = true;
            break;
        default:
            return throwError(ERROR_PREFIX + "Unknown Parameter Stat");
        }
    }

    if(snprintfRequired){
        //Add Preexpression
        ExecutableActionsParameters preexpressionParameters = {
            QStringLiteral("snprintf(") + snprintfVarName + ", elcount(" + snprintfVarName + "), " +
            "\"" + format + "\"" + arguments + ")"
        };

        (this->*executableInterpretFunctions[
                static_cast<std::underlying_type_t<Definition::Action::Executable>>(
                    Definition::Action::Executable::AddPreExpression)])(preexpressionParameters);
        QString CHAR_TYPE = "char";
        tclInterpreter.predefinitionsController.newVariable(CHAR_TYPE, snprintfVarName, {128});

        if(tclInterpreter.isError()){
            return throwError(ERROR_PREFIX + tclInterpreter.error());
        }

        lastProcedureCall().setOutputCommand(snprintfVarName);
    }else{
        lastProcedureCall().setOutputCommand(format);
        if(lastProcedureCall().stat() == Stat::ComplexWord)
            lastProcedureCall().changeStat(Stat::Word);
    }
    return Error::NoError;
}

