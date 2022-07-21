#include"FRI_FSD_XML_TCL_CAPL_Parser_Config.hpp"

//using ProcedureDefinitions = FSD_XML_TclCaplParserConfigInterpreter::DataModel::;
//using ProcedureDefinition = FSD_XML_TclCaplParserConfigInterpreter::DataModel::UserProceduresConfig::value_type;
//using ProcedureInterpreter = FSD_XML_TclCaplParserConfigInterpreter::DataModel::TclProcedureInterpreter;
//using XMLToken = FSD_XML_TclCaplParserConfigInterpreter::Data::XMLToken;

template <>
QString FSD_XML_TclCaplParserConfigInterpreter::Config::fileFilter(){
    return QString("*.xml");
}

//const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data::paramToken =
//{
//    .name = "param",
//    .requiredTextContent = true
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data::formatParamToken =
//{
//    .name = "formatParam",
//    .requiredAttributes =
//    {
//        "type"
//    }
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data::conditionalActionToken =
//{
//    .name = "conditionalAction",
//    .optionalTokens =
//     {
//         &paramToken,
//         &formatParamToken
//     },
//     .requiredAttributes =
//    {
//        "type"
//    }
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: executableActionToken =
//{
//    .name = "executableAction",
//     .optionalTokens =
//     {
//         &paramToken,
//         &formatParamToken
//     },
//    .requiredAttributes =
//    {
//        "type"
//    }
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: ruleToken =
//{
//    .name = "rule",
//     .optionalTokens =
//     {
//         &conditionalActionToken,
//         &executableActionToken
//     },
//    .optionalAttributes =
//    {
//        "controlFlag"
//    },
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: dynamicRulesToken =
//{
//    .name = "dynamicRules",
//     .optionalTokens =
//     {
//         &ruleToken
//     },
//    .expectedNoDuplicates = true,
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: rulesOnMoveToken =
//{
//    .name = "rulesOnMove",
//     .optionalTokens =
//     {
//         &ruleToken
//     },
//    .expectedNoDuplicates = true,
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: rulesForArgumentToken =
//{
//    .name = "rulesForArgument",
//     .optionalTokens =
//     {
//         &dynamicRulesToken,
//         &rulesOnMoveToken
//     },
//     .requiredAttributes =
//     {
//         "index"
//     },
//    .expectedNoDuplicates = true,
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: rulesForUnspecifiedArgumentToken =
//{
//    .name = "rulesForUnspecifiedArgument",
//     .optionalTokens =
//     {
//         &dynamicRulesToken,
//         &rulesOnMoveToken
//     },
//    .expectedNoDuplicates = true,
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: rulesOnEndOfCallToken =
//{
//    .name = "rulesOnEndOfCall",
//    .optionalTokens =
//    {
//        &ruleToken
//    }
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: procedureToken =
//{
//    .name = "procedure",
//     .optionalTokens =
//     {
//         &rulesForArgumentToken,
//         &rulesForUnspecifiedArgumentToken,
//         &rulesOnEndOfCallToken
//     },
//    .requiredAttributes =
//    {
//        "name"
//    },
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: defaultProcedureToken =
//{
//    .name = "defaultProcedure",
//     .optionalTokens =
//     {
//         &rulesForArgumentToken,
//         &rulesForUnspecifiedArgumentToken,
//         &rulesOnEndOfCallToken
//     },
//    .expectedNoDuplicates = true,
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: writeOnlyProcedureToken =
//{
//    .name = "procedure",
//    .requiredTextContent = true
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: writeOnlyProceduresToken =
//{
//    .name = "writeOnlyProcedures",
//     .optionalTokens =
//     {
//         &writeOnlyProcedureToken
//     },
//    .expectedNoDuplicates = true,
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: settingsToken =
//{
//    .name = "settings",
//     .optionalTokens =
//     {
//         &writeOnlyProceduresToken
//     },
//    .expectedNoDuplicates = true,
//};

// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: userConfigToken =
//{
//    .name = "userConfig",
//     .optionalTokens =
//     {
//         &settingsToken,
//         &procedureToken,
//         &defaultProcedureToken
//     },
//    .expectedNoDuplicates = true,
//};



// const XMLToken FSD_XML_TclCaplParserConfigInterpreter::Data:: xmlDefinition = {
//    .requiredTokens =
//    {
//        &userConfigToken
//    }
//};

template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::initialize(){
    interpreterData = new FSD_XML_TclCaplParserConfigInterpreter::Data;
    data = new FileReaderInterpreter_Configuration_Priv<FSD_XML>::Data;
    interpreterData->dmStats.append({&dataModel,Stat::TclConfig});

    /*XMLObject globalXMLObject = XMLObject::createXmlObject(errorMsg,interpreterData->xmlDefinition);
    if(not errorMsg.isEmpty())
        return ERROR_CALL(errorMsg);
    interpreterData->xmlObjects.push(globalXMLObject);*/
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
                "tclConfig",
                "settings",
                "writeOnlyProcedures",
                "procedures",
                "procedure",
                "defaultProcedure",
                "rulesForArgument",
                "rulesForUndefinedArgument",
                "rulesOnEndOfCall",
                "rulesOnInit",
                "rulesOnMove",
                "rawRule",
                "conditionalAction",
                "executableAction",
                "param",
                "formatRule"
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
    {
        if(not interpreterData->procedureNameTokenAppeared){
            return ERROR_CALL("Internal Error: Corrupted Procedure token");
        }
        interpreterData->procedureNameTokenAppeared = false;
    }
        Q_FALLTHROUGH();
    case Stat::DefaultProcedure:
    {
        interpreterData->rulesOnEndOfCallUsed = false;
        interpreterData->rulesForUnspecifiedArgumentUsed = false;
    }
        break;
    case Stat::RulesForArgument:
    case Stat::RulesForUnspecifiedArgument:
    {

        //interpreterData->dynamicRulesUsed = false;
        interpreterData->onMoveRulesUsed = false;
    }
        break;
    case Stat::RulesOnMove:
    {
        if(interpreterData->dmStats.size() > 2)
            return ERROR_CALL("Internal Error: Not enough DataModelStats");
        if((interpreterData->dmStats.rbegin() + 1)->stat == Stat::RulesForArgument
                or (interpreterData->dmStats.rbegin() + 1)->stat == Stat::RulesForUnspecifiedArgument)
        {
            if((interpreterData->dmStats.rbegin() + 2)->stat == Stat::Procedure){
                if(interpreterData->procedureNameTokenAppeared){
                    // Load Rules
                    DataModel* userConfig = nullptr;
                    if(not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.at(0).dataModel)))
                        return config.ERROR_CALL("Internal Error");

                    InterpreterData::RulesView* rulesView = nullptr;
                    if(not (rulesView = static_cast<InterpreterData::RulesView*>(interpreterData->dmStats.last().dataModel))
                            or userConfig->addIndex(interpreterData->procedureName, *rulesView))
                        return config.ERROR_CALL("Internal Error");
                }else{
                    return ERROR_CALL("Internal Error: Procedure name is undefined.");
                }
            }else{ // Default procedure
                // Load rules for Default
                DataModel* userConfig = nullptr;
                if(not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.at(0).dataModel)))
                    return config.ERROR_CALL("Internal Error");

                InterpreterData::RulesView* rulesView = nullptr;
                if(not (rulesView = static_cast<InterpreterData::RulesView*>(interpreterData->dmStats.last().dataModel))
                        or userConfig->addIndex(*rulesView))
                    return config.ERROR_CALL("Internal Error");
            }
        }else{ // Wrong DataModelStat
            return ERROR_CALL("Internal Error: Wrong DataModelStat");
        }
    }
        break;
    case Stat::RulesOnEndOfCall:
    {
        if(interpreterData->dmStats.size() > 1)
            return ERROR_CALL("Internal Error: Not enough DataModelStats");
        if((interpreterData->dmStats.rbegin() + 1)->stat == Stat::Procedure){
            if(interpreterData->procedureNameTokenAppeared){
                DataModel* userConfig = nullptr;
                if(not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.at(0).dataModel)))
                    return config.ERROR_CALL("Internal Error");

                InterpreterData::RulesView* rulesView = nullptr;
                if(not (rulesView = static_cast<InterpreterData::RulesView*>(interpreterData->dmStats.last().dataModel))
                        or userConfig->addIndex(interpreterData->procedureName, *rulesView))
                    return config.ERROR_CALL("Internal Error");
            }else{
                return ERROR_CALL("Internal Error: Procedure name is undefined.");
            }
        }else{ // Default procedure
            // Load rules for Default
            DataModel* userConfig = nullptr;
            if(not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.at(0).dataModel)))
                return config.ERROR_CALL("Internal Error");

            InterpreterData::RulesView* rulesView = nullptr;
            if(not (rulesView = static_cast<InterpreterData::RulesView*>(interpreterData->dmStats.last().dataModel))
                    or userConfig->addIndex(*rulesView))
                return config.ERROR_CALL("Internal Error");
        }
    }
        break;
    case Stat::RawRule:
    {
        /*interpreterData->dmStats.removeLast();
        static_cast<ProcedureDefinition::Rules*>(interpreterData->dmStats.last().dataModel)->append(interpreterData->tempRule);
        interpreterData->tempRule = ProcedureDefinition::Rule();
        return true;*/
    }
        break;
    case Stat::ActionParameter:
    {
        // To finalize same parameters
        InterpreterData::DataModelStats::reverse_iterator dmStatWithAction = interpreterData->dmStats.rbegin();
        while(dmStatWithAction != interpreterData->dmStats.rend() and dmStatWithAction->stat == Stat::ActionParameter)
            dmStatWithAction++;

        if(dmStatWithAction == interpreterData->dmStats.rend() or  // Action not found
                not (dmStatWithAction->stat == Stat::ExecutableAction or
                     dmStatWithAction->stat == Stat::ConditionalAction) or // No action before Parameter
                interpreterData->currentActionParamIndexes.size() - 1 != (dmStatWithAction - interpreterData->dmStats.rbegin())) // Always 0 level is 1 level - 0 level always exists with action
        {
            config.ERROR_CALL("Internal Error");
        }
        switch(dmStatWithAction->stat){
        case Stat::ConditionalAction:
        {
            using Action = InterpreterData::ConditionalAction;
            using ActionType = ConditionalsTypes;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            switch(action->type()){
            case ActionType::CompareAndWrite:
            {
                using Action = ConditionalsFactory::Product<ActionType::CompareAndWrite>;
                Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
                if(interpreterData->currentActionParamIndexes.size() == 1){
                    int level0Position = interpreterData->currentActionParamIndexes.at(0);
                    if( level0Position != 0 and level0Position % 2 == 0 /*ListString parameter*/){
                        action.argumentsMap().insert(interpreterData->tempIndex, interpreterData->tempStringList);
                    }
                }
            }
                break;
            default:
                break;
            }
        }
            break;
        case Stat::ExecutableAction:
        {
            using Action = InterpreterData::ExecutableAction;
            using ActionType = ConditionalsTypes;
            //Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
        }
            break;
        default:
            break;
        }

        // ------------------------------

        if(not interpreterData->currentActionParamIndexes.isEmpty())
            interpreterData->currentActionParamIndexes.removeLast();
        else
            return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        if(not interpreterData->currentActionParamIndexes.isEmpty())
            interpreterData->currentActionParamIndexes.last()++;
        else
            return ERROR_CALL("Internal Error: Parameters structure corrupted.");
    }
        break;
    case Stat::ConditionalAction:
    {
        if(interpreterData->currentActionParamIndexes.size() != 1)
            return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        using Action = InterpreterData::ConditionalAction;
        using ActionType = ConditionalsTypes;
        Action& action = *static_cast<Action*>(interpreterData->dmStats.last().dataModel);
        switch(action->type()){
        case ActionType::IsLastSavedStat:
        {

        }
            break;
        case ActionType::Compare:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 2)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        case ActionType::CompareAndWrite:
        {
            if(interpreterData->currentActionParamIndexes.at(0) <= 1
                    and not interpreterData->compareAndWrite_IndexUsed and not interpreterData->compareAndWrite_ListOfArgumentsUsed)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        case ActionType::CompareNumbOfArguments:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 1)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        default:
            break;
        }

        interpreterData->currentActionParamIndexes.removeLast();
        if(not interpreterData->currentActionParamIndexes.isEmpty())
            return ERROR_CALL("Internal Error: Parameters structure corrupted.");
    }
        break;
    case Stat::ExecutableAction:
    {
        if(interpreterData->currentActionParamIndexes.size() != 1)
            return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        using Action = InterpreterData::ExecutableAction;
        using ActionType = ExecutablesTypes;
        Action& action = *static_cast<Action*>(interpreterData->dmStats.last().dataModel);
        switch(action->type()){
        case ActionType::AddUserInteraction:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 1)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        case ActionType::AddFunctionDefinition:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 1)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        case ActionType::AddPreExpression:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 1)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        case ActionType::AddPredefinition:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 1)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        case ActionType::Error:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 1)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        case ActionType::Write:
        {
            if(interpreterData->currentActionParamIndexes.at(0) != 1)
                return ERROR_CALL("Internal Error: Parameters structure corrupted.");
        }
            break;
        default:
            break;
        }

        interpreterData->currentActionParamIndexes.removeLast();
        if(not interpreterData->currentActionParamIndexes.isEmpty())
            return ERROR_CALL("Internal Error: Parameters structure corrupted.");
    }
        break;
    default:
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
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::TclConfig>(){
    const QString PRE_ERROR_MSG = "InternalError: TclConfig Token";
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(interpreterData->tclConfigUsed)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - TclConfig token already defined.");
    interpreterData->tclConfigUsed = true;
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Settings>(){
    const QString PRE_ERROR_MSG = "InternalError: Settings Token";
    DataModel* userConfig = nullptr;
    if(interpreterData->dmStats.isEmpty() or
             not (interpreterData->dmStats.last().stat == Stat::TclConfig) or
             not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(interpreterData->settingsTokenUsed)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Settings token already defined.");

    interpreterData->dmStats.append({&userConfig->settings(),Stat::Settings });
    interpreterData->settingsTokenUsed = true;
    return true;
}


template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::WriteOnlyFunctions>(){
    const QString PRE_ERROR_MSG = "InternalError: WriteOnlyFunction Token";
    Settings* settings = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Settings) or
            not (settings = static_cast<Settings*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);
    // Check Attributes

    if(interpreterData->writeOnlyFunctionsUsed)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - WriteOnlyFunction token already defined.");

    interpreterData->dmStats.append({&settings->writeOnlyProcedures(),Stat::WriteOnlyFunctions });
    interpreterData->writeOnlyFunctionsUsed = true;
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Procedures>(){
    const QString PRE_ERROR_MSG = "InternalError: Procedures Token";
    DataModel* userConfig = nullptr;

    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::TclConfig) or
                not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(interpreterData->proceduresUsed)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Procedures token already defined.");
    interpreterData->dmStats.append({userConfig, Stat::Procedures });

    interpreterData->proceduresUsed = true;
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Procedure>(){
    const QString PRE_ERROR_MSG = "InternalError: Procedure Token";
    DataModel* userConfig = nullptr;

    if(interpreterData->dmStats.isEmpty() or
                not (interpreterData->dmStats.last().stat == Stat::WriteOnlyFunctions
                    or interpreterData->dmStats.last().stat == Stat::Procedures) or
                not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    // Prepare name
    /*
    if((config.data->reader->attributes().hasAttribute("userInteraction")))
    {

        if(userConfig->addProcedure(interpreterData->procedureName) == false){
            qDebug() << "Add Procedure failed - procedure name duplicated";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Procedure name duplicated");
        }
        userConfig->addProcedure(interpreterData->procedureName); // Variant for userInteraction - Maybe deprected
    }else{
        if(userConfig->addProcedure(interpreterData->procedureName) == false){
            qDebug() << "Add Procedure failed - procedure name duplicated";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Procedure name duplicated");
        }
    }*/

    // Add to map
    //interpreterData->listOfDefinitions.append(name);
    interpreterData->dmStats.append({userConfig, Stat::Procedure });

    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Name>(){
    const QString PRE_ERROR_MSG = "InternalError: Name Token";
    DataModel* userConfig = nullptr;

    if(interpreterData->dmStats.isEmpty() or
                not (interpreterData->dmStats.last().stat == Stat::Procedure) or
                not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(interpreterData->procedureNameTokenAppeared){
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Name token already defined.");
    }
    // Prepare name

    interpreterData->procedureName = config.data->reader->readElementText(QXmlStreamReader::IncludeChildElements);

    if(userConfig->addProcedure(interpreterData->procedureName) == false){
        qDebug() << "Add Procedure failed - procedure name duplicated";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Procedure name duplicated");
    }
    interpreterData->procedureNameTokenAppeared = true;
    return true;
}



template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::DefaultProcedure>(){
    const QString PRE_ERROR_MSG = "InternalError: DefaultProcedure Token";
    DataModel* userConfig = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            (interpreterData->dmStats.last().stat != Stat::TclConfig) or
             not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(interpreterData->defaultProcedureUsed){
        return config.ERROR_CALL(PRE_ERROR_MSG + "DefaultProcedure token already defined.");
    }

    interpreterData->dmStats.append({userConfig,Stat::DefaultProcedure });
    interpreterData->defaultProcedureUsed = true;
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForArgument>(){
    const QString PRE_ERROR_MSG = "InternalError: RulesForArgument Token";
    DataModel* userConfig = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Procedure or
                 interpreterData->dmStats.last().stat == Stat::DefaultProcedure) or
            not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);
    // Check Attributes
    uint index;
    bool ok = false;;
    if(config.data->reader->attributes().hasAttribute("index")){
        qDebug() << "RulesForArgument \"index\" attribute dont exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No attribute \"index\"");
    }
    if(static_cast<void>((index = config.data->reader->attributes().value("index").trimmed().toUInt(&ok))), !ok){
        qDebug() << "RulesForArgument \"index\" convertion error \"" + config.data->reader->attributes().value("index").toString() + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"index\" conversion error");
    }


    interpreterData->tempRulesView.index = static_cast<DataModel::RulesCategories>(index);
    interpreterData->dmStats.append({&interpreterData->tempRulesView, Stat::RulesForArgument});

    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForUnspecifiedArgument>(){
    const QString PRE_ERROR_MSG = "InternalError: RulesFoUndefinedArgument Token";
    DataModel* userConfig = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Procedure or
                 interpreterData->dmStats.last().stat == Stat::DefaultProcedure) or
            not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(interpreterData->rulesForUnspecifiedArgumentUsed){
        return config.ERROR_CALL(PRE_ERROR_MSG + "RulesFoUndefinedArgument token already defined.");
    }

    interpreterData->rulesForUnspecifiedArgumentUsed = true;
    interpreterData->tempRulesView.index = DataModel::RulesCategories::UndefinedArgument;
    interpreterData->dmStats.append({&interpreterData->tempRulesView, Stat::RulesForUnspecifiedArgument});
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnEndOfCall>(){
    const QString PRE_ERROR_MSG = "InternalError: RulesOnEndOfCall Token";
    DataModel* userConfig = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::Procedure or
                 interpreterData->dmStats.last().stat == Stat::DefaultProcedure) or
            not (userConfig = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(interpreterData->rulesOnEndOfCallUsed){
        return config.ERROR_CALL(PRE_ERROR_MSG + "RulesOnEndOfCall token already defined.");
    }
    interpreterData->rulesOnEndOfCallUsed = true;
    interpreterData->tempRulesView.index = DataModel::RulesCategories::OnEndOfCall;
    interpreterData->dmStats.append({&interpreterData->tempRulesView, Stat::RulesOnEndOfCall});
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnInit>(){
    const QString PRE_ERROR_MSG = "InternalError: RulesOnInit Token";
    return config.ERROR_CALL(PRE_ERROR_MSG + " - Not supported");
    /*InterpreterData::RulesView* rulesView = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::RulesForArgument or
                 interpreterData->dmStats.last().stat == Stat::RulesForUnspecifiedArgument) or
            not(rulesForArgument = static_cast<ProcedureDefinition::RulesForArgument*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->dynamicRulesUsed = true;
    interpreterData->dmStats.append({&rulesForArgument->rules, Stat::DynamicRules});*/
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnMove>(){
    const QString PRE_ERROR_MSG = "InternalError: RulesOnMove Token";
    Data::RulesView* rulesView = nullptr;
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::RulesForArgument or
                 interpreterData->dmStats.last().stat == Stat::RulesForUnspecifiedArgument) or
            not (rulesView = static_cast<InterpreterData::RulesView*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);
    if(interpreterData->onMoveRulesUsed){
        return config.ERROR_CALL(PRE_ERROR_MSG + "RulesOnMove token already defined.");
    }
    interpreterData->onMoveRulesUsed = true;
    interpreterData->dmStats.append({&rulesView->rules, Stat::RulesOnMove});
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RawRule>(){
    const QString PRE_ERROR_MSG = "InternalError: RawRule Token";
    using Rule = Data::Rule;
    using InterpreterRule = UserProcedureRule;
    using RuleType = RulesFactory::ProductTypeEnum;
    using RawRule = Data::RawRule;
    Data::RulesView* rulesView = nullptr;
    RawRule* rawRule = nullptr;
    if(interpreterData->dmStats.isEmpty() or
        Data::stat2type(interpreterData->dmStats.last().stat) < Data::stat2type(Stat::RulesOnEndOfCall) or
        Data::stat2type(interpreterData->dmStats.last().stat) > Data::stat2type(Stat::RulesOnMove) or
            not (rulesView = static_cast<InterpreterData::RulesView*>(interpreterData->dmStats.last().dataModel)))
            return config.ERROR_CALL(PRE_ERROR_MSG);
    // Check RuleControl Attribute
    if((config.data->reader->attributes().hasAttribute("controlFlag"))){
        QString controlFlagStr = config.data->reader->attributes().value("controlFlag").toString().trimmed();
        ControlFlag controlFlag = InterpreterRule::fromStr(controlFlagStr);
        if(controlFlag == InterpreterRule::Control::None){
            qDebug() << ("Rule \"controlFlag\" attribute: Unknown Value \"") + controlFlagStr + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"controlFlag\": Unknown Value \"" + controlFlagStr + "\"");
        }
        rawRule = static_cast<RawRule*>(RulesFactory::create(RuleType::RawRule));
        rawRule->setControlFlag(controlFlag);
    }else{
        rawRule = static_cast<RawRule*>(RulesFactory::create(RuleType::RawRule));
    }
    rulesView->rules.append(rawRule);

    interpreterData->dmStats.append({rawRule, Stat::RawRule});
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ConditionalAction>(){
    const QString PRE_ERROR_MSG = "InternalError: ConditionalAction Token";
    using InterpreterAction = UserProcedure::Action;
    using Action = Data::ConditionalAction;
    using ActionType = ConditionalsTypes;
    using RawRule = Data::RawRule;
    RawRule* rawRule = nullptr;

    if(interpreterData->dmStats.isEmpty() or
            interpreterData->dmStats.last().stat != Stat::RawRule or
            not interpreterData->currentActionParamIndexes.isEmpty() or
            not (rawRule = static_cast<RawRule*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(not (config.data->reader->attributes().hasAttribute("type"))){
        qDebug() << "ConditionalAction \"type\" attribute doesnt exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"type\"");
    }
    QString conditionalStr = config.data->reader->attributes().value("type").toString().trimmed();
    ActionType conditional = InterpreterAction::fromStr_conditional(conditionalStr);
    if(conditional == ActionType::None){
        qDebug() << ("ConditionalAction \"type\" attribute: Unknown Value \"") + conditionalStr + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"type\": Unknown Value \"" + conditionalStr + "\"");
    }

    Action action = ConditionalsFactory::create(conditional);
    rawRule->conditions().append(action);
    interpreterData->currentActionParamIndexes.append(0);
    interpreterData->dmStats.append({action, Stat::ConditionalAction});
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ExecutableAction>(){
    const QString PRE_ERROR_MSG = "InternalError: ExecutableAction Token";
    using InterpreterAction = UserProcedure::Action;
    using Action = Data::ExecutableAction;
    using ActionType = ExecutablesTypes;
    using RawRule = Data::RawRule;
    RawRule* rawRule = nullptr;

    if(interpreterData->dmStats.isEmpty() or
            interpreterData->dmStats.last().stat != Stat::RawRule or
            not interpreterData->currentActionParamIndexes.isEmpty() or
            not (rawRule = static_cast<RawRule*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(not (config.data->reader->attributes().hasAttribute("type"))){
        qDebug() << "ExecutableAction \"type\" attribute doesnt exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"type\"");
    }
    QString executableStr = config.data->reader->attributes().value("type").toString().trimmed();
    ActionType executable = InterpreterAction::fromStr_executable(executableStr);
    if(executable == ActionType::None){
        qDebug() << ("ExecutableAction \"type\" attribute: Unknown Value \"") + executableStr + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"type\": Unknown Value \"" + executableStr + "\"");
    }

    Action action = ExecutablesFactory::create(executable);
    rawRule->executables().append(action);
    interpreterData->currentActionParamIndexes.append(0);
    interpreterData->dmStats.append({action, Stat::ExecutableAction});
    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ActionParameter>(){
    const Stat THIS_STAT = Stat::ActionParameter;
    const QString PRE_ERROR_MSG = "InternalError: ActionParameter Token";
    if(interpreterData->dmStats.isEmpty() or
            not (interpreterData->dmStats.last().stat == Stat::ExecutableAction or
                 interpreterData->dmStats.last().stat == Stat::ConditionalAction or
                 interpreterData->dmStats.last().stat == Stat::ActionParameter))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    // Verification of parameters level control + get action
    Data::DataModelStats::reverse_iterator dmStatWithAction = interpreterData->dmStats.rbegin();
    while(dmStatWithAction != interpreterData->dmStats.rend() and dmStatWithAction->stat == Stat::ActionParameter)
        dmStatWithAction++;

    if(dmStatWithAction == interpreterData->dmStats.rend() or  // Action not found
            not (dmStatWithAction->stat == Stat::ExecutableAction or
                 dmStatWithAction->stat == Stat::ConditionalAction) or // No action before Parameter
            interpreterData->currentActionParamIndexes.size() - 1 != (dmStatWithAction - interpreterData->dmStats.rbegin())) // Always 0 level is 1 level - 0 level always exists with action
    {
        config.ERROR_CALL(PRE_ERROR_MSG);
    }

    void* paramData = nullptr;

    switch(dmStatWithAction->stat){
    case Stat::ConditionalAction:
    {
        using Action = Data::ConditionalAction;
        using ActionType = ConditionalsTypes;
        Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
        switch(action->type()){
        case ActionType::CompareNumbOfArguments:
        {
            using Action = ConditionalsFactory::Product<ActionType::CompareNumbOfArguments>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            // Description
            /*
                { Level 0 - Index 0 - List of parameters
                    { Level 1 - String parameters

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 0
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0:
                {
                    // Continue - create
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong number of parameters");
                }
            }
                break;
            case 1: // Level 1
            {
                // Only 1 parameter numbOfArgs in Level 0 possible - No verification nessasery
                // Only index value validation
                uint index;
                QString value;
                bool ok = false;;
                if(config.data->reader->attributes().hasAttribute("value")){
                    qDebug() << "Param \"value\" attribute dont exist";
                    return config.ERROR_CALL(PRE_ERROR_MSG + " - No attribute \"value\"");
                }
                value = config.data->reader->attributes().value("value").trimmed().toString();
                if(static_cast<void>((index = value.toUInt(&ok))), !ok){
                    qDebug() << "Param \"value\" convertion error \"" + value + "\"";
                    return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"value\" conversion error");
                }
                action.numbOfArgs().append(value);
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        case ActionType::CompareAndWrite:
        {
            using Action = ConditionalsFactory::Product<ActionType::CompareAndWrite>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            // Description
            /*
                { Level 1 - Index 0 - String -> Uint numbOfArguments

                },
                { Level 1 - Index i... -  Index Int

                }
                { Level 1 - Index i + 1... -  List of ParametersStrings
                    { Level 2 - String parameters

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 0 String -> Uint numbOfArguments
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0:
                {
                    int index;
                    QString value;
                    bool ok = false;
                    if(config.data->reader->attributes().hasAttribute("value")){
                        qDebug() << "Param \"value\" attribute dont exist";
                        return config.ERROR_CALL(PRE_ERROR_MSG + " - No attribute \"value\"");
                    }
                    value = config.data->reader->attributes().value("value").trimmed().toString();
                    if(not value.isEmpty()){
                        if(static_cast<void>((index = value.toInt(&ok))), !ok){
                            qDebug() << "Param \"value\" convertion error \"" + value + "\"";
                            return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"index\" conversion error");
                        }
                    }
                    action.numbOfArguments() = value;
                }
                    break;
                default:
                {
                    switch(interpreterData->currentActionParamIndexes.at(0) % 2){
                    case 1: // Index i... -  Index Int
                    {
                        if((interpreterData->compareAndWrite_IndexUsed and interpreterData->compareAndWrite_ListOfArgumentsUsed)
                                or (not interpreterData->compareAndWrite_IndexUsed and not interpreterData->compareAndWrite_ListOfArgumentsUsed)) // List of String not ready
                            return config.ERROR_CALL("Internal Error: Parameters structure corrupted.");
                        interpreterData->compareAndWrite_IndexUsed = true;
                        interpreterData->compareAndWrite_ListOfArgumentsUsed = false;
                        int index;
                        QString value;
                        bool ok = false;
                        if(config.data->reader->attributes().hasAttribute("value")){
                            qDebug() << "Param \"value\" attribute dont exist";
                            return config.ERROR_CALL(PRE_ERROR_MSG + " - No attribute \"value\"");
                        }
                        value = config.data->reader->attributes().value("value").trimmed().toString();
                        if(static_cast<void>((index = value.toInt(&ok))), !ok){
                            qDebug() << "Param \"value\" convertion error \"" + value + "\"";
                            return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"index\" conversion error");
                        }
                        interpreterData->tempIndex = index;
                    }
                        break;
                    case 0: // Index i + 1... -  List of ParametersStrings
                    {
                        // Continue - create
                        if(not interpreterData->compareAndWrite_IndexUsed and interpreterData->compareAndWrite_ListOfArgumentsUsed)// List of Index ready
                            return config.ERROR_CALL("Internal Error: Parameters structure corrupted.");
                        interpreterData->compareAndWrite_ListOfArgumentsUsed = true;
                        interpreterData->tempStringList.clear();
                    }
                    default:
                        break;
                    }
                }
                    break;
                }
            }
                break;
            case 1: // Level 1
            {
                int level0Position = interpreterData->currentActionParamIndexes.at(0);
                if(level0Position == 0 or level0Position % 2 == 1 /*Index parameter*/){
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
                }
                // Only 1 parameter numbOfArgs in Level 0 possible - No verification nessasery
                // Only index value validation
                interpreterData->tempStringList.append(config.data->reader->readElementText(QXmlStreamReader::IncludeChildElements));
                return config.FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>();

                // On End of Param - remove duplicates
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        case ActionType::Compare:
        {
            using Action = ConditionalsFactory::Product<ActionType::Compare>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);

            // Description
            /*
                { Level 1 - Index 0 - List of String
                    { Level 2 - String parameters

                    }
                },
                { Level 1 - Index 1 -  List of FormatRules
                    { Level 2 - Format Rules

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 1
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0: // Index 0 - List of String
                {
                    // Continue
                }
                    break;
                case 1: //  Index 1 -  List of FormatRules
                {
                    paramData = &action.inputFormattedString();
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong number of parameters");
                }
            }
                break;
            case 1: // Level 2
            {
                int level0Position = interpreterData->currentActionParamIndexes.at(0);
                switch(level0Position){
                case 0:
                {
                    action.stringsToCompare().append(config.data->reader->readElementText(QXmlStreamReader::IncludeChildElements));
                    return config.FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>();
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
                }
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        case ActionType::IsLastSavedStat:
        {
            using Action = ConditionalsFactory::Product<ActionType::IsLastSavedStat>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);

        }
            break;
        default:
            break;
        }
    }
        break;
    case Stat::ExecutableAction:
    {
        using Action = Data::ExecutableAction;
        using ActionType = ExecutablesTypes;
        Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
        switch(action->type()){
        case ActionType::Write:
        {
            using Action = ExecutablesFactory::Product<ActionType::Write>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            // Description
            /*
                { Level 1 - Index 0 -  List of FormatRules
                    { Level 2 - Format Rules

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 1
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0: //  Index 1 -  List of FormatRules
                {
                    // Create - expand
                    paramData = &action.inputFormattedString();
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong number of parameters");
                }
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        case ActionType::TclParse:
        {
            using Action = ExecutablesFactory::Product<ActionType::TclParse>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            // Description
            /*
                { Level 1 - Index 0 -  List of FormatRules
                    { Level 2 - Format Rules

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 1
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0: //  Index 1 -  List of FormatRules
                {
                    // Create - expand
                    paramData = &action.inputFormattedString();
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong number of parameters");
                }
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        case ActionType::Error:
        {
            using Action = ExecutablesFactory::Product<ActionType::Error>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            // Description
            /*
                { Level 1 - Index 0 -  List of FormatRules
                    { Level 2 - Format Rules

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 1
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0: //  Index 1 -  List of FormatRules
                {
                    // Create - expand
                    paramData = &action.inputFormattedString();
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong number of parameters");
                }
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        case ActionType::AddPreExpression:
        {
            using Action = ExecutablesFactory::Product<ActionType::AddPreExpression>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            // Description
            /*
                { Level 1 - Index 0 -  List of FormatRules
                    { Level 2 - Format Rules

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 1
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0: //  Index 1 -  List of FormatRules
                {
                    // Create - expand
                    paramData = &action.inputFormattedString();
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong number of parameters");
                }
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        case ActionType::AddUserInteraction:
        {
            using Action = ExecutablesFactory::Product<ActionType::AddUserInteraction>;
            Action& action = *static_cast<Action*>(dmStatWithAction->dataModel);
            // Description
            /*
                { Level 1 - Index 0 -  List of FormatRules
                    { Level 2 - Format Rules

                    }
                }
            */
            switch(dmStatWithAction - interpreterData->dmStats.rbegin()){
            case 0: // Level 1
            {
                switch(interpreterData->currentActionParamIndexes.at(0)){ // Current Position
                case 0: //  Index 1 -  List of FormatRules
                {
                    // Create - expand
                    paramData = &action.inputFormattedString();
                }
                    break;
                default:
                    return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong number of parameters");
                }
            }
                break;
            default:
                return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong parameters structure");
            }
        }
            break;
        default:
            break;
        }
    }
        break;
    default:
        break;
    }

    interpreterData->currentActionParamIndexes.append(0);
    interpreterData->dmStats.append({paramData, Stat::ActionParameter});
    /*QString str = config.data->reader->readElementText(QXmlStreamReader::IncludeChildElements);
    if(str.isEmpty()){
        qDebug() << "ActionParameter cant be empty";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - ActionParameter cant be empty.");
    }

    if(const QString error = interpreterData->checkAndAddActionParameter(THIS_STAT, str);
            not error.isEmpty())
    {
        return config.ERROR_CALL(PRE_ERROR_MSG + error);
    }*/


    return true;
}

template<>template<>template<>
bool FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::FormatRule>(){
    using RuleTypeInfo = FormatParameters::TypeInfo;
    using Rule = Data::FormatRule;
    using RuleType = FormatParametersType;
    using FormattedStringParam = ParametersFactory::Product<Parameters::FormattedString>;
    FormattedStringParam *formattedStringParam = nullptr;

    const QString PRE_ERROR_MSG = "InternalError: FormatRule Token";
    if(interpreterData->dmStats.isEmpty() or
            interpreterData->dmStats.last().stat != Stat::ActionParameter or
            not (formattedStringParam = static_cast<FormattedStringParam*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(PRE_ERROR_MSG);

    if(not (config.data->reader->attributes().hasAttribute("type"))){
        qDebug() << "FormatRule \"type\" attribute doesnt exist";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"type\"");
    }
    QString typeStr = config.data->reader->attributes().value("type").toString().trimmed();
    RuleType type = RuleTypeInfo::fromStr(typeStr);
    if(type == RuleType::None){
        qDebug() << ("FormatRule \"type\" attribute: Unknown Value \"") + typeStr + "\"";
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"type\": Unknown Value \"" + typeStr + "\"");
    }

    Rule ruleBase = FormatParametersFactory::create(type);

    interpreterData->dmStats.append({ruleBase, Stat::FormatRule});

    bool ok;
    QString value;
    QString string;
    switch(type){
    case RuleType::NameItem:
    {
    }
        break;
    case RuleType::ArgumentsFromItem:
    {
        using Rule = FormatParametersFactory::Product<RuleType::ArgumentsFromItem>;
        Rule& rule = *static_cast<Rule*>(ruleBase);

        int index;
        if(not (config.data->reader->attributes().hasAttribute("value"))){
            qDebug() << "FormatRule \"value\" attribute doesnt exist";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"value\"");
        }

        if(static_cast<void>((index = config.data->reader->attributes().value("value").trimmed().toInt(&ok))), not ok){
            qDebug() << "Param \"value\" convertion error \"" + config.data->reader->attributes().value("value").toString() + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"value\" conversion error");
        }
        rule.setIndex(index);
        rule.setSeparator(config.data->reader->readElementText(QXmlStreamReader::IncludeChildElements));
        formattedStringParam->parameters().append(&rule);
        return config.FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>();

    }
        break;
    case RuleType::IndexItem:
    {
        using Rule = FormatParametersFactory::Product<RuleType::IndexItem>;
        Rule& rule = *static_cast<Rule*>(ruleBase);

        int index;
        if(not (config.data->reader->attributes().hasAttribute("value"))){
            qDebug() << "FormatRule \"value\" attribute doesnt exist";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"value\"");
        }
        if(static_cast<void>((index = config.data->reader->attributes().value("value").trimmed().toInt(&ok))), not ok){
            qDebug() << "Param \"value\" convertion error \"" + config.data->reader->attributes().value("value").toString() + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Attribute \"value\" conversion error");
        }
        rule.setIndex(index);
    }
        break;
    case RuleType::FormatItem:
    {
        using Rule = FormatParametersFactory::Product<RuleType::FormatItem>;
        using Format = UserProcedure::Format;
        using Target = UserProcedure::Format::Target;
        Rule& rule = *static_cast<Rule*>(ruleBase);

        if(not (config.data->reader->attributes().hasAttribute("target"))){
            qDebug() << "FormatRule \"target\" attribute doesnt exist";
            return config.ERROR_CALL(PRE_ERROR_MSG + " - No Attribute \"target\"");
        }
        value = config.data->reader->attributes().value("target").trimmed().toString();

        Format::Target formatTarget = Format::fromStr_target(value);
        if(formatTarget == Format::Target::None){
            qDebug() << "Wrong FormatRule argument:" + value + "\"";
            return config.ERROR_CALL(PRE_ERROR_MSG + "Wrong FormatRule argument:" + value + "\"");
        }
        rule.setTarget(formatTarget);
    }
        break;
    case RuleType::TextItem:
    {
        using Rule = FormatParametersFactory::Product<RuleType::TextItem>;
        Rule& rule = *static_cast<Rule*>(ruleBase);

        value = config.data->reader->attributes().value("value").toString();

        rule.setText(value);
    }
        break;
    default:
        break;
    }
    formattedStringParam->parameters().append(ruleBase);

    return true;
}


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_XML_TclCaplParserConfigInterpreter::Config>> FSD_XML_TclCaplParserConfigInterpreter::processingFunctions = {
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::TclConfig>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Settings>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::WriteOnlyFunctions>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Procedure>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::Name>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::DefaultProcedure>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForArgument>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesForUnspecifiedArgument>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnEndOfCall>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnInit>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RulesOnMove>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::RawRule>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ConditionalAction>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ExecutableAction>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::ActionParameter>,
    &FSD_XML_TclCaplParserConfigInterpreter::processingFunction<FSD_XML_TclCaplParserConfigInterpreter::Stat::FormatRule>,
};
