#include"FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"

using ProcedureDefinitions = FSD_XML_TclCaplParserConfigInterpreter::DataModel::UserProceduresConfig;
using ProcedureDefinition = FSD_XML_TclCaplParserConfigInterpreter::DataModel::UserProceduresConfig::value_type;
//using ProcedureInterpreter = FSD_XML_TclCaplParserConfigInterpreter::DataModel::TclProcedureInterpreter;

template <>
QString FSD_XML_TclCaplParserConfigInterpreter::Config::fileFilter(){
    return QString("*.xml");
}

template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::initialize(){
    interpreterData = new FSD_XML_TclCaplParserConfigInterpreter::Data;
    data = new FileReaderInterpreter_Configuration_Priv<FSD_XML>::Data;
    interpreterData->dmStats.append({&dataModel,Stat::UserConfig});
    return true;
}

template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::deinitialize(){
    delete data;
    data = nullptr;
    delete interpreterData;
    interpreterData = nullptr;
    return true;
}

template<>
template<>
const InterpreterStatsConfiguration FSD_XML_TclCaplParserConfigInterpreter::statConfig = InterpreterStatsConfiguration::initializeStatConfig(
            QStringList{
                "userConfig",
                "settings",
                "writeOnlyFunctions",
                "procedure",
                "defaultProcedure",
                "rulesForArgument",
                "rulesForUnspecifiedArgument",
                "rulesOnEndOfCall",
                "dynamicRules",
                "rulesOnMove",
                "rule",
                "conditionalAction",
                "executableAction",
                "param",
                "formatRule",
                "paramsList"
            }
            );

using XMLStat = FSD_XML_TclCaplParserConfigInterpreter::Stat;
//using Data = FSD_XML_TclCaplParserConfigInterpreter::Data;
//using ConvertionType = FSD_XML_TclCaplParserConfigInterpreter::Data::ConversionType;
/*
 FSD_XML_TclCaplParserConfigInterpreter::Data::ConditionalActionDefinitions
 FSD_XML_TclCaplParserConfigInterpreter::Data::conditionalActionDefinitions =
 {
    // CompareNumbOfArguments
     {
      {
          { // Param 1
            ConversionType::Uint,
            -1  // Any
            //XMLStat::ActionParameter,
          }
      }
     },
     // IsLastSavedStat
     {
         {
             {
                 ConversionType::Uint,
                 //XMLStat::ActionParameter,
                 1
             }
         }
     },
     // Compare
     {
         {
             {
                 ConversionType::ParamOnly,
                 //XMLStat::ParametersList,
                 -1,
                 true // add list sizes before list
             },
             {
                 //XMLStat::ParametersList,
                 ConversionType::AnyParam,
                 -1
                 // AnyParam
             }
         }
     }
 };

 FSD_XML_TclCaplParserConfigInterpreter::Data::ExecutableActionDefinitions
 FSD_XML_TclCaplParserConfigInterpreter::Data::executableActionDefinitions =
 {
     // Write
        {
            {
             {
                 ConversionType::AnyParam,
                 -1
             }
            }
         },
     // TclParse
         {
            {
             {
                 ConversionType::AnyParam,
                 -1
             }
            }
         },
     // Error
         {
         {
             {
                 ConversionType::AnyParam,
                 -1
             }
         }
         },
     // ChangeLastSavedStat
         {
         {
             {
                 ConversionType::Uint,
                 1
                 //XMLStat::ActionParameter,

             }
         }
         },
     // AddFunctionDefinition
     {},
     // AddPreExpression
         {
         {
             {
                 ConversionType::AnyParam,
                 -1
             }
         }
         },
     // AddUserInteraction
     {},
     // FinalizeForEach
     {
         // Forbidden Action
     },
     // AddSnprintf
     {},
     // AddPredefinition
     {}
 };

const QString FSD_XML_TclCaplParserConfigInterpreter::Data::checkAndAddActionParameter(const Stat processingStat, QString processingStr)
{
    using DMStat = FSD_XML_TclCaplParserConfigInterpreter::Data::DataModelStats::const_reverse_iterator;
    DMStat lastAction = dmStats.rbegin();
    bool parametersListDetected = false;
    if(parametersListDetected = (lastAction->stat == XMLStat::ParametersList)){    // For ParametersList move iterator back
        lastAction++;
    }else{
        incrementCurActionParameterIndex();
    }
    ConversionType converstionType;
    QStringList * destinationParameters = nullptr;
    QStringList listOfArguments;
    switch (lastAction->stat) {
    case XMLStat::ConditionalAction:
    {
        destinationParameters = &static_cast<Conditional*>(lastAction->dataModel)->parameters();
        const ActionDefinition<Conditional::ActionType>::Parameters & actionDefParameters =
                conditionalActionDefinitions.at(static_cast<Data::Conditional*>(lastAction->dataModel)->cast2underlyingType()).parameters;
        // Check Current index against Definition parameters number
        if(currentActionParameterIndex <
                actionDefParameters.size())
        {
            if( (converstionType = actionDefParameters.at(currentActionParameterIndex).conversionType) < ConversionType::AnyParam){
                if(not parametersListDetected){
                    listOfArguments = processingStr.trimmed().split("\n", Qt::SkipEmptyParts);
                    for(QStringList::Iterator arg = listOfArguments.begin() ; arg < listOfArguments.end() ; arg++){
                        *arg = arg->trimmed();
                        if(arg->isEmpty())
                        {
                            int indexToRemove = arg - listOfArguments.begin();
                            listOfArguments.removeAt(indexToRemove);
                            arg = listOfArguments.begin() + indexToRemove - 1;
                        }
                    }
                    QStringList listOfArgumentsToCheck;
                    if((listOfArgumentsToCheck = listOfArguments.filter(
                            QRegularExpression( (actionDefParameters.at(currentActionParameterIndex).conversionType == ConversionType::Uint)?
                                                RegExpCore::regExpForUint : RegExpCore::regExpForInt))).size() != listOfArguments.size())
                    {
                        return QString("Argument conversion failed");
                    }
                }else{
                    return QString("Use param Token instead.");
                }
            }else{
                if(actionDefParameters.at(currentActionParameterIndex).conversionType == ConversionType::ParamOnly){
                    if(processingStat == XMLStat::ActionParameter){
                        // No Error
                    }else{
                        return QString("Only Param Token is permitted in this list.");
                    }
                }
            }

        }else{
            return QString("Too many parameters");
        }
    }
        break;
    case XMLStat::ExecutableAction:
    {
        destinationParameters = &static_cast<Executable*>(lastAction->dataModel)->parameters();
        const ActionDefinition<Executable::ActionType>::Parameters & actionDefParameters =
                executableActionDefinitions.at(static_cast<Data::Executable*>(lastAction->dataModel)->cast2underlyingType()).parameters;
        // Check Current index against Definition parameters number
        if(currentActionParameterIndex <
                actionDefParameters.size())
        {
            if((converstionType = actionDefParameters.at(currentActionParameterIndex).conversionType) < ConversionType::AnyParam){
                if(not parametersListDetected){
                    listOfArguments = processingStr.trimmed().split("\n", Qt::SkipEmptyParts);
                    for(QStringList::Iterator arg = listOfArguments.begin() ; arg < listOfArguments.end() ; arg++){
                        *arg = arg->trimmed();
                        if(arg->isEmpty())
                        {
                            int indexToRemove = arg - listOfArguments.begin();
                            listOfArguments.removeAt(indexToRemove);
                            arg = listOfArguments.begin() + indexToRemove - 1;
                        }
                    }
                    QStringList listOfArgumentsToCheck;
                    if((listOfArgumentsToCheck = listOfArguments.filter(
                            QRegularExpression( (actionDefParameters.at(currentActionParameterIndex).conversionType == ConversionType::Uint)?
                                                RegExpCore::regExpForUint : RegExpCore::regExpForInt))).size() != listOfArguments.size())
                    {
                        return QString("Argument conversion failed");
                    }
                }else{
                    return QString("Use param Token instead.");
                }
            }else{
                if(actionDefParameters.at(currentActionParameterIndex).conversionType == ConversionType::ParamOnly){
                    if(processingStat == XMLStat::ActionParameter){
                        // No Error
                    }else{
                        return QString("Only Param Token is permitted in this list.");
                    }
                }
            }
        }else{
            return QString("Too many parameters");
        }
    }
        break;
    default:
        return QString("Invalid Stat for checkActionParameter");
    }

    switch(processingStat){
    case XMLStat::FormatRule:
    {
        destinationParameters->append(QString(""));
    }
    case XMLStat::ActionParameter:
    {
        if(converstionType < ConversionType::AnyParam)
            destinationParameters->append(listOfArguments);
        else
            destinationParameters->append(processingStr);
    }
        break;
    default:
        return QString("Wrong processing stat");
    }

    return QString();
}
*/
template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>(){
    return ERROR_CALL("Internal Error: No Token");
}

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>(){
    return ERROR_CALL("XML Error: Invalid Token");
}

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>(){
    return true;
}

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>(){

    return true;
}

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>(){
    QString tokenName;
    {
        if(not FRI_COM_Manager::next(COMChannel))
            return ERROR_CALL("Internal Error: End of COMChannelData");
        NS_FRI_COM_Manager::ChannelData& dataCmd = FRI_COM_Manager::receive(COMChannel);
        if(dataCmd.command != NS_FRI_COM_Manager::Commands::DATA || !data)
            return ERROR_CALL("Internal Error: Wrong COMCommand");
        tokenName = static_cast<QXmlStreamReader*>(dataCmd.channelData)->name().toString();
        data->reader = static_cast<QXmlStreamReader*>(dataCmd.channelData);
    }
    InterpreterStatInformation statInfo;
    if(!statConfig.findStatInfo(statInfo, tokenName))
        return ERROR_CALL("Token Error: \""+ tokenName + "\" Unknown Token");
    stats.append((int)Parent_FSD::FileSpecificInterpreterStat::SIZE + statInfo.stat);
    return true;
}

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>(){
    if(interpreterData->dmStats.isEmpty())
        return ERROR_CALL("Internal Error: No DataModelStats");

    switch(interpreterData->dmStats.last().stat){
    case Stat::Procedure:
    case Stat::DefaultProcedure:
    {   // Add rulesForArguments and clear map
        interpreterData->rulesForArgumentsUsed = false;
        interpreterData->rulesForUnspecifiedArgumentUsed = false;
        interpreterData->rulesOnEndOfCallUsed = false;
        interpreterData->arguments.clear();
    }
        break;
    case Stat::RulesForArgument:
    case Stat::RulesForUnspecifiedArgument:
    {
        interpreterData->dynamicRulesUsed = false;
        interpreterData->onMoveRulesUsed = false;
    }
        break;
    case Stat::Rule:
    {
        /*interpreterData->dmStats.removeLast();
        static_cast<ProcedureDefinition::Rules*>(interpreterData->dmStats.last().dataModel)->append(interpreterData->tempRule);
        interpreterData->tempRule = ProcedureDefinition::Rule();
        return true;*/
    }
        break;
    }

    interpreterData->dmStats.removeLast();
    return true;
}

template<>
QVarLengthArray<ProcessingFunctions_FRI<FSD_XML_TclCaplParserConfigInterpreter::Config>, (int)FSD_XML_TclCaplParserConfigInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_XML_TclCaplParserConfigInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_XML_TclCaplParserConfigInterpreter::Config::processingFunctions_FS = {
    &FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>,
    &FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>,
    &FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>,
    &FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>,
    &FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>,
    &FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>
};
template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::UserConfig>(){
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL("Internal Error: UserConfig Token");

    //interpreterData->dmStats.append({pTestModule, FSD_XML_TclCaplParserConfigInterpreter::Stat::TEST_MODULE});
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Procedure>(){
    const QString PRE_ERROR_MSG = "InternalError: Procedure Token";
    DataModel* userConfig = nullptr;

    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::UserConfig) or
                not(userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);
    // Check Attributes
    QString name;
    QString userInteraction;
    if(not (config.data->reader->attributes().hasAttribute("name"))){
        qDebug() << "Procedure \"name\" attribute doesnt exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No atrribute \"name\"");
    }
    name = config.data->reader->attributes().value("name").toString().trimmed();
    if(interpreterData->listOfDefinitions.contains(name)){
        qDebug() << "Procedure \"name\" attribute has been used in this file \"" + name + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Procedure \"" + name + "\" already specified.");
    }
    /*if((config.data->reader->attributes().hasAttribute("userInteraction"))){
        userInteraction = config.data->reader->attributes().value("userInteraction").toString().trimmed();
        ProcedureInterpreter::UserInteractionStatus userInteractionStat = ProcedureInterpreter::fromStr(userInteraction);
        if(userInteractionStat == ProcedureInterpreter::UserInteractionStatus::None){
            qDebug() << ("Procedure \"userInteraction\" attribute: Unknown Value \"") + userInteraction + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Atribute \"userInteraction\": Unknown Value \"" + userInteraction + "\"");
        }
        userConfig->userProcedureConfig().append(ProcedureDefinition(name, userInteractionStat));
    }else{
        userConfig->userProcedureConfig().append(ProcedureDefinition(name));
    }*/


    interpreterData->listOfDefinitions.append(name);
    interpreterData->dmStats.append({&userConfig->userProcedureConfig().last(), Stat::Procedure });

    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Settings>(){
    const QString PRE_ERROR_MSG = "InternalError: Settings Token";
    DataModel* userConfig = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            (interpreterData->dmStats.last().stat != Stat::UserConfig) or
             not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->dmStats.append({&userConfig->proceduresSettings(),Stat::Settings });
    interpreterData->defaultProcedureUsed = true;

    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::WriteOnlyFunctions>(){
    /*using Settings = UserDefinitionsData::ProceduresSettings;
    const QString PRE_ERROR_MSG = "InternalError: WriteOnlyFunction Token";
    Settings* settings = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Settings) or
            not (settings = static_cast<Settings*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);
    // Check Attributes

    QString writeOnlyFunctions = config.data->reader->readElementText();
    settings->appendWriteOnlyProcedure(writeOnlyFunctions.trimmed().split("\n"));
*/
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::DefaultProcedure>(){
    const QString PRE_ERROR_MSG = "InternalError: DefaultProcedure Token";
    DataModel* userConfig = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            (interpreterData->dmStats.last().stat != Stat::UserConfig) or
             not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->dmStats.append({&userConfig->userDefaultProcedureConfig(),Stat::DefaultProcedure });
    interpreterData->defaultProcedureUsed = true;
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForArgument>(){
   /* const QString PRE_ERROR_MSG = "InternalError: RulesForArgument Token";
    ProcedureDefinition* definition = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Procedure or
                 interpreterData->dmStats.last().stat == Stat::DefaultProcedure) or
            not (definition = static_cast<ProcedureDefinition*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);
    // Check Attributes
    uint index;
    bool ok = false;;
    if(config.data->reader->attributes().hasAttribute("index")){
        qDebug() << "RulesForArgument \"index\" attribute dont exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No attribute \"index\"");
    }
    if((index = config.data->reader->attributes().value("index").toUInt(&ok)), !ok){
        qDebug() << "RulesForArgument \"index\" convertion error \"" + config.data->reader->attributes().value("index").toString() + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"index\" conversion error");
    }
    if(interpreterData->arguments.contains(index)){
        qDebug() << "RulesForArgument Index " + QString::number(index) + " already exists";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Argument at index " + QString::number(index) + " already exists");
    }

    if(index > 1000){
        qDebug() << "Argument index cant be greater than 1000";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Argument index cant be greater than 1000");
    }

    if(index >= definition->rulesForArguments.size())
        definition->rulesForArguments.resize(index + 1);

    interpreterData->rulesForArgumentsUsed = true;
    interpreterData->arguments.append(index);
    interpreterData->dmStats.append({&definition->rulesForArguments[index], Stat::RulesForArgument});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForUnspecifiedArgument>(){
    /*const QString PRE_ERROR_MSG = "InternalError: RulesFoUnspecifiedArgument Token";
    ProcedureDefinition* definition = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Procedure or
                 interpreterData->dmStats.last().stat == Stat::DefaultProcedure) or
            not (definition = static_cast<ProcedureDefinition*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->rulesForUnspecifiedArgumentUsed = true;
    interpreterData->dmStats.append({&definition->rulesForUnspecifiedArgument, Stat::RulesForUnspecifiedArgument});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnEndOfCall>(){
    /*const QString PRE_ERROR_MSG = "InternalError: RulesOnEndOfCall Token";
    ProcedureDefinition* definition = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Procedure or
                 interpreterData->dmStats.last().stat == Stat::DefaultProcedure) or
            not (definition = static_cast<ProcedureDefinition*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->rulesOnEndOfCallUsed = true;
    interpreterData->dmStats.append({&definition->rulesOnEndOfProcedureCall, Stat::RulesOnEndOfCall});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::DynamicRules>(){
    /*const QString PRE_ERROR_MSG = "InternalError: DynamicRules Token";
    ProcedureDefinition::RulesForArgument* rulesForArgument = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::RulesForArgument or
                 interpreterData->dmStats.last().stat == Stat::RulesForUnspecifiedArgument) or
            not(rulesForArgument = static_cast<ProcedureDefinition::RulesForArgument*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->dynamicRulesUsed = true;
    interpreterData->dmStats.append({&rulesForArgument->rules, Stat::DynamicRules});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnMove>(){
    /*const QString PRE_ERROR_MSG = "InternalError: RulesOnMove Token";
    ProcedureDefinition::RulesForArgument* rulesForArgument = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::RulesForArgument or
                 interpreterData->dmStats.last().stat == Stat::RulesForUnspecifiedArgument) or
            not (rulesForArgument = static_cast<ProcedureDefinition::RulesForArgument*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->onMoveRulesUsed = true;
    interpreterData->dmStats.append({&rulesForArgument->rulesOnMoveArgument, Stat::RulesOnMove});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Rule>(){
    /*const QString PRE_ERROR_MSG = "InternalError: Rule Token";
    using Rule = Data::Rule;
    if(interpreterData->dmStats.isEmpty() or
        Data::stat2type(interpreterData->dmStats.last().stat) < Data::stat2type(Stat::RulesOnEndOfCall) or
        Data::stat2type(interpreterData->dmStats.last().stat) > Data::stat2type(Stat::RulesOnMove))
            return config.ERROR_CALL(PRE_ERROR_MSG);
    // Check RuleControl Attribute
    if((config.data->reader->attributes().hasAttribute("controlFlag"))){
        QString controlFlagStr = config.data->reader->attributes().value("controlFlag").toString().trimmed();
        Rule::Control controlFlag = Rule::fromStr(controlFlagStr);
        if(controlFlag == Rule::Control::None){
            qDebug() << ("Rule \"controlFlag\" attribute: Unknown Value \"") + controlFlagStr + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"controlFlag\": Unknown Value \"" + controlFlagStr + "\"");
        }
        interpreterData->tempRule = Data::Rule({}, {}, controlFlag);
    }else{
        interpreterData->tempRule = Data::Rule();
    }

    interpreterData->dmStats.append({&interpreterData->tempRule, Stat::Rule});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ConditionalAction>(){
    /*const QString PRE_ERROR_MSG = "InternalError: ConditionalAction Token";
    using Action = ProcedureDefinition::Action;
    if(interpreterData->dmStats.isEmpty() or
            interpreterData->dmStats.last().stat != Stat::Rule)
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(not (config.data->reader->attributes().hasAttribute("type"))){
        qDebug() << "ConditionalAction \"type\" attribute doesnt exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"type\"");
    }
    QString conditionalStr = config.data->reader->attributes().value("type").toString().trimmed();
    Action::Conditional conditional = Action::fromStr_conditional(conditionalStr);
    if(conditional == Action::Conditional::None){
        qDebug() << ("ConditionalAction \"type\" attribute: Unknown Value \"") + conditionalStr + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"type\": Unknown Value \"" + conditionalStr + "\"");
    }

    interpreterData->clearCurActionParameterIndex();
    interpreterData->tempRule.conditions.append(Data::Conditional(conditional));
    interpreterData->dmStats.append({&interpreterData->tempRule.conditions.last(), Stat::ConditionalAction});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ExecutableAction>(){
    /*using Action = ProcedureDefinition::Action;
    const QString PRE_ERROR_MSG = "InternalError: ExecutableAction Token";
    if(interpreterData->dmStats.isEmpty() or
            interpreterData->dmStats.last().stat != Stat::Rule)
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(not (config.data->reader->attributes().hasAttribute("type"))){
        qDebug() << "ExecutableAction \"type\" attribute doesnt exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"type\"");
    }
    QString executableStr = config.data->reader->attributes().value("type").toString().trimmed();
    Action::Executable executable = Action::fromStr_executable(executableStr);
    if(executable == Action::Executable::None){
        qDebug() << ("ExecutableAction \"type\" attribute: Unknown Value \"") + executableStr + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"type\": Unknown Value \"" + executableStr + "\"");
    }

    interpreterData->clearCurActionParameterIndex();
    interpreterData->tempRule.actions.append(Data::Executable(executable));
    interpreterData->dmStats.append({&interpreterData->tempRule.actions.last(), Stat::ExecutableAction});
    return true;*/
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ActionParameter>(){
    const Stat THIS_STAT = Stat::ActionParameter;
    const QString PRE_ERROR_MSG = "InternalError: ActionParameter Token";
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::ExecutableAction or
                 interpreterData->dmStats.last().stat == Stat::ConditionalAction or
                 interpreterData->dmStats.last().stat == Stat::ParametersList))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    QString str = config.data->reader->readElementText();
    if(str.isEmpty()){
        qDebug() << "ActionParameter cant be empty";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - ActionParameter cant be empty.");
    }

   /* if(const QString error = interpreterData->checkAndAddActionParameter(THIS_STAT, str);
            not error.isEmpty())
    {
        return config.ERROR_CALL(PRE_ERROR_MSG + error);
    }*/


    return true;
}
//TODO: Dodaj ErrorHandling dla przypadków nieodpowiednich parent tokenów
template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::FormatRule>(){
    /*using Format = ProcedureDefinition::Format;
    const Stat THIS_STAT = Stat::FormatRule;
    const QString PRE_ERROR_MSG = "InternalError: FormatRule Token";
    bool ok = true;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::ExecutableAction or
                 interpreterData->dmStats.last().stat == Stat::ConditionalAction or
                 interpreterData->dmStats.last().stat == Stat::ParametersList))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(not (config.data->reader->attributes().hasAttribute("type"))){
        qDebug() << "FormatRule \"type\" attribute doesnt exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No attribute \"type\"" );
    }
    QString formatRuleName = config.data->reader->attributes().value("type").toString();
    Format::Rule formatRule = Format::fromStr_rule(formatRuleName);
    if(formatRule == Format::Rule::None){
        qDebug() << ("FormatRule \"type\" attribute: Unknown Value \"") + formatRuleName + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"type\": Unknown Value \"" + formatRuleName + "\"");
    }

    QString str = config.data->reader->readElementText();

    // Check Rule Arguments with type Enum ()
    switch(formatRule){
    case Format::Rule::SEPARATOR:
        {
            //str = str.trimmed();
            break;
        }
    case Format::Rule::ARGS_AFTER_INDEX:
        if(str.isEmpty()){
            qDebug() << "Wrong FormatRule argument for Rule \"" + formatRuleName + "\": Can't be empty";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Argument for Rule \"" + formatRuleName + "\" cant be empty ");
        }
        if(false){
    case Format::Rule::INDEX_OR_FULL_LINE:

            str = str.trimmed();
        }        
        if(not str.isEmpty() and (str.toInt(&ok), !ok)){
            qDebug() << "Wrong FormatRule argument for Rule \"" + formatRuleName + "\": \"" + str + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Wrong Argument for Rule\"" + formatRuleName + "\": \"" + str + "\"");
        }
        break;
    case Format::Rule::TARGET:
    {
        str = str.trimmed();
        Format::Target formatTarget = Format::fromStr_target(str);
        if(formatTarget == Format::Target::None){
            qDebug() << "Wrong FormatRule argument for Rule \"" + formatRuleName + "\": \"" + str + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Wrong Argument for Rule \"" + formatRuleName + "\": \"" + str + "\"");
        }
        str = QString::number(static_cast<std::underlying_type_t<Format::Target>>(formatTarget));
    }
        break;
    default:
        break;
    }

    if(const QString error = interpreterData->checkAndAddActionParameter(
                THIS_STAT, Format::cast_format_rule_str(formatRule) + str);
            not error.isEmpty())
    {
        return config.ERROR_CALL(PRE_ERROR_MSG + error);
    }
*/

    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ParametersList>(){
    const Stat THIS_STAT = Stat::ParametersList;
    const QString PRE_ERROR_MSG = "InternalError: ParametersList Token";
    interpreterData->parametersList = InterpreterData::ParametersList();// Reset ParametersList
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::ConditionalAction or
                 interpreterData->dmStats.last().stat == Stat::ExecutableAction))
        return config.ERROR_CALL(PRE_ERROR_MSG);

   // interpreterData->incrementCurActionParameterIndex();
    interpreterData->dmStats.append({nullptr, Stat::ParametersList});
    return true;
}

template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_XML_TclCaplParserConfigInterpreter::Config>> FSD_XML_TclCaplParserConfigInterpreter::processingFunctions = {
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::UserConfig>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Settings>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::WriteOnlyFunctions>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Procedure>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::DefaultProcedure>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForArgument>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForUnspecifiedArgument>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnEndOfCall>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::DynamicRules>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnMove>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Rule>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ConditionalAction>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ExecutableAction>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ActionParameter>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::FormatRule>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ParametersList>,
};
