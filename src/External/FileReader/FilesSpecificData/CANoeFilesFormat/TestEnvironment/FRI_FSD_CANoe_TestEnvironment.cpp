#include"FRI_FSD_CANoe_TestEnvironment.hpp"
#include"FileReader/FilesSpecificData/XML/TestModule/FRI_FSD_XML_TestModule.hpp"
#include"CANoe/Configuration/canoefileobjecttestmodule.hpp"
#include"CAPLObject/CAPLScript/caplscript.hpp"


// Test Environment ---------------------------------------------------------------------------------------------

template <>
QString FSD_CANoe_CANoeTestEnvironment::Config::fileFilter(){
    return QString("*.tse");
}



const CANoeFileObjectMapping FSD_CANoe_CANoeTestEnvironment::Data::objectsMapping = {
    QString(),
    // Subobjects: 1 SubObject
    // Level 1 - VTSPersistentRoot ------------------------------------------------------------------
    {
        {
           QString("VTSPersistentRoot"), // Subobjects: 1 SubObject
           { // Level 2 - VTSProgrammedNode
             {    QString("VTSProgrammedNode"), //  No SubObjects,
                  {// Level 3 - VTSWrapper<class_VProgrammedNode>
                   {
                        QString("VTSWrapper<class_VProgrammedNode>"),
                       {    // Level 4 - VXmlTestNode
                            {
                                QString("VXmlTestNode"),
                                {   // Level 5 - VTestNode (Contains list of CAN's)
                                    {
                                       QString("VTestNode"),
                                        {// Level 6 - VProgrammedNode
                                         {
                                            QString("VProgrammedNode"),
                                             {  // No Subobjects

                                             },
                                             {  // Vector VFileName
                                                {
                                                    QString("xml"),    // TestModule Xml
                                                    (int)Stat::TEST_MODULE
                                                },
                                                {
                                                    QString("vxt"),    // TestModule Xml - Extension .vxt
                                                    (int)Stat::TEST_MODULE
                                                }
                                             }
                                         }
                                        } ,
                                         {  // Vector VFileName
                                            {
                                                QString("can"),     // CAPLScript
                                                (int)Stat::CAPL_SCRIPT,
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
bool FSD_CANoe_CANoeTestEnvironment::Config::initialize(){
    interpreterData = new FSD_CANoe_CANoeTestEnvironment::Data;
    data = new FileReaderInterpreter_Configuration_Priv<FSD_CANoe>::Data;
    FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
    if(!COM)
        return ERROR_CALL(Error::ERROR);
    QString filePath;
    NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
    if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL(Error::ERROR);
    filePath = *static_cast<QString*>(dataCmd->channelData);
    dataModel.setFile(filePath);
    interpreterData->dmStats.append(&dataModel);
    return true;
}

template<>
bool FSD_CANoe_CANoeTestEnvironment::Config::deinitialize(){
    delete data;
    data = nullptr;
    if(!interpreterData || interpreterData->dmStats.size() != 1)
       return ERROR_CALL(Error::ERROR);
    delete interpreterData;
    interpreterData = nullptr;
    return true;
}

template<>
template<>
const InterpreterStatsConfiguration FSD_CANoe_CANoeTestEnvironment::statConfig = InterpreterStatsConfiguration::initializeStatConfig(
            QStringList{

            }
            );

template<>template<>template<>
bool FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::STANDARD>(){
    interpreterData->dmStats.append(new CANoeFileObjectStandard(*interpreterData->curObjectMapping));
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::TEST_MODULE>(){
    TestModule *tempTestModule = new TestModule();
    TestModuleFileInterpreterConfig testModuleFileInterpreter(*tempTestModule);
    QString filePath = interpreterData->tempPathHelper;

    if(!(filePath = testModuleFileInterpreter.readFileByFilePath(filePath, interpreterData->blackList)).isEmpty())
   {
        // Check for interpreter errors
        if(testModuleFileInterpreter.isError()){   // if error true
            qDebug() << testModuleFileInterpreter.getErrorStr();
        }else{
           // if Data Model != nullptr, then file has been read
            if(testModuleFileInterpreter.getOnTheBlackList()){
                // BlackList PlaceHoldered
                if(!interpreterData->dmStats.last()->contains(  // Append If Dont contain black listed dm stat
                        interpreterData->dmStats_blackList.at(interpreterData->blackList.indexOf(filePath)))){
                    interpreterData->dmStats.last()->append(interpreterData->dmStats_blackList.at(interpreterData->blackList.indexOf(filePath)));
                    qDebug() << "Test Module From Black List: " << filePath;
                }
                SET_PTR_DO(tempTestModule, nullptr);
            }else{
                //xmlTestModulesByPath.insert(tempTestModule, filePath);
                CANoeFileObjectTestModule * fileObjectTestModule = new CANoeFileObjectTestModule(*tempTestModule);
                interpreterData->dmStats.last()->append(fileObjectTestModule);
                qDebug() << "Test Module Success: " << filePath;
                interpreterData->blackList.append(filePath);
                interpreterData->dmStats_blackList.append(fileObjectTestModule);
                tempTestModule = nullptr;
            }

        }
    }
    SET_PTR_DO(tempTestModule, nullptr);
    return true;
}

template<>template<>template<>
bool FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::CAPL_SCRIPT>(){
    QString filePath = interpreterData->tempPathHelper;
    if(QFileInfo::exists(filePath)){
        if(!interpreterData->blackList.contains(filePath)){
            CAPLScript *caplScript = new CAPLScript(filePath);

            //xmlTestModulesByPath.insert(tempTestModule, filePath);
            CANoeFileObject<CAPLScript> * fileObjectCAPLScript = new CANoeFileObject<CAPLScript>(*caplScript);
            interpreterData->dmStats.last()->append(fileObjectCAPLScript);
            qDebug() << "CAPLScript Success: " << caplScript->absoluteFilePath();
            interpreterData->blackList.append(filePath);
            interpreterData->dmStats_blackList.append(fileObjectCAPLScript);
            caplScript = nullptr;
        }else{  // BlackList
            if(!interpreterData->dmStats.last()->contains(  // Append If Dont contain black listed dm stat
                    interpreterData->dmStats_blackList.at(interpreterData->blackList.indexOf(filePath)))){
                interpreterData->dmStats.last()->append(interpreterData->dmStats_blackList.at(interpreterData->blackList.indexOf(filePath)));
                qDebug() << "CAPL Script From Black List: " << filePath;
            }
        }
    }
    return true;
}

template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_CANoe_CANoeTestEnvironment::Config>> FSD_CANoe_CANoeTestEnvironment::processingFunctions = {
    &FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::STANDARD>,
    &FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::TEST_MODULE>,
    &FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::CAPL_SCRIPT>
};


template<>
template<>
bool FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::BEGIN_OF_OBJECT>(){
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
        if(beginOfObjectParams.size() != 3 || interpreterData->objectMappingTrace.size() + 1 != beginOfObjectParams.at(1).toInt(&numbOk) || !numbOk || beginOfObjectParams.at(0).isEmpty())
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
bool FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::END_OF_OBJECT>(){
    FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
    bool numbOk = false;
    if(!COM)
        return ERROR_CALL(Error::ERROR);
    NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
    if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL(Error::ERROR);
    QStringList beginOfObjectParams = static_cast<QString*>(dataCmd->channelData)->split(QString(" "));
    if(interpreterData->ignoreTil.isEmpty()){
        if(beginOfObjectParams.size() != 3 || interpreterData->objectMappingTrace.size() != beginOfObjectParams.at(2).toInt(&numbOk) || !numbOk || beginOfObjectParams.at(1).isEmpty())
            return ERROR_CALL(Error::ERROR);
        interpreterData->objectMappingTrace.removeLast();
        if(data->falseLevel == -1){
            if(interpreterData->curObjectMapping->name != beginOfObjectParams.at(1))
                return ERROR_CALL(Error::ERROR);
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
bool FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::DATA>(){
    if(interpreterData->ignoreTil.isEmpty()){
    if(data->falseLevel == -1 && !interpreterData->curObjectMapping->VFileNameContol.isEmpty())
    {
        FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
        if(!COM)
            return ERROR_CALL(Error::ERROR);
        NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
        if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
            return ERROR_CALL(Error::ERROR);
        // Find:  <VFileName V9 QL>
        //qDebug() << *static_cast<QString*>(dataCmd->channelData);
        if(static_cast<QString*>(dataCmd->channelData)->contains(QString("<VFileName"))){
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
            if(beginOfObjectParams.size() == 5){
                filePath = (filePath = beginOfObjectParams.at(4)).replace(QString("\""), QString(""));
                filePath = filePath.trimmed();
                // Find suffix
                if(!filePath.isEmpty()){
                    filePath = dataModel.path() + QString("\\") + filePath;
                    QFileInfo fileInfo = QFileInfo(filePath);
                    /*if(!fileInfo.makeAbsolute())
                    return ERROR_CALL(Error::ERROR);*/
                    for(int i = 0 ; i < interpreterData->curObjectMapping->VFileNameContol.size(); i++){
                        if(fileInfo.suffix() == interpreterData->curObjectMapping->VFileNameContol.at(i).fileSuffix){
                            interpreterData->tempPathHelper = filePath;
                            stats.append(interpreterData->curObjectMapping->VFileNameContol.at(i).stat + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
                            break;
                        }
                    }
                    // Out of Loop - no Interpreter
                }
            }
        }
    }
    }
    return true;
}

template<>
QVarLengthArray<ProcessingFunctions_FRI<FSD_CANoe_CANoeTestEnvironment::Config>, (int)FSD_CANoe_CANoeTestEnvironment::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_CANoe_CANoeTestEnvironment::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_CANoe_CANoeTestEnvironment::Config::processingFunctions_FS = {
    &FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::BEGIN_OF_OBJECT>,
    &FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::END_OF_OBJECT>,
    &FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::DATA>
};
