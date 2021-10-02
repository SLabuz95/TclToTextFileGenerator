#include"FRI_FSD_CANoe_Config.hpp"
#include"FileReader/FilesSpecificData/CANoeFilesFormat/TestEnvironment/FRI_FSD_CANoe_TestEnvironment.hpp"
#include"FileReader/FilesSpecificData/XML/VSysvar/FRI_FSD_XML_VSysVar.hpp"
#include"CANoe/Other/canoefileobjectvsysvar.hpp"
#include"GeneralPurposeMacros/memorymenegementmacros.hpp"
#include"CAPLObject/CAPLScript/VSysvar/caplvsysvar.hpp"
#include"CANoe/Configuration/canoenetworksinfo.hpp"
#include"CANoe/Configuration/CANoeNetworkDatabaseMapping.hpp"
#include"CANoe/Configuration/CANoeVSimulationNode.hpp"
#include"CAPLObject/CAPLScript/caplscript.hpp"
#include"CANoe/Configuration/canoefileobjectcanoedbc.hpp"
#include"FileReader/FilesSpecificData/CANoeDBC/CANoe_DBC/FRI_FSD_CANoeDBC_CANoeDBC.hpp"

template <>
QString FSD_CANoe_CANoeConfig::Config::fileFilter(){
    return QString("*.cfg");
}



const CANoeFileObjectMapping FSD_CANoe_CANoeConfig::Data::objectsMapping = {
    QString(),
    // Subobjects: 1 SubObject
    // Level 1 - VGlobalConfiguration ------------------------------------------------------------------
    {
        {
           QString("VGlobalConfiguration"), // Subobjects: 2
           {
             {    // Level 2 - VTestSetupBox
                  QString("VTestSetupBox"), //  No SubObjects,
                  {

                  },
                  { // Vector of FileNameControls
                      {
                          QString("tse"),
                          (int)Stat::TEST_ENVIRONMENT
                      }
                  }
             },
             {  // Level 2 - VGlobalParameters
                QString("VGlobalParameters"),   // Subobjects: 3
                {
                    {   // Level 3 - VSVConfigurationStreamer
                        QString("VSVConfigurationStreamer"), // No SubObjects
                        {

                        },
                        {   // Vector of FileNameControls (*.xml, *.vsysvar)
                            {
                                QString("xml"),
                                (int)Stat::VSYSVAR,
                            },
                            {
                                QString("vsysvar"),
                                (int)Stat::VSYSVAR
                            }
                        }
                    },
                    {   // Level 3 - VDatabaseContainerStreamer
                        QString("VDatabaseContainerStreamer"), // No SubObjects
                        {

                        },
                        {   // Vector of FileNameControls (*.dbc)
                            {
                                QString("dbc"),
                                (int)Stat::DBC,
                            }
                        }
                    },
                    {   // Level 3 - ILConfiguration::VProxyManager
                        QString("ILConfiguration::VProxyManager"),  // SubObjects: 1
                        {
                            {   // Level 4 - ILConfiguration::VDatabase
                                QString("ILConfiguration::VDatabase"),
                                {},  // No Subobjects
                                {}, // No VFiles
                                {}, // No Types
                                (int)FSD_CANoe_CANoeConfig::Stat::VDATABASE
                            }
                        }

                    }
                },
                {
                    // Empty Vector of FileNameControls (VGlobalParameters)
                }
             },
             {  // Level 2 - VGBRealTimeBox
                QString("VGBRealTimeBox"),
                {
                    {   // Level 3 - VDOLocalInfoStruct
                        QString("VDOLocalInfoStruct"),
                        {
                            {   // Level 4 - VDAOBus
                                QString("VDAOBus"),
                                {
                                    {   // Level 5 - VDAOGBFunctionBlock
                                        QString("VDAOGBFunctionBlock"),
                                        {
                                            {   // Level 6 - VSimulationNode
                                                QString("VSimulationNode"),
                                                {
                                                    {   // Level 7 -  VProgrammedNode
                                                        QString("VProgrammedNode"),
                                                        {

                                                        },
                                                        {   // Vector of CAN Files
                                                            {
                                                                QString("can"),
                                                                (int)Stat::CAPL_NODE
                                                            }
                                                        },
                                                        {}, // No Types
                                                        (int)FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE

                                                    }
                                                }
                                            }
                                        }

                                    }
                                }

                            }
                        }

                    }
                }
             }

           }
       }
    }
};


template<>
bool FSD_CANoe_CANoeConfig::Config::initialize(){
    interpreterData = new FSD_CANoe_CANoeConfig::Data;
    data = new FileReaderInterpreter_Configuration_Priv<FSD_CANoe>::Data;
    FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
    if(!COM)
        return ERROR_CALL(Error::ERROR);
    NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
    if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL(Error::ERROR);
    QString filePath = *static_cast<QString*>(dataCmd->channelData);
    dataModel.setFile(filePath);
    interpreterData->dmStats.append(&dataModel);
    return true;
}

template<>
bool FSD_CANoe_CANoeConfig::Config::deinitialize(){
    delete data;
    data = nullptr;
    if(!interpreterData || interpreterData->dmStats.size() != 1)
        return ERROR_CALL(Error::ERROR);
    delete interpreterData;
    interpreterData = nullptr;    
    return true;
}


template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::STANDARD>(){
    interpreterData->dmStats.append(new CANoeFileObjectStandard(*interpreterData->curObjectMapping));
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::TEST_ENVIRONMENT>(){
    CANoeTestEnvironment* testEnvironment = new CANoeTestEnvironment(FSD_CANoe_CANoeTestEnvironment::Data::objectsMapping);
    FSD_CANoe_CANoeTestEnvironment::Config testEnvironmentFileInterpreter(*testEnvironment);
    QString filePath = interpreterData->tempPathHelper;
    QStringList _PH_BlackList;
    if(!(filePath = testEnvironmentFileInterpreter.readFileByFilePath(filePath, _PH_BlackList)).isEmpty())
   {
        // Check for interpreter errors
        if(testEnvironmentFileInterpreter.isError()){   // if error true
            qDebug() << testEnvironmentFileInterpreter.getErrorStr();
            SET_PTR_DO(testEnvironment, nullptr);
            return config.ERROR_CALL(Error::ERROR);
        }else{
           // if Data Model != nullptr, then file has been read
            if(testEnvironmentFileInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                interpreterData->dmStats.last()->append(testEnvironment);
                qDebug() << "Test Environment Success: " << testEnvironment->absoluteFilePath();
                testEnvironment = nullptr;

            }

        }
    }
    SET_PTR_DO(testEnvironment, nullptr);
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSYSVAR>(){
    QString filePath = interpreterData->tempPathHelper;
    CAPLVSysvar *tempVSysVar = new CAPLVSysvar(filePath);
    CAPLVSysVarFileInterpreterConfig VSysVarFileInterpreter(*tempVSysVar);
    if(!(filePath = VSysVarFileInterpreter.readFileByFilePath(filePath, interpreterData->blackList)).isEmpty())
   {
        // Check for interpreter errors
        if(VSysVarFileInterpreter.isError()){   // if error true
            qDebug() << VSysVarFileInterpreter.getErrorStr();
        }else{
           // if Data Model != nullptr, then file has been read
            if(VSysVarFileInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                CANoeFileObjectVSysVar * fileObjectVSysVar = new CANoeFileObjectVSysVar(*tempVSysVar);
                interpreterData->dmStats.last()->append(fileObjectVSysVar);
                qDebug() << "VSysvar Success: " << filePath;
                interpreterData->blackList.append(filePath);
                tempVSysVar = nullptr;
            }

        }
    }
    SET_PTR_DO(tempVSysVar, nullptr);
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::DBC>(){
    QString filePath = interpreterData->tempPathHelper;
    DBCNetwork *tempDBCNetwork = new DBCNetwork(filePath);
    FSD_CANoeDBC_CANoeDBC::Config DBCFileInterpreter(*tempDBCNetwork);
    if(!(filePath = DBCFileInterpreter.readFileByFilePath(filePath, interpreterData->blackList)).isEmpty())
   {
        // Check for interpreter errors
        if(DBCFileInterpreter.isError()){   // if error true
            qDebug() << DBCFileInterpreter.getErrorStr();
        }else{
           // if Data Model != nullptr, then file has been read
            if(DBCFileInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                CANoeFileObjectDBC * fileObjectVSysVar = new CANoeFileObjectDBC(*tempDBCNetwork);
                interpreterData->dmStats.last()->append(fileObjectVSysVar);
                qDebug() << "DBC Success: " << filePath;
                interpreterData->blackList.append(filePath);
                tempDBCNetwork = nullptr;
            }
        }
    }
    SET_PTR_DO(tempDBCNetwork, nullptr);
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::CAPL_NODE>(){
    qDebug() << "CAPL_NODE START";
    QString filePath = interpreterData->tempPathHelper;
    if(!interpreterData->blackList.contains(filePath)){
        CAPLScript *caplScript = new CAPLScript(filePath);
        static_cast<CANoeVSimulationNode*>(interpreterData->dmStats.last())->script = caplScript;
        qDebug() << "VSimulationNode CAPLScript Success: " << caplScript->absoluteFilePath();
        interpreterData->blackList.append(filePath);
        caplScript = nullptr;
        // Manual Control
        interpreterData->readControl = Data::ManualControlStat::IGNORE_LINES;
        interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
        interpreterData->ignoreLines = 2;
        interpreterData->statForManualControl = Stat::VSIMULATION_NODE_DB_NODE_NAME_READ;
    }
    qDebug() << "CAPL_NODE END";
    return true;
}


template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::EOF_MBSSDATA_FOR_NETWORKS_READ>(){
    qDebug() << "EOF_MBSSDATA_FOR_NETWORKS_READ START";
    interpreterData->readControl = Data::ManualControlStat::READ_LINE;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->statForManualControl = Stat::NUMB_OF_NETWORKS_READ;
    qDebug() << "EOF_MBSSDATA_FOR_NETWORKS_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NUMB_OF_NETWORKS_READ>(){
    qDebug() << "NUMB_OF_NETWORKS_READ START";
    CANoeNetworksInfo *networksInfo = new CANoeNetworksInfo(*interpreterData->curObjectMapping);
    bool ok = false;
    int size = interpreterData->tempPathHelper.toInt(&ok);
    if(!ok)
        return config.ERROR_CALL(Error::ERROR);
    networksInfo->networksInfo.resize(size);
    interpreterData->dmStats.append(networksInfo);
    interpreterData->curNetworkInfo = 0;
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::READ_LINE;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->statForManualControl = Stat::NETWORK_NAME_READ;
    qDebug() << "NUMB_OF_NETWORKS_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_NAME_READ>(){
    qDebug() << "NETWORK_NAME_READ START";
    if(interpreterData->tempPathHelper.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    static_cast<CANoeNetworksInfo*>(interpreterData->dmStats.last())->networksInfo[interpreterData->curNetworkInfo].name = interpreterData->tempPathHelper;
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::READ_LINE;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->statForManualControl = Stat::NETWORK_TYPE_READ;
    qDebug() << "NETWORK_NAME_READ END";
    return true;

}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_TYPE_READ>(){
    qDebug() << "NETWORK_TYPE_READ START";
    if(interpreterData->tempPathHelper.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    bool ok = false;
    NetworkType type = static_cast<NetworkType>(interpreterData->tempPathHelper.toInt(&ok));
    if(!ok)
        return config.ERROR_CALL(Error::ERROR);
    switch(type){
    case NetworkType::CAN:
    case NetworkType::ETHERNET:
        break;
    default:
        return config.ERROR_CALL(Error::ERROR);
    }
    static_cast<CANoeNetworksInfo*>(interpreterData->dmStats.last())->networksInfo[interpreterData->curNetworkInfo].type = type;
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::IGNORE_LINES;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->ignoreLines = 1;
    interpreterData->statForManualControl = Stat::NETWORK_CHANNEL_READ;
    qDebug() << "NETWORK_TYPE_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_CHANNEL_READ>(){
    qDebug() << "NETWORK_CHANNEL_READ START";
    if(interpreterData->tempPathHelper.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    bool ok = false;
    int channel = interpreterData->tempPathHelper.toInt(&ok);
    if(!ok)
        return config.ERROR_CALL(Error::ERROR);
    static_cast<CANoeNetworksInfo*>(interpreterData->dmStats.last())->networksInfo[interpreterData->curNetworkInfo].channel = channel;
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::WAIT_FOR_STRING;
    interpreterData->statForManualControl = Stat::EOF_BUS_READ;
    interpreterData->manualControl_string = QString("EOF_BUS");
    interpreterData->readControl_ignoreIfBeginOrEndOfObject = true;
    qDebug() << "NETWORK_CHANNEL_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::EOF_BUS_READ>(){
    qDebug() << "EOF_BUS_READ START";
    if(++interpreterData->curNetworkInfo < static_cast<CANoeNetworksInfo*>(interpreterData->dmStats.last())->networksInfo.size()){
        // Manual Control
        interpreterData->readControl = Data::ManualControlStat::READ_LINE;
        interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
        interpreterData->statForManualControl = Stat::NETWORK_NAME_READ;
    }else{
        static_cast<CANoeConfig*>(interpreterData->dmStats.first())->environment().networksInfo = static_cast<CANoeNetworksInfo*>(interpreterData->dmStats.takeLast());
    }
    qDebug() << "EOF_BUS_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE>(){
    qDebug() << "VDATABASE START";
    CANoeNetworksDatabaseMapping* networkDbMapping = new CANoeNetworksDatabaseMapping(*interpreterData->curObjectMapping);
    interpreterData->dmStats.append(networkDbMapping);
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::IGNORE_LINES;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->ignoreLines = 1;
    interpreterData->statForManualControl = Stat::VDATABASE_DATABASE_NAME_READ;
    qDebug() << "VDATABASE END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE_DATABASE_NAME_READ>(){
    qDebug() << "VDATABASE_DATABASE_NAME_READ START";
    if(interpreterData->tempPathHelper.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->lastVDataBase = interpreterData->tempPathHelper;
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::IGNORE_LINES;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->ignoreLines = 2;
    interpreterData->statForManualControl = Stat::VDATABASE_NETWORK_NAME_READ;
    qDebug() << "VDATABASE_DATABASE_NAME_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE_NETWORK_NAME_READ>(){
    qDebug() << "VDATABASE_NETWORK_NAME_READ START";
    if(interpreterData->tempPathHelper.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    static_cast<CANoeNetworksDatabaseMapping*>(interpreterData->dmStats.last())->networkDataBaseMap.insert(interpreterData->tempPathHelper,interpreterData->lastVDataBase);
    qDebug() << "VDATABASE_NETWORK_NAME_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE>(){
    qDebug() << "VSIMULATION_NODE START";
    CANoeVSimulationNode* simulationNode = new CANoeVSimulationNode(*interpreterData->curObjectMapping);
    interpreterData->dmStats.append(simulationNode);
    qDebug() << "VSIMULATION_NODE END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_DB_NODE_NAME_READ>(){
    qDebug() << "VSIMULATION_NODE_DB_NODE_NAME_READ START";
    if(interpreterData->tempPathHelper.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    if(!(interpreterData->tempPathHelper.contains(".") || interpreterData->tempPathHelper == "<< default >>")){
        static_cast<CANoeVSimulationNode*>(interpreterData->dmStats.last())->nodeName = interpreterData->tempPathHelper;
    }
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::READ_LINE;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->statForManualControl = Stat::VSIMULATION_NODE_DB_NAME_READ;
    qDebug() << "VSIMULATION_NODE_DB_NODE_NAME_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_DB_NAME_READ>(){
    qDebug() << "VSIMULATION_NODE_DB_NAME_READ START";
    static_cast<CANoeVSimulationNode*>(interpreterData->dmStats.last())->dbName = interpreterData->tempPathHelper;
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::WAIT_FOR_STRING;
    interpreterData->manualControl_string = "EOF_MBSSDATA";
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->statForManualControl = Stat::VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES;
    qDebug() << "VSIMULATION_NODE_DB_NAME_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES>(){
    qDebug() << "VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES START";
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::READ_LINE;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->statForManualControl = Stat::VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ;
    qDebug() << "VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ>(){
    qDebug() << "VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ START";
    if(interpreterData->tempPathHelper.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    bool ok = false;
    int numbOfNetworks = interpreterData->tempPathHelper.toInt(&ok);
    if(!ok)
        return config.ERROR_CALL(Error::ERROR);
    static_cast<CANoeVSimulationNode*>(interpreterData->dmStats.last())->listOfNetworksIndexes.resize(numbOfNetworks);
    interpreterData->curNetworkInfo = 0;
    // Manual Control
    interpreterData->readControl = Data::ManualControlStat::READ_LINE;
    interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
    interpreterData->statForManualControl = Stat::VSIMULATION_NODE_NETWORK_INDEX_READ;
    qDebug() << "VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ END";
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_NETWORK_INDEX_READ>(){
    qDebug() << "VSIMULATION_NODE_NETWORK_INDEX_READ START";
    if(interpreterData->curNetworkInfo <  static_cast<CANoeVSimulationNode*>(interpreterData->dmStats.last())->listOfNetworksIndexes.size()){
        QStringList strList = interpreterData->tempPathHelper.split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
        if(strList.isEmpty())
            return config.ERROR_CALL(Error::ERROR);
        bool ok = false;
        int networksIndex = strList.first().toInt(&ok);
        if(!ok)
            return config.ERROR_CALL(Error::ERROR);
        static_cast<CANoeVSimulationNode*>(interpreterData->dmStats.last())->listOfNetworksIndexes[interpreterData->curNetworkInfo] = networksIndex;
        interpreterData->curNetworkInfo++;
        // Manual Control
        interpreterData->readControl = Data::ManualControlStat::READ_LINE;
        interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
        interpreterData->statForManualControl = Stat::VSIMULATION_NODE_NETWORK_INDEX_READ;
    }
    qDebug() << "VSIMULATION_NODE_NETWORK_INDEX_READ END";
    return true;
}

template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_CANoe_CANoeConfig::Config>> FSD_CANoe_CANoeConfig::processingFunctions = {
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::STANDARD>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::TEST_ENVIRONMENT>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSYSVAR>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::DBC>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::CAPL_NODE>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::EOF_MBSSDATA_FOR_NETWORKS_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NUMB_OF_NETWORKS_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_NAME_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_TYPE_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_CHANNEL_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::EOF_BUS_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE_DATABASE_NAME_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE_NETWORK_NAME_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_DB_NODE_NAME_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_DB_NAME_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ>,
    &FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_NETWORK_INDEX_READ>,
};


template<>
template<>
bool FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::BEGIN_OF_OBJECT>(){
    if(interpreterData->ignoreTil.isEmpty()){
        FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
        bool numbOk = false;
        if(!COM)
            return ERROR_CALL(Error::ERROR);
        NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
        if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
            return ERROR_CALL(Error::ERROR);
        QStringList beginOfObjectParams = static_cast<QString*>(dataCmd->channelData)->split(QString(" "));
        if(beginOfObjectParams.size() == 2){
            if((interpreterData->ignoreTil = beginOfObjectParams.at(1)).isEmpty())
                return ERROR_CALL(Error::ERROR);
            return true;
        }
        if(beginOfObjectParams.size() != 3 || beginOfObjectParams.at(0).isEmpty())
            return ERROR_CALL(Error::ERROR);
        if(interpreterData->readControl != InterpreterData::ManualControlStat::AUTOMATIC){
            if(interpreterData->readControl_setErrorIfBeginOrEndOfObject)
               return ERROR_CALL(Error::ERROR);
            if(interpreterData->readControl_ignoreIfBeginOrEndOfObject){
                if((interpreterData->ignoreTil = beginOfObjectParams.at(0)).isEmpty())
                    return ERROR_CALL(Error::ERROR);
                return true;
            }
        }
        if(interpreterData->objectMappingTrace.size() + 1 != beginOfObjectParams.at(1).toInt(&numbOk) || !numbOk )
            return ERROR_CALL(Error::ERROR);
        if(config.data->falseLevel == -1){
            QString newObjectName = beginOfObjectParams.at(0);
            int i = 0;
            int size = interpreterData->curObjectMapping->objects.size();
            for(; i < size; i++){
                if(newObjectName == interpreterData->curObjectMapping->objects.at(i).name)
                {
                    interpreterData->curObjectMapping = &interpreterData->curObjectMapping->objects.at(i);
                    stats.append(interpreterData->curObjectMapping->stat + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
                    interpreterData->objectMappingTrace.append(i);
                    break;
                }
            }
            if(i == size){
                data->falseLevel = interpreterData->objectMappingTrace.size();
                interpreterData->objectMappingTrace.append(-1);
            }
        }else{
            interpreterData->objectMappingTrace.append(-1);
        }
    }
    return true;
}

template<>
template<>
bool FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::END_OF_OBJECT>(){
    FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
    bool numbOk = false;
    if(!COM)
        return ERROR_CALL(Error::ERROR);
    NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
    if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL(Error::ERROR);
    QStringList beginOfObjectParams = static_cast<QString*>(dataCmd->channelData)->split(QString(" "));
    if(interpreterData->ignoreTil.isEmpty()){
        if((interpreterData->readControl != InterpreterData::ManualControlStat::AUTOMATIC && interpreterData->readControl_setErrorIfBeginOrEndOfObject) || beginOfObjectParams.size() != 3 || interpreterData->objectMappingTrace.size() != beginOfObjectParams.at(2).toInt(&numbOk) || !numbOk || beginOfObjectParams.at(1).isEmpty())
            return ERROR_CALL(Error::ERROR);
        interpreterData->objectMappingTrace.removeLast();
        if(data->falseLevel == -1){
            if(interpreterData->curObjectMapping->name != beginOfObjectParams.at(1))
                return ERROR_CALL(Error::ERROR);
            if(interpreterData->curObjectMapping->name == "VDOLocalInfoStruct"){
                interpreterData->readControl = InterpreterData::ManualControlStat::WAIT_FOR_STRING;
                interpreterData->manualControl_string = QString("EOF_MBSSDATA");
                interpreterData->statForManualControl = FSD_CANoe_CANoeConfig::Stat::EOF_MBSSDATA_FOR_NETWORKS_READ;
                interpreterData->readControl_setErrorIfBeginOrEndOfObject = true;
            }
            interpreterData->curObjectMapping = &interpreterData->objectsMapping;
            for(int i = 0; i < interpreterData->objectMappingTrace.size(); i++)
                interpreterData->curObjectMapping = &interpreterData->curObjectMapping->objects.at(interpreterData->objectMappingTrace.at(i));
            DataModel::DataModelBase* prelast = interpreterData->dmStats.takeLast();
            interpreterData->dmStats.last()->append(prelast);
        }else{
            if(interpreterData->objectMappingTrace.size() == data->falseLevel)
                data->falseLevel = -1;
        }
    }else{
        if(interpreterData->ignoreTil == beginOfObjectParams.at(1))
            interpreterData->ignoreTil = QString();
    }
    return true;
}

template<>
template<>
bool FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::DATA>(){
    if(interpreterData->ignoreTil.isEmpty()){
    if(data->falseLevel == -1)
    {
        FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
        if(!COM)
            return ERROR_CALL(Error::ERROR);
        NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
        if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
            return ERROR_CALL(Error::ERROR);
        // Find:  <VFileName V9 QL>
        //qDebug() << *static_cast<QString*>(dataCmd->channelData);
        interpreterData->tempPathHelper = *static_cast<QString*>(dataCmd->channelData);
        switch (interpreterData->readControl) {
        case InterpreterData::ManualControlStat::AUTOMATIC:
        {
            if(!interpreterData->curObjectMapping->VFileNameContol.isEmpty() && static_cast<QString*>(dataCmd->channelData)->contains(QString("<VFileName"))){
                QStringList beginOfObjectParams = static_cast<QString*>(dataCmd->channelData)->split(QString(" "));
                // Find and add string with doubleQuotes
                int firstQuoteString = -1;
                for(int i = 3; i < beginOfObjectParams.size(); i++){
                    int countResult = 0;
                    if((countResult = beginOfObjectParams.at(i).count(QChar('\"'))) == 2){
                        if(firstQuoteString != -1){
                            return ERROR_CALL(Error::ERROR);
                        }
                        break;
                    }else{
                        if(countResult == 1){
                            if(firstQuoteString != -1){
                                if(i != beginOfObjectParams.size() - 1)
                                    return ERROR_CALL(Error::ERROR);
                                break;
                            }else{
                                firstQuoteString = i;
                            }
                        }else{
                            if(countResult > 1)
                                return ERROR_CALL(Error::ERROR);
                        }
                    }
                }
                if(firstQuoteString != -1){
                    QString tempStr;
                    QStringList tempList = beginOfObjectParams.mid(firstQuoteString);
                    tempStr = tempList.join(QString(" "));
                    for(int i = firstQuoteString; i < beginOfObjectParams.size(); )
                        beginOfObjectParams.removeAt(i);
                    beginOfObjectParams.append(tempStr);
                }
                // Check FileSuffixes
                QString filePath;
                if(beginOfObjectParams.size() == 5)
                {
                    filePath = (filePath = beginOfObjectParams.at(4)).replace(QString("\""), QString(""));
                    filePath = filePath.trimmed();
                    // Find suffix
                    if(!filePath.isEmpty()){
                        filePath = dataModel.path() + QString("\\") + filePath;
                        QFileInfo fileInfo = QFileInfo(filePath);
                        for(int i = 0 ; i < interpreterData->curObjectMapping->VFileNameContol.size(); i++){
                            if(interpreterData->curObjectMapping->VFileNameContol.at(i).fileSuffix.compare(fileInfo.suffix(), Qt::CaseInsensitive) == 0){
                                interpreterData->tempPathHelper = filePath;
                                stats.append(interpreterData->curObjectMapping->VFileNameContol.at(i).stat + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
                                break;
                            }
                        }
                        // Out of loop - No File Interpreter
                    }
                }
            }
        }
            break;
        case InterpreterData::ManualControlStat::IGNORE_LINES:
        {
            if(interpreterData->ignoreLines != 0){
                interpreterData->ignoreLines--;
                break;
            }
        }
        case  InterpreterData::ManualControlStat::READ_LINE:
        {
            interpreterData->readControl = InterpreterData::ManualControlStat::AUTOMATIC;
            interpreterData->readControl_setErrorIfBeginOrEndOfObject = false;
            interpreterData->readControl_ignoreIfBeginOrEndOfObject = false;
            stats.append((int)interpreterData->statForManualControl + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
        }
            break;
        case InterpreterData::ManualControlStat::WAIT_FOR_STRING:
        {
            if(static_cast<QString*>(dataCmd->channelData) == interpreterData->manualControl_string){
                interpreterData->readControl = InterpreterData::ManualControlStat::AUTOMATIC;
                interpreterData->readControl_setErrorIfBeginOrEndOfObject = false;
                interpreterData->readControl_ignoreIfBeginOrEndOfObject = false;
                stats.append((int)interpreterData->statForManualControl + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
            }
        }
            break;
        default:
            break;
        }

    }
    }
    return true;
}

template<>
QVarLengthArray<ProcessingFunctions_FRI<FSD_CANoe_CANoeConfig::Config>, (int)FSD_CANoe_CANoeConfig::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_CANoe_CANoeConfig::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_CANoe_CANoeConfig::Config::processingFunctions_FS = {
    &FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::BEGIN_OF_OBJECT>,
    &FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::END_OF_OBJECT>,
    &FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::DATA>
};
