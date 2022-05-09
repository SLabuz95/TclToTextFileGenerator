#include"TclInterpreter/CommandsCont/commandsCont.hpp"
#include"TclInterpreter/CommandsCont/commandCallConfig.hpp"
#include"TclInterpreter/tclToCAPL.hpp"
#include"Tcl2Capl/controller.hpp"
#include"TclInterpreter/ListsCont/listsCont.hpp"

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
            unknownString = tclInterpreter.readCurrentKeyword();            
            if(lastProcedureCall().rawParameters().last().stat() == Stat::Word){
                if(addNewParameter(unknownString) == Error::Error)
                    return throwError(ERROR_PREFIX + error());
            }else{  // Any other stat is parameter without whitespace after parameter Ex. "abc"abc Expection [puts abc]abc - ComplexWord
                return throwError(ERROR_PREFIX + "Impossible Case");
                //return lastProcedureCall().newParameter(Stat::Word, unknownString);
            }
        }
            break;
        case Stat::Whitespace:
        {
            // Create new empty Word parameter
            return addNewParameter();
        }
            break;
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
            // Commented but required : Maybe just new parameter Word - Avoid whole procedure
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
            case Stat::Braces:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));

            case Stat::ComplexWord:
                return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
            default:
            {
                if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                    return throwError(ERROR_PREFIX + error());
                }
                break;
            }
            }
        }
        return createCall(Stat::CommandSubbing);
    }
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
            case Stat::Braces:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            case Stat::ComplexWord:
                return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
            case Stat::Word:
            {
                return newParameterProcessing();
            }
            default:
            {
                if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                    return throwError(ERROR_PREFIX + error());
                }
                // Commented but required : Maybe just new parameter Word - Avoid whole procedure
                return interpret();
            }
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
                case Stat::Braces:
                case Stat::DoubleQuotes:
                case Stat::Script:
                    return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
                case Stat::ComplexWord:
                    return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
                default:
                    break;
            }            
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                return throwError(ERROR_PREFIX + error());
            }
        }
        // Commented but required : Maybe just add parameter VariableSubbing + StartVariableSubbing - Avoid whole procedure
        return interpret();
   }
    case Stat::BackslashSubbing:
    {
        if(not lastProcedureCall().isLastParameterEmpty()){
            switch(lastProcedureCall().rawParameters().last().stat()){            
            // Error Cases
            case Stat::Braces:
            case Stat::DoubleQuotes:
            case Stat::Script:
                return throwError(ERROR_PREFIX + "No whitespace after DoubleQuotes or Braces. Stat for this error: " + QString::number(qToUnderlying(lastProcedureCall().rawParameters().last().stat())));
            case Stat::ComplexWord:
                return throwError(ERROR_PREFIX + "Impossible case. Unfinished ComplexWord.");
            default:
                break;
            }
            if(createCallAndMoveLastParameterToOne(Stat::ComplexWord) == Error::Error){
                return throwError(ERROR_PREFIX + error());
            }
        }
        // Commented but required : Maybe just add parameter BackslashSubbing + BackslashSubbing procedure - Avoid whole procedure
        return interpret();
    }
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
    const QString ERROR_PREFIX = "Constructor Special Command <Stat::Common>: ";

    CallConfig::setCommandSubbingNewParameter_procedureNameMode();
    return Error::NoError;
}


template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbingStart>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::CommandSubbingStart>: ";

    return Error::NoError;
}

template<>
Error TclProcedureInterpreter::newParameter_mode<Stat::CommandSubbing>(){
    const QString ERROR_PREFIX = "New Parameter Special Command <Stat::CommandSubbing>: ";

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
        switch(lastProcedureCall().rawParameters().last().stat()){
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
        tclInterpreter.listController().deactivateListLevelControl();
        if(tclInterpreter.listController().isListClosed()){ // - Control deactived or list closed - finalize
            if(finalizeCall() == Error::Error)
                return throwError(ERROR_PREFIX + error());
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
        switch(lastProcedureCall().rawParameters().last().stat()){
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
        // Interpret then add as parameter
        if(processBackslashSubbing() == Error::Error)
            return throwError(ERROR_PREFIX + error());
    }
        break;
    case Stat::VariableSubbing:
    {
        if(addNewParameter(processingStat()) == Error::Error)
            return throwError(ERROR_PREFIX + error());
        startVariableSubbing();
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
        if(createCall(Stat::CommandSubbing) == Error::Error)
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
    case Stat::CommandSubbingEnd:
    case Stat::BracesStart:
        return newParameterProcessing();
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
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
        if(addNewParameter(processingStat()) == Error::Error)
            return throwError(ERROR_PREFIX + error());
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
        if(createCall(Stat::CommandSubbing) == Error::Error)
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
    case Stat::EndOfString:
    case Stat::Semicolon:
    case Stat::Whitespace:
    {
        // Finalize and add new empty parameter
        if(finalizeCall() == Error::Error){
           return throwError(ERROR_PREFIX + error());
        }
        // Recall interpreter procedure TclInterpreter for the same state
        return interpret();
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
        //unknownString = tclInterpreter.readCurrentKeyword();
        //return createCall(unknownString);
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
Error TclProcedureInterpreter::interpret_mode<Stat::VariableSubbing>(){
    const QString ERROR_PREFIX = "Interpret Special Command <Stat::VariableSubbing>: ";

    switch(processingStat()){
    case Stat::BracesStart:
    {
        if(createCall(Stat::BracesStart) == Error::Error){
            updateCurrentCallProcedures(); // Error Recovery
            return throwError(ERROR_PREFIX + error());
        }
        deactivateBackslashSubbing();   // Backslash subbing doesnt work for VariableSubbing Braces pattern
    }
        break;
    default:
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
        switch(tclInterpreter.textInterpreter().currentCharForSpecialSign()->toLatin1()){
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

    if(addNewParameter(Stat::BackslashSubbing, specialSignStr) == Error::Error){
        updateCurrentCallProcedures();
        tclInterpreter.setStandardInterpreterMode(); // Recovery
        return throwError(ERROR_PREFIX + error());
    }
    updateCurrentCallProcedures();

    tclInterpreter.setStandardInterpreterMode();
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
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },
    //DoubleQuotes
    {
        &TclProcedureInterpreter::interpret_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::newCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::DoubleQuotes>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },
    //ComplexWord
    {
        &TclProcedureInterpreter::interpret_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::newCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::ComplexWord>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },
    //Expression
    {
        &TclProcedureInterpreter::interpret_mode<Stat::Expression>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Expression>,
        &TclProcedureInterpreter::newCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::Expression>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },
    //Script
    {
        &TclProcedureInterpreter::interpret_mode<Stat::Script>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Script>,
        &TclProcedureInterpreter::newCall_mode<Stat::Script>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::Script>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },
    //ExprCommand
    {
        &TclProcedureInterpreter::interpret_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::newCall_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::ExprCommand>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
    },
    //Ignore
    {
        &TclProcedureInterpreter::interpret_mode<Stat::Ignore>,
        &TclProcedureInterpreter::newParameterProcessing_mode<Stat::Ignore>,
        &TclProcedureInterpreter::newCall_mode<Stat::Ignore>,
        &TclProcedureInterpreter::finalizeCall_mode<Stat::Ignore>,
        &TclProcedureInterpreter::constructor_mode<Stat::Common>,
        &TclProcedureInterpreter::newParameter_mode<Stat::Common>,
        &TclProcedureInterpreter::destructor_mode<Stat::Common>,
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
    // Change current methods for Call processing
    updateCurrentCallProcedures();
    if(constructor() == Error::Error){
        // Failed to initialize Call - Remove last call +  update call procedures
        // Other recovery procedures shall be perform in constructor procedure
        procedureCalls.removeLast();
        updateCurrentCallProcedures();
        return Error::Error;
    }

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
            return performVariableSubbingRecoveryProcedure();
        }else{
        // Otherwise New parameter required
            //return addNewParameter();
            return interpret();
        }
    }

    return Error::NoError;
}

Error Controller::startVariableSubbing(){
    updateCurrentCallProcedures(Stat::VariableSubbing);
}

Error Controller::processBackslashSubbing(){
    QString specialSignStr;
    // Check and prepare specialSignStr
    if(backslashSubbingActive == false){
        return addNewParameter(QString("\\"));
    }

    if(tclInterpreter.textInterpreter().isCurrentChar()) // If not accesible -> End of string that means EndOfLine-BackslashSubbing is required
    {
        if(not procedureCalls.isEmpty() and procedureCalls.last().stat() == Stat::Braces){
            return addNewParameter(QString("\\"));
        }
        switch (tclInterpreter.textInterpreter().currentCharForSpecialSign()->toLatin1()) {
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
                    tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                    return addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr);
                }else{
                    return addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr,
                                           specialSignStr);
                }
            }else{
                return addNewParameter(Stat::BackslashSubbing,
                                       QString("\\") + specialSignStr,
                                       specialSignStr);
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
                            tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                            return addNewParameter(Stat::BackslashSubbing,
                                                   QString("\\") + specialSignStr);
                        }else{
                            return addNewParameter(Stat::BackslashSubbing,
                                                   QString("\\") + specialSignStr,
                                                   specialSignStr);
                        }
                    }else{
                        return addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                               specialSignStr);
                    }
                }else{
                    return addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr,
                                           specialSignStr);
                }
            }else{
                return addNewParameter(Stat::BackslashSubbing,
                                       QString("\\") + specialSignStr,
                                       specialSignStr);
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
            return addNewParameter(Stat::BackslashSubbing,
                                   QString("\\") + *tclInterpreter.textInterpreter().currentCharForSpecialSign());
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
                        tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                        return addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr);
                    }else{
                        return addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                               specialSignStr);
                    }
                }else{
                    return addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + specialSignStr,
                                           specialSignStr);
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
                        tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
                        return addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr);
                    }else{
                        return addNewParameter(Stat::BackslashSubbing,
                                               QString("\\") + specialSignStr,
                                               specialSignStr);
                    }
                }else{
                    return addNewParameter(Stat::BackslashSubbing,
                                           QString("\\") + *tclInterpreter.textInterpreter().currentCharForSpecialSign(),
                                           *tclInterpreter.textInterpreter().currentCharForSpecialSign());
                }
            }

            break;
         }
        tclInterpreter.textInterpreter().incrementCurrentCharDueToSpecialSign();
    }else{
        tclInterpreter.setBackslashSubbingInterpreterMode();
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

Error Controller::addNewParameter(Stat stat, QString rawParameter, OutputCommand outputCommand){
    if(lastProcedureCall().isLastParameterEmpty()){
        return (lastProcedureCall().replaceLastParameter(stat, rawParameter, outputCommand) == Error::NoError
                and newParameter() == Error::NoError)?
                    Error::NoError : Error::Error;
    }else{
        return (lastProcedureCall().newParameter(stat, rawParameter, outputCommand) == Error::NoError
                and newParameter() == Error::NoError)?
                    Error::NoError : Error::Error;
    }
}


Error Controller::addFinalizedCallParameter(){
    if(procedureCalls.size() > 1){
        Call&& lastCall = procedureCalls.takeLast();

        updateCurrentCallProcedures();

        if(lastProcedureCall().stat() == Stat::CommandSubbing)
        {
            if(lastProcedureCall().parametersLength() == 1){
                // Procedure name processing
                CallConfig::setCommandSubbingNewParameter_procedureNameMode();
            }else{
                // Parameters processing
                CallConfig::setCommandSubbingNewParameter_parametersMode();
            }
        }

        if(lastProcedureCall().newParameter(lastCall) == Error::Error){
            return Error::Error;
        }
    }else{
        // Add to generated script - Tcl Interpreter
    }
    return Error::NoError;
}

Error Controller::finalizeCall(){
    return (destructor() == Error::NoError
            and addFinalizedCallParameter() == Error::NoError)?
                Error::NoError : Error::Error;
}
