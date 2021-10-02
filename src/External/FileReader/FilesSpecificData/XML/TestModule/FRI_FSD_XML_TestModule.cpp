#include"FRI_FSD_XML_TestModule.hpp"

template <>
QString FSD_XML_TestModuleInterpreter::Config::fileFilter(){
    return QString("*.vxt;*.xml");
}

template<>
bool FSD_XML_TestModuleInterpreter::Config::initialize(){
    interpreterData = new FSD_XML_TestModuleInterpreter::Data;
    data = new FileReaderInterpreter_Configuration_Priv<FSD_XML>::Data;
    interpreterData->dmStats.append({&dataModel,Stat::TEST_MODULE});
    return true;
}

template<>
bool FSD_XML_TestModuleInterpreter::Config::deinitialize(){
    delete data;
    data = nullptr;
    delete interpreterData;
    interpreterData = nullptr;
    return true;
}

template<>
template<>
const InterpreterStatsConfiguration FSD_XML_TestModuleInterpreter::statConfig = InterpreterStatsConfiguration::initializeStatConfig(
            QStringList{
                "testmodule",
                "testgroup",
                "capltestcase",
                "info",
                "variants",
                "variant",
                "preparation",
                "initialize",
                "sysvar",
                "description",
                "caplparam",
                "miscinfo",
                "name",
                "completion",
                "engineer",
                "capltestfunction"
            }
            );


template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>(){
    return ERROR_CALL(Error::ERROR);
}

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>(){
    return ERROR_CALL(Error::ERROR);
}

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>(){
    return true;
}

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>(){

    return true;
}

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>(){
    QString tokenName;
    {
        FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
        if(!COM)
            return ERROR_CALL(Error::ERROR);
        NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
        if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
            return ERROR_CALL(Error::ERROR);
        tokenName = static_cast<QXmlStreamReader*>(dataCmd->channelData)->name().toString();
        data->reader = static_cast<QXmlStreamReader*>(dataCmd->channelData);
    }
    InterpreterStatInformation statInfo;
    if(!statConfig.findStatInfo(statInfo, tokenName))
        return ERROR_CALL(Error::ERROR);
    stats.append((int)Parent_FSD::FileSpecificInterpreterStat::SIZE + statInfo.stat);
    return true;
}

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>(){
    if(interpreterData->dmStats.isEmpty())
        return ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.removeLast();
    return true;
}

template<>
QVarLengthArray<ProcessingFunctions_FRI<FSD_XML_TestModuleInterpreter::Config>, (int)FSD_XML_TestModuleInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_XML_TestModuleInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_XML_TestModuleInterpreter::Config::processingFunctions_FS = {
    &FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>,
    &FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>,
    &FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>,
    &FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>,
    &FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>,
    &FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>
};
template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_MODULE>(){
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    // Check Attributes
    QString str = config.data->reader->attributes().value("title").toString();
    if(str.isEmpty()){
        //qDebug() << "\"title\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }
    TestModule* pTestModule = static_cast<DataModel*>(interpreterData->dmStats.last().dataModel);
    SetParamRules spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_TITLE), INT_MAX, SET_PARAM_ACTION_ADD};
    pTestModule->setParam(str, spr);
    str = config.data->reader->attributes().value("ident").toString();
    if(str.isEmpty()){
       // qDebug() << "\"ident\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }
    spr = {DB_GET_REAL_PARAM(TestModule, TEST_MODULE_VERSION), INT_MAX, SET_PARAM_ACTION_ADD};
    pTestModule->setParam(str, spr);
    //interpreterData->dmStats.append({pTestModule, FSD_XML_TestModuleInterpreter::Stat::TEST_MODULE});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_GROUP>(){
    TestObject* testModule = nullptr;
    if(interpreterData->dmStats.isEmpty() || !(interpreterData->dmStats.last().stat == Stat::TEST_MODULE || interpreterData->dmStats.last().stat == Stat::TEST_GROUP) || !(testModule = static_cast<TestModule*>(interpreterData->dmStats.last().dataModel)))
        return config.ERROR_CALL(Error::ERROR);
    // Check Attributes
    QString str = config.data->reader->attributes().value("title").toString();
    if(str.isEmpty()){
        //qDebug() << "\"title\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }
    /*str = attributes.value("ident").toString();
    if(str.isEmpty()){
        //qDebug() << "\"ident\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }*/
    //testGroup.setParam(&str, NUMB_OF_TEST_OBJECT_PARAM + TEST_GROUP_IDENT);
    TestGroup* pTestGroup = nullptr;
    switch (interpreterData->dmStats.last().stat) {
    case Stat::TEST_MODULE:
    case Stat::TEST_GROUP:
        pTestGroup = new TestGroup();
        break;
    default:
        return config.ERROR_CALL(Error::ERROR);
    }
    testModule->append(pTestGroup);
    SetParamRules spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_TITLE), INT_MAX, SET_PARAM_ACTION_ADD};
    pTestGroup->setParam(str, spr);
    interpreterData->dmStats.append({pTestGroup,Stat::TEST_GROUP });

    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_TEST_CASE>(){
    if(interpreterData->dmStats.isEmpty() || !(interpreterData->dmStats.last().stat == Stat::TEST_GROUP || interpreterData->dmStats.last().stat == Stat::TEST_MODULE) || !interpreterData->dmStats.last().dataModel)
        return config.ERROR_CALL(Error::ERROR);
    // Check Attributes
    TestCase* pTestCase = nullptr;
    QString str;
    SetParamRules spr;
    pTestCase = new TestCase();
    str = config.data->reader->attributes().value("name").toString();
    if(str.isEmpty()){
        //qDebug() << "\"name\" attribute dont exist";
        return config.ERROR_CALL(Error::ERROR);
    }
    spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD};
    pTestCase->setParam(str, spr);
    spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_TITLE), INT_MAX, SET_PARAM_ACTION_ADD};
    pTestCase->setParam(str, spr);
    spr = {DB_GET_REAL_PARAM(TestCase, TEST_CASE_IDENT), INT_MAX, SET_PARAM_ACTION_ADD};
    pTestCase->setParam(&str, spr);
    interpreterData->dmStats.last().dataModel->append(pTestCase);
    interpreterData->dmStats.append({pTestCase, Stat::CAPL_TEST_CASE});
    QString title = config.data->reader->attributes().value("title").toString();
    if(title.isEmpty()){
        //qDebug() << "\"title\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }else{
        spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_TITLE), INT_MAX, SET_PARAM_ACTION_ADD};
        pTestCase->setParam(str, spr);
    }
    str = config.data->reader->attributes().value("ident").toString();
    if(str.isEmpty()){
        //qDebug() << "\"ident\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }else{
        spr = {DB_GET_REAL_PARAM(TestCase, TEST_CASE_IDENT), INT_MAX, SET_PARAM_ACTION_ADD};
        pTestCase->setParam(&str, spr);
    }

    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::INFO>(){
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::INFO});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::VARIANTS>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::TEST_MODULE || !interpreterData->dmStats.last().dataModel)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::VARIANTS});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::VARIANT>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::VARIANTS)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::VARIANT});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::PREPARATION>(){
    if(interpreterData->dmStats.isEmpty() || (int)interpreterData->dmStats.last().stat > (int)Stat::CAPL_TEST_CASE)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::PREPARATION});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::INIT>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::PREPARATION)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::INIT});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::SYSVAR>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::INIT)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::SYSVAR});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_DESCRIPTION>(){
    if(interpreterData->dmStats.isEmpty() || (int)interpreterData->dmStats.last().stat > (int)Stat::INFO)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::TEST_DESCRIPTION});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_PARAM>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::CAPL_TEST_CASE || !interpreterData->dmStats.last().dataModel)
        return config.ERROR_CALL(Error::ERROR);
    // Check Attributes
    QString type;
    QString str;
    type = config.data->reader->attributes().value("type").toString();
    if(type.isEmpty()){
       //qDebug() << "\"type\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }
    str = config.data->reader->attributes().value("name").toString();
    if(str.isEmpty()){
        //qDebug() << "\"name\" attribute dont exist";
        //return config.ERROR_CALL(Error::ERROR);
    }
    QString value;
    uint paramType = 0;
    if(str == "TestcaseDescription"){
        paramType = 1;
    }
    if(str == "TestcaseRequirements"){
        paramType = 2;
    }
    if(str == "TestcaseParameters"){
        paramType = 3;
    }
    value = config.data->reader->readElementText();
    QString tempStr;
    SetParamRules spr;
    TestCaseParameter* parameterPtr = new TestCaseParameter();
    static_cast<TestCase*>(interpreterData->dmStats.last().dataModel)->append(parameterPtr);
    switch(paramType){
    case 1:
    {
        // Name of Parameter
        tempStr = "sDescription";
        spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(tempStr, spr)){
            // Error
        }
        // Type of Parameter
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_TYPE), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(str, spr)){
            // Error
        }
        // Value
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_VALUE), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(value, spr)){
            // Error
        }
    }
        break;
    case 2:
    {
        // Name of Parameter
        tempStr = "sReq";
        spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(tempStr, spr)){
            // Error
        }
        // Type of Parameter
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_TYPE), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(str, spr)){
            // Error
        }
        // Value
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_VALUE), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(value, spr)){
            // Error
        }
    }
        break;
    case 3:
    {
        tempStr = "sParams";
        // Name of Parameter
        spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(tempStr, spr)){
            // Error
        }
        // Type of Parameter
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_TYPE), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(str, spr)){
            // Error
        }
        // Value
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_VALUE), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(value, spr)){
            // Error
        }
    }
        break;
    default:
        // Check Attributes
        // Name of Parameter
        spr = {DB_GET_REAL_PARAM(TestObject, TEST_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(str, spr)){
            // Error
        }
        // Type of Parameter
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_TYPE), INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(type, spr)){
            // Error
        }
        // Value
        spr = {DB_GET_REAL_PARAM(TestCaseParameter, TEST_CASE_PARAMETER_VALUE),INT_MAX, SET_PARAM_ACTION_ADD};
        if(!parameterPtr->setParam(value, spr)){
            // Error
        }
        break;
    }
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::MISCINFO>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::TEST_MODULE)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::MISCINFO});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::NAME>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::INFO)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::NAME});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::COMPLETION>(){
    if(interpreterData->dmStats.isEmpty() || (int)interpreterData->dmStats.last().stat > (int)Stat::CAPL_TEST_CASE)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::COMPLETION});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::ENGINEER>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::TEST_MODULE)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::ENGINEER});
    return true;
}

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_TEST_FUNCTION>(){
    TestModule* testModule = nullptr;
    if(interpreterData->dmStats.isEmpty() || !(interpreterData->dmStats.last().stat == Stat::PREPARATION || interpreterData->dmStats.last().stat == Stat::COMPLETION) || interpreterData->dmStats.size() < 2 || interpreterData->dmStats.at(interpreterData->dmStats.size() - 2).stat != Stat::TEST_MODULE || !(testModule = static_cast<TestModule*>(interpreterData->dmStats.at(interpreterData->dmStats.size() - 2).dataModel)))
        return config.ERROR_CALL(Error::ERROR);
    SetParamRules spr = {DB_GET_REAL_PARAM(TestModule, TEST_MODULE_PREPERATION), INT_MAX, SET_PARAM_ACTION_ADD};
    if(!config.data->reader->attributes().hasAttribute("name")){
        //qDebug() << "\"name\" attribute dont exist";
        return config.ERROR_CALL(Error::ERROR);
    }
    QString str = config.data->reader->attributes().value("name").toString();

    if(!testModule->setParam(str, spr))
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({nullptr, Stat::CAPL_TEST_FUNCTION});
    return true;
}


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_XML_TestModuleInterpreter::Config>> FSD_XML_TestModuleInterpreter::processingFunctions = {
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_MODULE>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_GROUP>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_TEST_CASE>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::INFO>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::VARIANTS>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::VARIANT>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::PREPARATION>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::INIT>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::SYSVAR>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_DESCRIPTION>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_PARAM>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::MISCINFO>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::NAME>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::COMPLETION>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::ENGINEER>,
    &FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_TEST_FUNCTION>,

};
