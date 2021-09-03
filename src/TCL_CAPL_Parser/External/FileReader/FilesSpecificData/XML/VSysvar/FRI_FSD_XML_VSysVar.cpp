#include"FRI_FSD_XML_VSysVar.hpp"
#include"CAPLVariable/CAPLVariables/CAPLClass/SysVar/sysvarvar.hpp"

#include<QDir>


template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::initialize(){
    interpreterData = new FSD_XML_CAPLVSysvarInterpreter::Data;
    data = new FileReaderInterpreter_Configuration_Priv<FSD_XML>::Data;
    interpreterData->dmStats.append({&dataModel, Stat::SYSTEM_VARIABLES});
    return true;
}

template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::deinitialize(){
    delete data;
    data = nullptr;
    delete interpreterData;
    interpreterData = nullptr;
    return true;
}


template<>
template<>
const InterpreterStatsConfiguration FSD_XML_CAPLVSysvarInterpreter::statConfig = InterpreterStatsConfiguration::initializeStatConfig(
            QStringList{
                "systemvariables",
                "namespace",
                "variable",
                "valuetable",
                "valuetableentry",
                "struct",
                "structMember"
            }
            );

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>(){
    return ERROR_CALL(Error::ERROR);
}

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>(){
    return ERROR_CALL(Error::ERROR);
}

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>(){
    return true;
}

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>(){

    return true;
}

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>(){
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
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>(){
    if(interpreterData->dmStats.isEmpty())
        return  ERROR_CALL(Error::ERROR);
    CAPLObject * caplObject = nullptr;
    switch(interpreterData->dmStats.last().stat){
    case Stat::NAMESPACE:
    {
        if((caplObject = interpreterData->dmStats.last().dataModel)){
            GetParamRules gpr;
            QString namespaceStr;
            namespaceStr = caplObject->getParam(gpr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME)});
            interpreterData->fullNamespaceStr.chop(namespaceStr.length());
            if(!interpreterData->fullNamespaceStr.isEmpty())
                interpreterData->fullNamespaceStr.chop(2);
        }

    }
        break;
    case Stat::STRUCT:
    {
        qDebug() << interpreterData->dmStats.last().dataModel->toString();
    }
        break;
    default:
        break;
    }
    interpreterData->dmStats.removeLast();
    return true;
}

template<>
QVarLengthArray<ProcessingFunctions_FRI<FSD_XML_CAPLVSysvarInterpreter::Config>, (int)FSD_XML_CAPLVSysvarInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_XML_CAPLVSysvarInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_XML_CAPLVSysvarInterpreter::Config::processingFunctions_FS = {
    &FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>,
    &FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>,
    &FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>,
    &FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>,
    &FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>,
    &FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>
};


template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::SYSTEM_VARIABLES>(){

    // Check Attributes
    return true;
}

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::NAMESPACE>(){
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    // Check Attributes
    QString str = config.data->reader->attributes().value("name").toString();    
    CAPLNamespace* pCAPLNamespace = nullptr;
    if(!str.isEmpty()){
        pCAPLNamespace = new CAPLNamespace();
        SetParamRules spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD};
        pCAPLNamespace->setParam(str, spr);
        if(interpreterData->fullNamespaceStr.isEmpty())
            interpreterData->fullNamespaceStr += str;
        else
            interpreterData->fullNamespaceStr += QString("::") + str;

        // Add to EXISTING parent
        Data::DMStats::iterator parentObj = interpreterData->dmStats.end() - 1;
        bool stop = false;
        do {
            switch(parentObj->stat){
            case Stat::NAMESPACE:
                if(parentObj->dataModel)
            case Stat::SYSTEM_VARIABLES:
                    stop = true;
                break;
            default:
                return config.ERROR_CALL(Error::ERROR);
            }
        } while (!stop && --parentObj >= interpreterData->dmStats.begin());
        if(!stop || !parentObj->dataModel || !parentObj->dataModel->setParam(&pCAPLNamespace, spr = {DB_GET_REAL_PARAM(CAPLNamespace, CAPL_NAMESPACE_OBJECTS), INT_MAX, SET_PARAM_ACTION_ADD}))
            return config.ERROR_CALL(Error::ERROR);
        // ---------------------------------------------
    }
    interpreterData->dmStats.append({pCAPLNamespace,Stat::NAMESPACE});
    return true;
}

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VARIABLE>(){
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    SysVarVarPriv* pVar = nullptr;
    SetParamRules spr;
    // Check Type - > Type -> BitCount -> Signed/UnSigned
    QString str;
    str = config.data->reader->attributes().value("type").toString();
    // Types
    // - Int
    if(str == QString("int")){
        // Signed/Unsigned
        str = config.data->reader->attributes().value("isSigned").toString();
        if(str == QString("false")){
            // Unsigned
            // 32Bit/64bit
            str = config.data->reader->attributes().value("bitcount").toString();
            if(str == QString("32")){
                str = config.data->reader->attributes().value("startValue").toString();
                pVar = new SysVarVarTemplate<CAPLVariableType::DWORD>(static_cast<quint32>(str.toUInt()),false, false);

            }else{
                if(str == QString("64")){
                    str = config.data->reader->attributes().value("startValue").toString();
                    pVar = new SysVarVarTemplate<CAPLVariableType::QWORD>(static_cast<quint64>(str.toULongLong()),false, false);

                }else{
                    //return config.ERROR_CALL(Error::ERROR);
                }
            }            
        }else{
            if(str == QString("true")){
                // Signed
                // 32Bit/64bit
                str = config.data->reader->attributes().value("bitcount").toString();
                if(str.isEmpty() || str == QString("32")){
                    str = config.data->reader->attributes().value("startValue").toString();
                    pVar = new SysVarVarTemplate<CAPLVariableType::LONG>(static_cast<qint32>(str.toInt()),false, false);

                }else{
                    if(str == QString("64")){
                        str = config.data->reader->attributes().value("startValue").toString();
                        pVar = new SysVarVarTemplate<CAPLVariableType::INT64>(static_cast<qint64>(str.toLongLong()),false, false);

                    }else{
                        //return config.ERROR_CALL(Error::ERROR);
                    }
                }
            }else{
                //return config.ERROR_CALL(Error::ERROR);
            }
        }        
    }else{
        if(str == QString("float")){
            // - Float
            str = config.data->reader->attributes().value("startValue").toString();
            pVar = new SysVarVarTemplate<CAPLVariableType::DOUBLE>(static_cast<double>(str.toDouble()),false, false);
        }else{
            if(str == QString("string")){
                // - String
                str = config.data->reader->attributes().value("startValue").toString();
                pVar = new SysVarVarArray(str, CAPLVariableType::CHAR, 0,false, false);
            }else{
                if(str == QString("floatarray")){
                    // - FloatArray
                    quint32 size =  config.data->reader->attributes().value("arrayLength").toUInt();
                    str = config.data->reader->attributes().value("startValue").toString();
                    pVar = new SysVarVarArray(str, CAPLVariableType::DOUBLE, size,false, false);

                }else{
                    if(str == QString("intarray")){
                        // - int Array (ValueTable available) - parameter arrayLength - parameter (init) startValue ex. 2;2;3 -> split with ; -> use loop to init values
                        quint32 size =  config.data->reader->attributes().value("arrayLength").toUInt();
                        str = config.data->reader->attributes().value("startValue").toString();
                        pVar = new SysVarVarArray(str, CAPLVariableType::LONG, size,false, false);

                    }else{
                        if(str == QString("data")){ // Array of bytes - Size based on startValue ex. 02 BA -> split by space -> hexa only
                            quint32 size =  config.data->reader->attributes().value("arrayLength").toUInt();
                            str = config.data->reader->attributes().value("startValue").toString();
                            pVar = new SysVarVarArray(str, CAPLVariableType::BYTE, size,false, false);

                       }else{
                           if(str == QString("struct")){    // Read attribute structDefinition -> Find Def and create Struct variable
                                if(!(str = config.data->reader->attributes().value("structDefinition").toString()).isEmpty())
                                {

                                    pVar = new SysVarVarTemplate<CAPLVariableType::STRUCT>(str,false, false);
                                    /*
                                    QStringList strList = str.split("::", Qt::SkipEmptyParts);
                                    GetParamRules gpr;
                                    CAPLNamespace* finalDataModel = static_cast<CAPLNamespace*>(interpreterData->dmStats.first().dataModel);
                                    int curSLIndex = 0;
                                    while(pVar && curSLIndex < strList.size()){
                                        for(int i = 0, run = true; run && i < finalDataModel->size(); i++){
                                            if(finalDataModel->at(i)->getParam(gpr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME)}) == strList.at(curSLIndex))
                                            {
                                                switch (finalDataModel->at(i)->getType()) {
                                                case CAPL_NAMESPACE:
                                                {
                                                    finalDataModel = static_cast<CAPLNamespace*>(finalDataModel->at(i));
                                                    run = false;
                                                }
                                                    break;
                                                case CAPL_STRUCT_DEFINITION:
                                                {
                                                    if(curSLIndex == strList.size() - 1){
                                                        SetParamRules spr;
                                                        CAPLStruct* caplStruct = static_cast<CAPLStruct*>(finalDataModel->at(i));
                                                        if(pVar->refVariable()->setParam(&caplStruct, spr = {DB_GET_REAL_PARAM(CAPLStructVar, CAPL_STRUCT_VAR_STRUCT_DEF), UINT_MAX, SET_PARAM_ACTION_ADD})){
                                                            break;
                                                        }
                                                    }

                                                }
                                                 Q_FALLTHROUGH();
                                                default:
                                                    SET_PTR_DO(pVar, nullptr);
                                                    run = false;
                                                }
                                                curSLIndex++;
                                            }
                                        }
                                    }
                                    */
                                }
                           }
                        }
                    }
                }
            }
        }
    }

    if(pVar){
        CAPLVariable* property = nullptr;
        str = config.data->reader->attributes().value("name").toString();
        pVar->setParam(str, spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD});
        property = CAPLVariable::createVariableString(str);
        str = "name";
        property->setParam(str, spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD});
        SET_PTR_DO(pVar->_name, property);
        property = CAPLVariable::createVariableString(interpreterData->fullNamespaceStr);
        str = "`namespace";
        property->setParam(str, spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD});
        SET_PTR_DO(pVar->_namespace, property);

        // Add to EXISTING parent
        Data::DMStats::iterator parentObj = interpreterData->dmStats.end() - 1;
        bool stop = false;
        do {
            switch(parentObj->stat){
            case Stat::NAMESPACE:
                if(parentObj->dataModel)
            case Stat::SYSTEM_VARIABLES:
                    stop = true;
                break;
                default:
                    return config.ERROR_CALL(Error::ERROR);
            }
        } while (!stop && --parentObj >= interpreterData->dmStats.begin());
        if(!stop || !parentObj->dataModel || !parentObj->dataModel->setParam(&pVar, spr = {DB_GET_REAL_PARAM(CAPLStruct, CAPL_STRUCT_ELEMENT), INT_MAX, SET_PARAM_ACTION_ADD}))
            return config.ERROR_CALL(Error::ERROR);
        // ---------------------------------------------
        //qDebug() << pVar->toString();
    }
    interpreterData->dmStats.append({pVar,Stat::VARIABLE });
    return true;
}

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VALUE_TABLE>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::VARIABLE)
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->dmStats.append({interpreterData->dmStats.last().dataModel,Stat::VALUE_TABLE});
    return true;
}

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VALUE_TABLE_ENTRY>(){
    if(interpreterData->dmStats.isEmpty() || interpreterData->dmStats.last().stat != Stat::VALUE_TABLE)
        return config.ERROR_CALL(Error::ERROR);
    SysVarVarPriv* varPriv = static_cast<SysVarVarPriv*>(interpreterData->dmStats.last().dataModel);
    QString str = config.data->reader->attributes().value("description").toString();
    switch(varPriv->variableType()){
    case CAPLVariableType::LONG:
    {
        qint32 val = config.data->reader->attributes().value("value").toInt();
        static_cast<SysVarVarTemplate<CAPLVariableType::LONG>*>(varPriv)->valueTable().insert(str, val);
    }
        break;
    case CAPLVariableType::INT64:
    {
        qint64 val = config.data->reader->attributes().value("value").toLongLong();
        static_cast<SysVarVarTemplate<CAPLVariableType::INT64>*>(varPriv)->valueTable().insert(str, val);
    }
        break;
    case CAPLVariableType::DWORD:
    {
        quint32 val = config.data->reader->attributes().value("value").toUInt();
        static_cast<SysVarVarTemplate<CAPLVariableType::DWORD>*>(varPriv)->valueTable().insert(str, val);
    }
        break;
    case CAPLVariableType::QWORD:
    {
        quint64 val = config.data->reader->attributes().value("value").toULongLong();
        static_cast<SysVarVarTemplate<CAPLVariableType::QWORD>*>(varPriv)->valueTable().insert(str, val);
    }
        break;    
    case CAPLVariableType::ARRAY:
    {
        qint32 val = config.data->reader->attributes().value("value").toInt();
        static_cast<SysVarVarTemplate<CAPLVariableType::ARRAY>*>(varPriv)->valueTable().insert(str, val);
    }
        break;
    /*case CAPLVariableType::STRUCT:
    {
        qint32 val = config.data->reader->attributes().value("value").toInt();
        static_cast<SysVarVarTemplate<CAPLVariableType::STRUCT>*>(varPriv)->valueTable().insert(str, val);
    }
        break;*/
    default:
        //qDebug() << "NO VALUE TABLE for " + varPriv->toString();
        return config.ERROR_CALL(Error::ERROR);
    }

    interpreterData->dmStats.append({nullptr,Stat::VALUE_TABLE_ENTRY});
    return true;
}

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::STRUCT>(){
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL(Error::ERROR);

    QString str = config.data->reader->attributes().value("name").toString();
    CAPLStruct* caplStruct = new CAPLStruct(str);

    // Add to EXISTING parent
    SetParamRules spr;
    Data::DMStats::iterator parentObj = interpreterData->dmStats.end() - 1;
    bool stop = false;
    do {
        switch(parentObj->stat){
        case Stat::NAMESPACE:
            if(parentObj->dataModel)
                stop = true;
            break;
        default:
            return config.ERROR_CALL(Error::ERROR);
        }
    } while (!stop && --parentObj >= interpreterData->dmStats.begin());
    if(!stop || !parentObj->dataModel || !parentObj->dataModel->setParam(&caplStruct, spr = {DB_GET_REAL_PARAM(CAPLNamespace, CAPL_NAMESPACE_OBJECTS), INT_MAX, SET_PARAM_ACTION_ADD}))
        return config.ERROR_CALL(Error::ERROR);
    // ---------------------------------------------
    interpreterData->dmStats.append({caplStruct,Stat::STRUCT});
    return true;
}

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::STRUCT_MEMBER>(){
    if(interpreterData->dmStats.isEmpty())
        return config.ERROR_CALL(Error::ERROR);
    SysVarVarPriv* pVar = nullptr;
    SetParamRules spr;
    // Check Type - > Type -> BitCount -> Signed/UnSigned
    QString str;
    // Types
    str = config.data->reader->attributes().value("type").toString();
    // - Int
    if(str == QString("int")){
        // Signed/Unsigned
        str = config.data->reader->attributes().value("isSigned").toString();
        if(str == QString("true")){
            // signed
            // 32Bit/64bit
            str = config.data->reader->attributes().value("bitcount").toString();
            if(str == QString("32")){
                str = config.data->reader->attributes().value("startValue").toString();
                pVar = new SysVarVarTemplate<CAPLVariableType::LONG>(static_cast<qint32>(str.toInt()),false, false);

            }else{
                if(str == QString("64")){
                    str = config.data->reader->attributes().value("startValue").toString();
                    pVar = new SysVarVarTemplate<CAPLVariableType::INT64>(static_cast<qint64>(str.toLongLong()),false, false);

                }else{
                    //return config.ERROR_CALL(Error::ERROR);
                }
            }
        }else{
            if(str == QString("false")){
                // UnSigned
                // 32Bit/64bit
                str = config.data->reader->attributes().value("bitcount").toString();
                if(str.isEmpty() || str == QString("32")){
                    str = config.data->reader->attributes().value("startValue").toString();
                    pVar = new SysVarVarTemplate<CAPLVariableType::DWORD>(static_cast<quint32>(str.toUInt()),false, false);

                }else{
                    if(str == QString("64")){
                        str = config.data->reader->attributes().value("startValue").toString();
                        pVar = new SysVarVarTemplate<CAPLVariableType::QWORD>(static_cast<quint64>(str.toULongLong()),false, false);

                    }else{
                        //return config.ERROR_CALL(Error::ERROR);
                    }
                }
            }else{
                //return config.ERROR_CALL(Error::ERROR);
            }
        }

    }else{
        if(str == QString("float")){
            // - Float
            str = config.data->reader->attributes().value("startValue").toString();
            pVar = new SysVarVarTemplate<CAPLVariableType::DOUBLE>(static_cast<double>(str.toDouble()),false, false);

        }else{
            if(str == QString("string")){
                // - String
                quint32 size =  config.data->reader->attributes().value("bitcount").toInt() / 8/*bits*/;
                str = config.data->reader->attributes().value("startValue").toString();
                pVar = new SysVarVarArray(str, CAPLVariableType::CHAR, size,false, false);

            }else{
                if(str == QString("floatarray")){
                    // - FloatArray
                    quint32 size =  config.data->reader->attributes().value("arrayLength").toInt();
                    str = config.data->reader->attributes().value("startValue").toString();
                    pVar = new SysVarVarArray(str, CAPLVariableType::DOUBLE, size,false, false);

                }else{
                    if(str == QString("intarray")){
                        // - int Array (ValueTable available) - parameter arrayLength - parameter (init) startValue ex. 2;2;3 -> split with ; -> use loop to init values
                        quint32 size =  config.data->reader->attributes().value("arrayLength").toInt();
                        str = config.data->reader->attributes().value("startValue").toString();
                        pVar = new SysVarVarArray(str, CAPLVariableType::LONG, size,false, false);

                    }else{
                        if(str == QString("data")){ // Array of bytes - Size based on startValue ex. 02 BA -> split by space -> hexa only
                            quint32 size =  config.data->reader->attributes().value("arrayLength").toInt()/ 8/*bits*/;
                            str = config.data->reader->attributes().value("startValue").toString();
                            pVar = new SysVarVarArray(str, CAPLVariableType::BYTE, size,false, false);

                       }else{
                           if(str == QString("struct")){    // Read attribute structDefinition -> Find Def and create Struct variable
                               if(!(str = config.data->reader->attributes().value("structDefinition").toString()).isEmpty())
                               {
                                   pVar = new SysVarVarTemplate<CAPLVariableType::STRUCT>(str,false, false);

                               }

                           }
                        }
                    }
                }
            }
        }
    }
    if(pVar){
        CAPLVariable* property = nullptr;
        str = config.data->reader->attributes().value("name").toString();
        pVar->setParam(str, spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD});
        property = CAPLVariable::createVariableString(str);
        property->setParam(str, spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD});
        SET_PTR_DO(pVar->_name, property);


        // Add to EXISTING parent
        Data::DMStats::iterator parentObj = interpreterData->dmStats.end() - 1;
        bool stop = false;
        do {
            switch(parentObj->stat){
            case Stat::STRUCT:
                    stop = true;
                    break;
                default:
                    return config.ERROR_CALL(Error::ERROR);
            }
        } while (!stop && --parentObj >= interpreterData->dmStats.begin());
        if(!stop || !parentObj->dataModel || !parentObj->dataModel->setParam(&pVar, spr = {DB_GET_REAL_PARAM(CAPLStruct, CAPL_STRUCT_ELEMENT), INT_MAX, SET_PARAM_ACTION_ADD}))
            return config.ERROR_CALL(Error::ERROR);
        // ---------------------------------------------
       // qDebug() << pVar->toString();
    }

    interpreterData->dmStats.append({pVar,Stat::STRUCT_MEMBER});
    return true;
}

template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_XML_CAPLVSysvarInterpreter::Config>> FSD_XML_CAPLVSysvarInterpreter::processingFunctions = {
    &FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::SYSTEM_VARIABLES>,
    &FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::NAMESPACE>,
    &FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VARIABLE>,
    &FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VALUE_TABLE>,
    &FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VALUE_TABLE_ENTRY>,
    &FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::STRUCT>,
    &FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::STRUCT_MEMBER>,
};
