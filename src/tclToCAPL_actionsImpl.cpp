#include"tclToCAPL.hpp"
#include"tclToCAPL_errorFileConfig.hpp"
#include"Tcl2Capl/controller.hpp"
#include"Tcl2Capl/caplFunctionDefiniitions.hpp"

using Interpreter = TCLInterpreter;
using TextInterpreter = TCLInterpreter::TextInterpreter;
using TclProcedureInterpreter = TCLInterpreter::TCLProceduresInterpreter;
using Error = Interpreter::Error;
using Result = TextInterpreter::Result;
using ReadIgnoreResult = TextInterpreter::ReadIgnoreResult;
using CheckingResult = TextInterpreter::CheckingResult;
using KeywordsMap = TextInterpreter::KeywordsMap;
using TCLInterpreterFunctions = Interpreter::InterpretFunctions;
using UserInteractionStatus = TclProcedureInterpreter::UserInteractionStatus;
using Rule = TclProcedureInterpreter::ProcedureDefinition::Rule;
using Action = TclProcedureInterpreter::ProcedureDefinition::Action;
using RuleControlFlag = Rule::Control;
using Format = TclProcedureInterpreter::ProcedureDefinition::Format;
using FormatRule = TclProcedureInterpreter::ProcedureDefinition::Format::Rule;
using FormatTarget = TclProcedureInterpreter::ProcedureDefinition::Format::Target;

// PROCEDURE DEFINITION ACTIONS ==================================================================================
template <>
TclProcedureInterpreter::ConditionResult
TclProcedureInterpreter::executeConditionalAction
<TclProcedureInterpreter::ProcedureDefinition::Action::Conditional::CompareNumbOfArguments>
(TclProcedureInterpreter::ConditionalActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeConditionalAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::CompareNumbOfArguments>: ";
    if(parameters.isEmpty()){
        throwError(ERROR_PREFIX + "No action parameters");
        return ConditionResult::Satisfied;
    }
    for(QStringList::ConstIterator parameter = parameters.begin(); parameter < parameters.end(); parameter++){
        bool ok;
        ProcedureCall::Parameters::size_type size = 0;
        if((size = (parameter->toInt(&ok))), !ok){
            throwError(ERROR_PREFIX + "Action Argument: " + *parameter + " cant be parsed as integer.");
            return ConditionResult::Satisfied;
        }
        if(lastProcedureCall().parametersLength() == size)
            return ConditionResult::Satisfied;
    }
    return ConditionResult::Unsatisfied;
}

template <>
TCLInterpreter::TCLProceduresInterpreter::ConditionResult
TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::IsLastSavedStat>
(ConditionalActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeConditionalAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::IsSavedStat>: ";

    if(parameters.isEmpty()){
        throwError(ERROR_PREFIX + "No action parameters");
        return ConditionResult::Satisfied;
    }
    for(QStringList::ConstIterator parameter = parameters.begin(); parameter < parameters.end(); parameter++){
        bool ok;
        Stat stat = Stat::Size;
        if((stat = static_cast<Stat>(parameter->toInt(&ok))), !ok){
            throwError(ERROR_PREFIX + "Action Argument: " + *parameter + " cant be parsed as integer.");
            return ConditionResult::Satisfied;
        }
        if(stat >= Stat::Size){
            throwError(ERROR_PREFIX + "Stat not in range. Current index: " + QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Size)) + " Current size: " + QString::number(tclInterpreter.savedStatsSize()));
            return ConditionResult::Satisfied;
        }
        if(tclInterpreter.lastSavedStat().stat() == stat)
            return ConditionResult::Satisfied;
    }
    return ConditionResult::Unsatisfied;
}

template <>
TCLInterpreter::TCLProceduresInterpreter::ConditionResult
TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::Compare>
(ConditionalActionsParameters parameters){
    const QString ERROR_PREFIX = "executeConditionalAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Compare>: ";


    QString formatStr;
    bool ok = false;
    uint numbOfCompares = UINT_MAX;
    uint numbOfFormatArgs = UINT_MAX;
    if(parameters.size() < 3){
        throwError(ERROR_PREFIX + "Number of Action Parameters dont match.");
        return ConditionResult::Satisfied;
    }
    if((numbOfCompares = parameters.at(0).toUInt(&ok), !ok) ||
            numbOfCompares + 1 >= parameters.size()  ||
            (numbOfFormatArgs = parameters.at(numbOfCompares + 1).toUInt(&ok), !ok) ||
            numbOfCompares + numbOfFormatArgs + 2 != parameters.size())
    {
        throwError(ERROR_PREFIX + "Action Parameters are wrong.");
        return ConditionResult::Satisfied;
    }
    if(!createAndAssignString(formatStr, parameters.mid(2 + numbOfCompares)))
    {
        throwError(ERROR_PREFIX + "");
        return ConditionResult::Satisfied;
    }
    for(QStringList::ConstIterator str = parameters.begin() + 1; str < parameters.begin() + 1 + numbOfCompares; str++)
        if(*str == formatStr)
            return ConditionResult::Satisfied;
    return ConditionResult::Unsatisfied;
}


template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Write>
(ExecutableActionsParameters arguments)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Write>: ";
    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, arguments)) != arguments.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }
    if(finalizeOn){ // Used for procedure
        command = str;
    }else{  // For last argument
        lastProcedureCall().lastParameter().setCommand( str);
    }
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction<
TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::TclParse> (ExecutableActionsParameters arguments){
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::TclParse>: ";
    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, arguments)) != arguments.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }

    TCLInterpreter newTclInterpreter(tclInterpreter.userConfig, tclInterpreter.functionDefinitions);
    if(newTclInterpreter.toCAPL(str) == Error::Error or newTclInterpreter.anyErrors())
    {
        throwError(ERROR_PREFIX + "TclParsing Failed:" + " {Current Result: " + newTclInterpreter.error() +"}");
        return;
    }
    if(finalizeOn){ // Used for procedure
        command = newTclInterpreter.readCommand().remove("\n");
    }else{  // For last argument
        lastProcedureCall().lastParameter().setCommand(newTclInterpreter.readCommand().remove("\n"));
    }
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Error>
(ExecutableActionsParameters arguments)
{
    const QString ERROR_PREFIX = "executeAction<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Error>: ";

    if(arguments.isEmpty())
        throwError(ERROR_PREFIX + "No arguments for Action Rule");
    else
        throwError(QString("Error Action: ") + arguments.first());

}


template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::ChangeLastSavedStat>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::ChangeLastSavedStat>: ";

    // IMPORTANT - You cant change FunctionCall, Snprintf or PendingSnprintf stat
    switch(tclInterpreter.lastSavedStat().stat()){
        case Stat::CommandSubbing:
        //case Stat::Snprintf:
        //case Stat::PendingSnprintf:
        {
            throwError(ERROR_PREFIX + "Its impossible to change CommandCall, Snprintf or PendingSnprintf");
            return ;
        }
        default:
            break;
    }

    if(parameters.size() != 1){
        throwError(ERROR_PREFIX + "Number of action arguments dont match. Expected: 1.");
        return ;
    }
    bool ok;
    Stat stat = Stat::Size;
    if((stat = static_cast<Stat>(parameters.first().toInt(&ok))), !ok){
        throwError(ERROR_PREFIX + "Action Argument: " + parameters.first() + " cant be parsed as integer.");
        return ;
    }
    if(stat >= Stat::Size or stat == Stat::None){
        throwError(ERROR_PREFIX + "Stat not in range. Current index: " + QString::number(static_cast<std::underlying_type_t<Stat>>(Stat::Size)) + " Current size: " + QString::number(tclInterpreter.savedStatsSize()));
        return ;
    }
    //if(lastProcedureCall().name() == "foreach")
      //  qDebug() << "Test";
    tclInterpreter.lastSavedStat().setStat(stat);
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddFunctionDefinition>
(ExecutableActionsParameters parameters)
{
    tclInterpreter.functionDefinitions.addDefinitionNoRules(lastProcedureCall());
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPreExpression>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPreExpression>: ";

    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, parameters)) != parameters.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }

    if(not str.endsWith(";\n"))
        str.append(";");
    tclInterpreter.preexpressions().append(str);
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddUserInteraction>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddUserInteraction>: ";

    QStringList::size_type result = 0;
    QString str;
    if((result = createAndAssignString(str, parameters)) != parameters.size()){
        throwError(ERROR_PREFIX + "createAndAssignString Failed for Argument " + QString::number(result) + " {Current Result: " + str +"}");
        return;
    }
    if(!finalizeOn)
    {
        throwError(ERROR_PREFIX + "Internal Error");
        return;
    }

    if(addPreExpressionForUserInteraction() == Error::Error){
        throwError(ERROR_PREFIX + tclInterpreter.error());
        return;
    }

}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::FinalizeForEach>
(ExecutableActionsParameters parameters)
{
    /*
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::FinalizeForEach>: ";


    QString result;


    // First Argument processing
    QStringList argumentsForParam1;

    switch(procedureCalls.last().parameters().first().stat()){
    case Stat::String:
    case Stat::PendingString:
        argumentsForParam1 = {procedureCalls.last().parameters().first().command()};
        break;
    case Stat::Braces:
    {
        if(procedureCalls.last().parameters().first().savedStat().isAdvancedList())
        {
            throwError(ERROR_PREFIX + "Not implemented Advanced List control for Argument 1");
            return;
        }
        argumentsForParam1 = procedureCalls.last().parameters().first().savedStat().command().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    }
        break;
    default:
    {
        throwError(ERROR_PREFIX + "Not implemented control for Argument 1");
        return;
    }
    }

    // Second Argument processing
    QStringList argumentsForParam2;
    enum VariableType{
       String,
       Long
    }variableType = VariableType::String;
    QString loopIteratorEndValue;

    switch(procedureCalls.last().parameters().at(1).stat()){
    case Stat::Variable:
    {
        argumentsForParam2 += procedureCalls.last().parameters()[1].savedStat().command();
        loopIteratorEndValue = QString("elcount(") + argumentsForParam2.last() + ")";
    }
        break;
    case Stat::Snprintf:
    {
        throwError(ERROR_PREFIX + "List of Variables and function calls are forbidden for Argument 2 ");
        return;
    }
        break;

    case Stat::StringInQuotes:
    {
        variableType = VariableType::Long;
        argumentsForParam2 = procedureCalls.last().parameters()[1].savedStat().command().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for(QStringList::Iterator arg2 = argumentsForParam2.begin();
            arg2 < argumentsForParam2.end() and variableType == VariableType::Long; arg2++){
            if(arg2->endsWith(",")){
                arg2->chop(1);
                if(!tclInterpreter.isStringConstNumber(*arg2))
                {
                    arg2->append(",");
                    variableType = VariableType::String;
                }
            }else{
                if(!tclInterpreter.isStringConstNumber(*arg2))
                {
                    variableType = VariableType::String;
                }
            }
        }
    }
        break;
    case Stat::Braces:
    {
        if(procedureCalls.last().parameters()[1].savedStat().isAdvancedList())
        {
           ListStatInfo::ListInfos& listInfos = procedureCalls.last().parameters()[1].savedStat().advancedList()->getListInfos();
           QStringList tempArguments;
           using ListElement = ListStatInfo::ListInfos::Iterator;
           for(ListElement listElement = listInfos.begin(); listElement < listInfos.end(); listElement++)
           {
               QString tempStr = listElement->toString("");
               if(tempStr.startsWith("{") and tempStr.endsWith("}")){
                   argumentsForParam2.append(tempStr.mid(1, tempStr.size()-2));
               }else{
                   tempArguments = tempStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                   argumentsForParam2.append(tempArguments);
               }
           }
        }else{
            variableType = VariableType::Long;
            argumentsForParam2 = procedureCalls.last().parameters()[1].savedStat().command().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            for(QStringList::Iterator arg2 = argumentsForParam2.begin();
                arg2 < argumentsForParam2.end() and variableType == VariableType::Long; arg2++){
                if(arg2->endsWith(",")){
                    arg2->chop(1);
                    if(!tclInterpreter.isStringConstNumber(*arg2))
                    {
                        arg2->append(",");
                        variableType = VariableType::String;
                    }
                }else{
                    if(!tclInterpreter.isStringConstNumber(*arg2))
                    {
                        variableType = VariableType::String;
                    }
                }
            }
        }
    }
        break;
    default:
    {
        throwError(ERROR_PREFIX + "Not implemented control for Argument 2");
        return;
    }
    }
    // Write Results
    /* Result:
     {
    // _FOREACH_CONFIG_
      long _foreachIter = 0;
      Declare Arguments 1
      int _foreachList[<size of initializer>];
      // = {<List>};
      Declare Arguments 2 -> Comment Initializers
    for(_foreachIter = 0; _foreachIter < elcount(argument2); _foreachIter += numbOfArg1){
    // Loop Initialization

    // -------------
    Content

    // -------
    }

    if(loopIteratorEndValue.isEmpty()){
        if(argumentsForParam2.size() % argumentsForParam1.size() != 0){
            throwError(ERROR_PREFIX + "Wrong arguments implemntation - Arg1: " + argumentsForParam1.join(", ") + " Arg2 : " + argumentsForParam2.join(", ")) ;
            return;
        }else{
            loopIteratorEndValue = QString::number(argumentsForParam2.size() / argumentsForParam1.size());
        }
    }
    result = QString("{\n// _FOREACH_CONFIG_\nlong_foreachIter = 0;\n");
    // Arguments 1
    if(variableType == VariableType::String){
        for(QStringList::Iterator str = argumentsForParam1.begin(); str < argumentsForParam1.end(); str++){
            result += QString("char ") + *str + "[128];\n";
        }
    }else{
        for(QStringList::Iterator str = argumentsForParam1.begin(); str < argumentsForParam1.end(); str++){
            result += QString("long ") + *str + ";\n";
        }
    }
    // Arguments 2
    if(variableType == VariableType::String){
        result += QString("char _forEachList[") + QString::number(argumentsForParam2.size()) + "];\n// = {";
        for(QStringList::Iterator str = argumentsForParam1.begin(); str < argumentsForParam1.end(); str++){
            result += QString("\"") + *str + "\", ";
        }
        result.chop(QStringLiteral(", ").length());
        result += QString("};");
    }
    result +=  QString("for(_foreachIter = 0; _foreachIter < " + loopIteratorEndValue + "; _foreachIter++){");
    */

}


template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddSnprintf>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddSnprintf>: ";

    const QString snprintfVarName = tclInterpreter.snprintfController().getNextSnprintfVarName();
    ExecutableActionsParameters preexpressionParameters = {
        QStringLiteral("snprintf(") + snprintfVarName + ", elcount(" + snprintfVarName + "), \"",

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
            ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::SnprintfFormat),

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::ARGS_AFTER_INDEX) +
            "1",   // Ignore argument 0 == concat

        "\", ",
        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::TARGET) +
            ProcedureDefinition::Format::cast_target_str(ProcedureDefinition::Format::Target::CaplFormat),

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::SEPARATOR) +
            ", ",

        ProcedureDefinition::Format::FORMAT_RULE_CALL(),
        ProcedureDefinition::Format::cast_format_rule_str(ProcedureDefinition::Format::Rule::ARGS_AFTER_INDEX) +
            "1",   // Ignore argument 0 == concat
        ")"
    };


    (this->*executableInterpretFunctions[
            static_cast<std::underlying_type_t<ProcedureDefinition::Action::Executable>>(
                ProcedureDefinition::Action::Executable::AddPreExpression)])(preexpressionParameters);
    QString CHAR_TYPE = "char";
    tclInterpreter.predefinitionsControl().newVariable(CHAR_TYPE, snprintfVarName, {128});

    if(tclInterpreter.isError()){
        throwError(ERROR_PREFIX + tclInterpreter.error());
        return;
    }

    command = (snprintfVarName);
}

template <>
void TCLInterpreter::TCLProceduresInterpreter::executeAction
<TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPredefinition>
(ExecutableActionsParameters parameters)
{
    const QString ERROR_PREFIX = "executeAction<\
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPredefinition>: ";
    using ArrayRanks = PredefinitionsControl::ArrayRanks; // 1 Element == 0, no array
    using Variable = PredefinitionsControl::Variable;
    //WARNING: Nowy typ lub zmiana nazwy wymaga sprawdzenia algorytmow ponizej
    using ProcedureReturnType = Variable::Type;
    struct ProcedureReturnTypeInfo{
        ProcedureReturnType returnType = QString();
        ProcedureCall::Parameters::size_type parseFrom = 0;
    };

    ProcedureReturnType FLOAT_TYPE = "float";
    //ProcedureReturnType UINT_TYPE = "dword";
    ProcedureReturnType INT_TYPE = "long";
    ProcedureReturnType CHAR_TYPE = "char";

    using ReturnTypeForProcedure =  const QMap<const QString, ProcedureReturnTypeInfo>;
    ReturnTypeForProcedure returnTypeByProcedure = {
        {"expr", {}},
        {"expr_parser", {}}
    };

    std::decay_t<ProcedureReturnType> type;
    QString value;
    ArrayRanks arrayRanks = {0};
//TODO: Zabezpieczyc obsuge wszystkich stanow - powod: konfiguracja uzytkownika
    // If Procedure Name == "set"
    if(procedureCalls.last().name() == "set"){
        if(procedureCalls.last().parametersLength() == 2){
            // Init
            procedureCalls.last().parameters()[0].setCommand(procedureCalls.last().parameters().at(0).command().replace(":", ""));
            using Mode = UserInputConfig::Settings::InterpreterMode;
            using ParameterRefs = QList<ProcedureCall::Parameters::Iterator>;
            using ParameterRef = ProcedureCall::Parameters::Iterator;
            using RawParameterRef = ProcedureCall::Parameters::ConstIterator;
            // First parameterRefs is always first argumet of set procedure
            ParameterRefs parameterRefs{procedureCalls.last().parameters().begin() + 1};
            ParameterRef lastParameterRef = nullptr;
            while(not parameterRefs.isEmpty()){
                lastParameterRef = parameterRefs.last();
                if(type.isEmpty()){ // Initializing (second original parameter of set procedure)
                    switch(lastParameterRef->stat()){
//                       case Stat::Const:    // If const that means const is float, check if its uinteger
//                       {
//                            /*if(tclInterpreter.isStringUIntNumber(lastParameterRef->command())){ // If Uint
//                                //type = UINT_TYPE;
//                            }else{  // No Uint, Probably Integer*/
//                                if(tclInterpreter.isStringIntNumber(lastParameterRef->command())){  // Integer
//                                    type = INT_TYPE;
//                                }else{ // No Integer, for sure its Float
//                                    type = FLOAT_TYPE;
//                                }
//                            //}
//                            // Stop Loop Clear ParameterRefs
//                            value = lastParameterRef->command();
//                            parameterRefs.clear();
//                       }
//                        break;

                    case Stat::VariableSubbing:
                    {
                        Variable variable;
                        if(tclInterpreter.predefinitionsControl().findVariable(variable, lastParameterRef->command()) or tclInterpreter.predefinitionsControl().findVariableGlobally(tclInterpreter.userConfig.predefinitions(), variable, lastParameterRef->command())){
                            if(tclInterpreter.userConfig.proceduresSettings().mode() != Mode::PredefinitionsOnly or not variable.type.isEmpty()){
                                type = variable.type;
                                value = lastParameterRef->command();
                                arrayRanks = variable.arrayRanks;
                            }
                        }else{
                            if(tclInterpreter.userConfig.proceduresSettings().mode() != Mode::PredefinitionsOnly){
                                type = CHAR_TYPE;
                                arrayRanks = {128};
                                value = lastParameterRef->command();
                            }
                        }
                        parameterRefs.clear();
                    }
                        break;

                    case Stat::CommandSubbing:
                    {
                        type = CHAR_TYPE;
                        arrayRanks = {128};
                        value = lastParameterRef->command();
                        parameterRefs.clear();
                    }
                        break;

                    case Stat::Braces:
                    {
                        type = CHAR_TYPE;
                        arrayRanks = {128};
                        value = lastParameterRef->command();
                        parameterRefs.clear();
                    }
                        break;

//                    case Stat::String:
//                    {
//                        type = CHAR_TYPE;
//                        arrayRanks = {128};
//                        value = lastParameterRef->command();
//                        parameterRefs.clear();
//                    }
                        break;
                    case Stat::DoubleQuotes:
//                    case Stat::StringInQuotes:
//                    case Stat::PendingString: //TODO: Dodajac argument w postaci Pending String jako argument procedury, sprawdz czy jest stala
                    {
                        if(tclInterpreter.isStringConstNumber(lastParameterRef->command())){ // For any new string, check if its number
                            /*if(tclInterpreter.isStringUIntNumber(lastParameterRef->command())){ // If Uint
                                //type = UINT_TYPE;
                            }else{  // No Uint, Probably Integer*/
                                if(tclInterpreter.isStringIntNumber(lastParameterRef->command())){  // Integer
                                    type = INT_TYPE;
                                }else{ // No Integer, for sure its Float
                                    type = FLOAT_TYPE;
                                }
                            //}
//                            lastParameterRef->setStat(Stat::Const);
                        }else{
                            type = CHAR_TYPE;
                            arrayRanks = {128};
                        }
                        // Stop Loop Clear ParameterRefs
                        value = lastParameterRef->command();
                        parameterRefs.clear();
                    }
                        break;

//                    case Stat::Snprintf:
//                    case Stat::PendingSnprintf:
//                    {
//                        type = CHAR_TYPE;
//                        arrayRanks = {128};
//                        value = lastParameterRef->command();
//                        parameterRefs.clear();
//                    }
//                        break;

                    default:
                    {
                       throwError(ERROR_PREFIX + "Not implemented stat for initialising stat");
                       return;
                    }

                    }
                }else{
                    for(RawParameterRef rawParameter = lastParameterRef->rawParameters().constBegin();
                        rawParameter < lastParameterRef->rawParameters().constEnd(); rawParameter++)
                    {
                        switch(rawParameter->stat()){
//                           case Stat::Const:    // If const that means const is float, check if its uinteger
//                           {
//                                // For Expressions
//                                if(type.front() != FLOAT_TYPE.front()){ // Type other than FLOAT_TYPE
//                                    if(tclInterpreter.isStringIntNumber(rawParameter->command())){ // Int
//                                        type = INT_TYPE;
//                                    }else{  // Parameter is Float
//                                        type = FLOAT_TYPE;
//                                    }
//                                }else{} // Type is already float
//                           }
//                            break;
                        case Stat::VariableSubbing:
                        {

                        }
                            break;

                        case Stat::CommandSubbing:
                        {

                        }
                            break;

                        case Stat::Braces:
                        {

                        }
                            break;
                        case Stat::Word:
                        {
                            type = CHAR_TYPE;
                            arrayRanks = {128};
                            value = lastParameterRef->command();
                            parameterRefs.clear();
                        }
                            break;
                        case Stat::DoubleQuotes:
//                        case Stat::StringInQuotes:
//                        case Stat::PendingString: //TODO: Dodajac argument w postaci Pending String jako argument procedury, sprawdz czy jest stala
                        {

                        }
                            break;

//                        case Stat::Snprintf:
//                        case Stat::PendingSnprintf:
//                        {

//                        }
//                            break;

                        default:
                        {
                           throwError(ERROR_PREFIX + "Not implemented stat for initialized stat");
                           return;
                        }
                        }
                    }
                }
            }
        }else{
            type = CHAR_TYPE;
            arrayRanks = {128};
        }
    }else{
        type = CHAR_TYPE;
    }

    // Otherwise

    if(procedureCalls.last().parametersLength() == 0){
        throwError(ERROR_PREFIX + "Numb of arguments of procedure call is 0.");
        return;
    }
    tclInterpreter.predefinitionsControl().newVariable(type, procedureCalls.last().parameters().first().command());

}


TclProcedureInterpreter::ConditionInterpretFunctions TclProcedureInterpreter::conditionalInterpreterFunctions = {
    &TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::CompareNumbOfArguments>,
    &TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::IsLastSavedStat>,
    &TCLInterpreter::TCLProceduresInterpreter::executeConditionalAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Conditional::Compare>,
};

TclProcedureInterpreter::ExecutableInterpretFunctions TclProcedureInterpreter::executableInterpretFunctions = {
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Write>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::TclParse>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::Error>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::ChangeLastSavedStat>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddFunctionDefinition>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPreExpression>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddUserInteraction>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::FinalizeForEach>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddSnprintf>,
    &TCLInterpreter::TCLProceduresInterpreter::executeAction<
    TCLInterpreter::TCLProceduresInterpreter::ProcedureDefinition::Action::Executable::AddPredefinition>,
};
