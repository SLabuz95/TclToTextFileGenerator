#include"FRI_FSD_CANoeDBC_CANoeDBC.hpp"
//#include"FileReader/FilesSpecificData/CANoeFilesFormat/TestEnvironment/FRI_FSD_CANoe_TestEnvironment.hpp"
//#include"FileReader/FilesSpecificData/XML/VSysvar/FRI_FSD_XML_VSysVar.hpp"
#include"CANoe/Other/canoefileobjectvsysvar.hpp"
#include"GeneralPurposeMacros/memorymenegementmacros.hpp"
#include"CAPLObject/CAPLScript/VSysvar/caplvsysvar.hpp"
#include"CANoe/Configuration/canoenetworksinfo.hpp"
#include"CANoe/Configuration/CANoeNetworkDatabaseMapping.hpp"
#include"CANoe/Configuration/CANoeVSimulationNode.hpp"
#include"CAPLObject/CAPLScript/caplscript.hpp"
#include"CAPLVariable/CAPLVariables/CAPLClass/DbMsg/dbmsg.hpp"
#include"CAPLVariable/CAPLVariables/CAPLClass/DbSig/dbsig.hpp"
#include"CAPLVariable/CAPLVariables/CAPLArray/caplarray.hpp"
#include"CAPLVariable/CAPLVariables/CAPLClass/DbSig/dbsigvar.hpp"
#include"RegExpCore/regexprcore.hpp"
#include"CAPLVariable/CAPLVariables/CAPLClass/DbMsg/dbmsgvar.hpp"

template <>
QString FSD_CANoeDBC_CANoeDBC::Config::fileFilter(){
    return QString();
}

const QVector<FSD_CANoeDBC_CANoeDBC::Data::TagMapping> FSD_CANoeDBC_CANoeDBC::Data::tagMapping
{
    {
        QString("BU_:"),
        Stat::NODES_LIST
    },
    {
        QString("BO_"),
        Stat::DBMSG
    },
    {
        QString("SG_"),
        Stat::SIGNAL_
    },
    {
        QString("BO_TX_BU_"),
        Stat::TX_MESSAGE_NODES_LIST
    },
    {
        QString("EV_"),
        Stat::ENVIRONMENT_VARIABLE
    }
};

template<>
bool FSD_CANoeDBC_CANoeDBC::Config::initialize(){
    interpreterData = new FSD_CANoeDBC_CANoeDBC::Data;
    data = new FileReaderInterpreter_Configuration_Priv<FSD_CANoeDBC>::Data;
    FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
    if(!COM)
        return ERROR_CALL(Error::ERROR);
    NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
    if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL(Error::ERROR);
    QString filePath = *static_cast<QString*>(dataCmd->channelData);
    dataModel.setFile(filePath);
    interpreterData->dbNetwork = &dataModel;
    return true;
}

template<>
bool FSD_CANoeDBC_CANoeDBC::Config::deinitialize(){
    delete data;
    data = nullptr;    
    delete interpreterData;
    interpreterData = nullptr;
    return true;
}

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::NONE>(){
    return true;
}

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::NODES_LIST>(){
    if(interpreterData->dbNetwork->nodes.isEmpty())
        qDebug() << QString("CANoeDBC - Nodes:");
    for(int i = 0; i < interpreterData->lineData.size(); i++)
        qDebug() << interpreterData->lineData.at(i);
    interpreterData->dbNetwork->nodes += interpreterData->lineData;
    return true;
}

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::DBMSG>(){
    enum class DBMSGData{
        ID,
        NAME,
        DLC,
        TRANSMITTER,
        _COUNT
    };
    DbMsgVar* dbMsgVar = static_cast<DbMsgVar*>(DbMsg::createVariable_priv(false, false));
    if(!dbMsgVar){
        return config.ERROR_CALL(Error::ERROR);
    }
    int transmitterIndex = -1;
    SetParamRules spr;
    bool ok = false;
    // NAME
    {
        QString dbMsgName = interpreterData->lineData.at(static_cast<int>(DBMSGData::NAME));
        if(!dbMsgName.isEmpty())    dbMsgName.chop(1);  // Remove ":"
        if(dbMsgName == "VECTOR__INDEPENDENT_SIG_MSG"){ // Ignore Msg
            SET_PTR_DO(dbMsgVar, nullptr);
            interpreterData->lastDbMsg = nullptr;
            return true;
        }
        if(!CAPLVariable::checkSymbolicName(dbMsgName)){
            qDebug() << "Wrong DBMsg Name: " + dbMsgName;
            SET_PTR_DO(dbMsgVar, nullptr);
            return config.ERROR_CALL(Error::ERROR);
        }
        if(!dbMsgVar->setParam(dbMsgName, spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD})){
            SET_PTR_DO(dbMsgVar, nullptr);
            return config.ERROR_CALL(Error::ERROR);
        }
    }
    // ID
    {
        quint32 idValue = interpreterData->lineData.at(static_cast<int>(DBMSGData::ID)).toUInt(&ok);
        if(!ok || !dbMsgVar->at(static_cast<int>(DbMsg::Property::ID))->setParam(&idValue, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
            SET_PTR_DO(dbMsgVar, nullptr);
            return config.ERROR_CALL(Error::ERROR);
        }
    }
    // DLC
    {
        quint8 dlcValue = interpreterData->lineData.at(static_cast<int>(DBMSGData::DLC)).toUShort(&ok);
        if(!ok || !dbMsgVar->at(static_cast<int>(DbMsg::Property::DLC))->setParam(&dlcValue, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
            SET_PTR_DO(dbMsgVar, nullptr);
            return config.ERROR_CALL(Error::ERROR);
        }
    }
    // TRANSMITTER
    {
        QString transmitter = interpreterData->lineData.at(static_cast<int>(DBMSGData::TRANSMITTER));
        if(transmitter != "Vector__XXX"){
            if(!static_cast<CAPLArray*>(dbMsgVar->at(static_cast<int>(DbMsg::Property::TRANSMITTER)))->copyStringData(transmitter)){
                SET_PTR_DO(dbMsgVar, nullptr);
                return config.ERROR_CALL(Error::ERROR);
            }
            transmitterIndex = interpreterData->dbNetwork->nodes.indexOf(transmitter);
        }

    }
    qDebug() << dbMsgVar->toString();
    interpreterData->lastDbMsg = dbMsgVar;
    interpreterData->dbNetwork->dbMsgs.append(dbMsgVar);
    if(transmitterIndex != -1)
        interpreterData->dbNetwork->nodeIndexToDbMsgIndexMap.insert(interpreterData->dbNetwork->dbMsgs.size() - 1, transmitterIndex);
    return true;
}

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::SIGNAL_>(){
    enum class DBSigData{
        NAME,
        MULTIPLEX_INFO, // OPTIONAL
        SEPARATOR_WITH_MULTIPLEX_INFO,  // :
        BITSTART__LENGTH__BYTE_ORDER__SIGNEDNESS = NAME + 1, // Ignore Separator and Multiplex  // Enum Class BitStart_Length_ByteOrder_Signedness
        FACTOR__OFFSET, // Enum Class Factor_Offset
        MINIMUM__MAXIMUM,   // Enum Class Minimum_Maximum
        UNIT,
        RX_NODES,
        _COUNT_NO_MULTIPLEX_INFO , // WITHOUT OPTIONAL MULTIPLEX INFO
        _COUNT_WITH_MULTIPLEX_INFO,  // WITH OPTIONAL MULTIPLEX_INFO
        _COUNT_WITH_MULTIPLEX_INFO_SEPARATOR,  // WITH OPTIONAL MULTIPLEX_INFO
        _COUNT_NO_MULTIPLEX_INFO_SEPARATOR = _COUNT_WITH_MULTIPLEX_INFO_SEPARATOR - 1, // WITHOUT OPTIONAL MULTIPLEX INFO
        SEPARATOR_NO_MULTIPLEX_INFO = MULTIPLEX_INFO // WITHOUT OPTIONAL MULTIPLEX INFO
    };
    enum class BitStart_Length_ByteOrder_Signedness{
        BITSTART,
        LENGTH,
        BYTE_ORDER,
        SIGNEDNESS,
        _COUNT
    };
    enum class Factor_Offset{
        FACTOR,
        OFFSET,
        _COUNT
    };
    enum class Minimum_Maximum{
        MINIMUM,
        MAXIMUM,
        _COUNT
    };
    // Possible Unit with spaces
    int startIndex = -1;
    int endIndex = -1;
    for(int i = 0; i < interpreterData->lineData.size(); i++)
    {
        if(interpreterData->lineData.at(i).contains(QString("\""))){
            if(startIndex == -1){
                startIndex = i;
            }else{
                if(endIndex == -1)
                {
                    endIndex = i;
                }else{
                    // Assume that there is only string
                    qDebug() << "More Strings with Speach Marks";
                    return config.ERROR_CALL();
                }
            }
        }
    }
    if(startIndex == -1){
        qDebug() << "No Speach Marks - File Syntax Error";
        return config.ERROR_CALL();
    }else{
        QString tempStr;
        while(startIndex <= endIndex--){
            tempStr += interpreterData->lineData.takeAt(startIndex);
        }
        interpreterData->lineData.replace(startIndex, tempStr);
    }


    if(interpreterData->lastDbMsg){
        // check and remove SEPARATORS
        switch(static_cast<DBSigData>(interpreterData->lineData.size())){
        case DBSigData::_COUNT_WITH_MULTIPLEX_INFO_SEPARATOR:
        {
            if(interpreterData->lineData.at(static_cast<int>(DBSigData::SEPARATOR_WITH_MULTIPLEX_INFO)) != QString(":"))
                return config.ERROR_CALL();
            interpreterData->lineData.removeAt(static_cast<int>(DBSigData::SEPARATOR_WITH_MULTIPLEX_INFO));
        }
            break;
        case DBSigData::_COUNT_NO_MULTIPLEX_INFO_SEPARATOR:
        {
            if(interpreterData->lineData.at(static_cast<int>(DBSigData::SEPARATOR_NO_MULTIPLEX_INFO)) != QString(":"))
                return config.ERROR_CALL();
            interpreterData->lineData.removeAt(static_cast<int>(DBSigData::SEPARATOR_NO_MULTIPLEX_INFO));
        }
            break;
        default:
            return config.ERROR_CALL();
        }

        DbSigVar* dbSigVar = DbSigVar::createClassVar(DbSig::getNullDef(), false, false);
        if(!dbSigVar){
            return config.ERROR_CALL(Error::ERROR);
        }
        SetParamRules spr;
        bool ok = false;
        // NAME
        {
            QString dbSigName = interpreterData->lineData.at(static_cast<int>(DBSigData::NAME));
            if(!CAPLVariable::checkSymbolicName(dbSigName)){
                qDebug() << "Wrong DBSig Name: " + dbSigName;
                SET_PTR_DO(dbSigVar, nullptr);
                return config.ERROR_CALL(Error::ERROR);
            }
            if(!dbSigVar->setParam(dbSigName, spr = {DB_GET_REAL_PARAM(CAPLObject, CAPL_OBJECT_NAME), INT_MAX, SET_PARAM_ACTION_ADD})){
                SET_PTR_DO(dbSigVar, nullptr);
                return config.ERROR_CALL(Error::ERROR);
            }
        }
        // MULTIPLEX (Read And Remove for LineData)
        {
            if(interpreterData->lineData.size() == static_cast<int>(DBSigData::_COUNT_WITH_MULTIPLEX_INFO)){    // MULTIPLEX Exist
                QString dbSigMultiplexData = interpreterData->lineData.takeAt(static_cast<int>(DBSigData::MULTIPLEX_INFO));
                QRegExp tempRegExp("m\\d+");
                if(tempRegExp.exactMatch(dbSigMultiplexData) && tempRegExp.matchedLength() == dbSigMultiplexData.size()){
                    // Remove m by right(size - 1)
                    dbSigMultiplexData = dbSigMultiplexData.right(dbSigMultiplexData.size() - 1);
                    dbSigVar->setSignalType(DbSigVar::SignalType::MULTIPLEXED_SIGNAL);
                    if((dbSigVar->setMultiplexValue(dbSigMultiplexData.toUInt(&ok)), !ok)){
                        qDebug() << "Wrong DBSig MultiplexData Value: " + dbSigMultiplexData;
                        SET_PTR_DO(dbSigVar, nullptr);
                        return config.ERROR_CALL(Error::ERROR);
                    }
                }else{
                    if(dbSigMultiplexData == "M"){
                        dbSigVar->setSignalType(DbSigVar::SignalType::MULTIPLEXOR);
                    }else{
                        qDebug() << "Wrong DBSig MultiplexData: " + dbSigMultiplexData;
                        SET_PTR_DO(dbSigVar, nullptr);
                        return config.ERROR_CALL(Error::ERROR);

                    }
                }
            }
        }
        // BITSTART, LENGTH, BYTE_ORDER, SIGNEDNESS
        {
            QString sigInfo = interpreterData->lineData.at(static_cast<int>(DBSigData::BITSTART__LENGTH__BYTE_ORDER__SIGNEDNESS));
            QStringList splittedSigInfo;
            if((splittedSigInfo = sigInfo.split(QRegExp("\\D+"), Qt::SkipEmptyParts), splittedSigInfo.size() != 3) ||
                   !((sigInfo.endsWith('+') || sigInfo.endsWith('-'))) )
            {
                qDebug() << "Wrong DBSig Info: " + sigInfo;
                SET_PTR_DO(dbSigVar, nullptr);
                return config.ERROR_CALL(Error::ERROR);
            }
            { // BITSTART
                quint32 bitstart = splittedSigInfo.at(static_cast<int>(BitStart_Length_ByteOrder_Signedness::BITSTART)).toUInt(&ok);
                if(!ok || !dbSigVar->at(static_cast<int>(DbSig::Property::BITSTART))->setParam(&bitstart, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
                    qDebug() << "Wrong DBSigInfo::BitStart = " << splittedSigInfo.at(static_cast<int>(BitStart_Length_ByteOrder_Signedness::BITSTART));
                    return config.ERROR_CALL(Error::ERROR);
                }
            }
            {   // BITCOUNT
                quint32 bitcount = splittedSigInfo.at(static_cast<int>(BitStart_Length_ByteOrder_Signedness::LENGTH)).toUInt(&ok);
                if(!ok || !dbSigVar->at(static_cast<int>(DbSig::Property::BITCOUNT))->setParam(&bitcount, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
                    qDebug() << "Wrong DBSigInfo::BitCount = " << splittedSigInfo.at(static_cast<int>(BitStart_Length_ByteOrder_Signedness::LENGTH));
                    return config.ERROR_CALL(Error::ERROR);
                }
            }
            {   // BYTE_ORDER
                DbSigVar::ByteOrder byteOrder = static_cast<DbSigVar::ByteOrder>(splittedSigInfo.at(static_cast<int>(BitStart_Length_ByteOrder_Signedness::BYTE_ORDER)).toInt(&ok));
                if(!ok || static_cast<int>(byteOrder) > static_cast<int>(DbSigVar::ByteOrder::INTEL) || static_cast<int>(byteOrder) < 0){
                    qDebug() << "Wrong DBSigInfo::ByteOrder = " << splittedSigInfo.at(static_cast<int>(BitStart_Length_ByteOrder_Signedness::BYTE_ORDER));
                    return config.ERROR_CALL(Error::ERROR);
                }
                dbSigVar->setByteOrder(byteOrder);
            }
            {   // SIGNEDNESS
                DbSigVar::Signedness signedness = (sigInfo.endsWith('+'))? DbSigVar::Signedness::UNSIGNED : DbSigVar::Signedness::SIGNED;
                dbSigVar->setSignedness(signedness);
            }
        }
        {   // FACTOR__OFFSET
            QString sigInfo = interpreterData->lineData.at(static_cast<int>(DBSigData::FACTOR__OFFSET));
            QStringList splittedSigInfo;
            QRegExp regExp(QString("\\(") + RegExpCore::regExpForFloats + QString("\\,") + RegExpCore::regExpForFloats + QString("\\)"));
            if(!regExp.exactMatch(sigInfo) ||
                    regExp.matchedLength() != sigInfo.size() ||
                    (splittedSigInfo = sigInfo.split(QString(","), Qt::SkipEmptyParts), splittedSigInfo.size() != 2) ||
                    (splittedSigInfo[0] = splittedSigInfo.at(0).right(splittedSigInfo.at(0).size() - 1), splittedSigInfo[1].chop(1), false))
            {
                qDebug() << "Wrong DBSig Info: " + sigInfo;
                SET_PTR_DO(dbSigVar, nullptr);
                return config.ERROR_CALL(Error::ERROR);
            }
            {   // FACTOR
                float factor = splittedSigInfo.at(static_cast<int>(Factor_Offset::FACTOR)).toFloat(&ok);
                if(!ok || !dbSigVar->at(static_cast<int>(DbSig::Property::FACTOR))->setParam(&factor, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
                    qDebug() << "Wrong DBSigInfo::Factor = " << splittedSigInfo.at(static_cast<int>(Factor_Offset::FACTOR));
                    return config.ERROR_CALL(Error::ERROR);
                }
            }
            {   // OFFSET
                float offset = splittedSigInfo.at(static_cast<int>(Factor_Offset::OFFSET)).toFloat(&ok);
                if(!ok || !dbSigVar->at(static_cast<int>(DbSig::Property::OFFSET))->setParam(&offset, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
                    qDebug() << "Wrong DBSigInfo::Offset = " << splittedSigInfo.at(static_cast<int>(Factor_Offset::OFFSET));
                    return config.ERROR_CALL(Error::ERROR);
                }
            }
        }
        {   // MINIMUM__MAXIMUM
            QString sigInfo = interpreterData->lineData.at(static_cast<int>(DBSigData::MINIMUM__MAXIMUM));
            QStringList splittedSigInfo;
            QRegExp regExp(QString("\\[") + RegExpCore::regExpForFloats + QString("\\|") + RegExpCore::regExpForFloats + QString("\\]"));
            if(!regExp.exactMatch(sigInfo) ||
                    regExp.matchedLength() != sigInfo.size() ||
                    (splittedSigInfo = sigInfo.split(QString("|"), Qt::SkipEmptyParts), splittedSigInfo.size() != 2) ||
                    (splittedSigInfo[0] = splittedSigInfo.at(0).right(splittedSigInfo.at(0).size() - 1), splittedSigInfo[1].chop(1), false))
            {
                qDebug() << "Wrong DBSig Info: " + sigInfo;
                SET_PTR_DO(dbSigVar, nullptr);
                return config.ERROR_CALL(Error::ERROR);
            }
            {   // MINIMUM
                double minimum = splittedSigInfo.at(static_cast<int>(Minimum_Maximum::MINIMUM)).toDouble(&ok);
                if(!ok || !dbSigVar->at(static_cast<int>(DbSig::Property::MINIMUM))->setParam(&minimum, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
                    qDebug() << "Wrong DBSigInfo::Minimum = " << splittedSigInfo.at(static_cast<int>(Minimum_Maximum::MINIMUM));
                    return config.ERROR_CALL(Error::ERROR);
                }
            }
            {   // OFFSET
                double maximum = splittedSigInfo.at(static_cast<int>(Minimum_Maximum::MAXIMUM)).toDouble(&ok);
                if(!ok || !dbSigVar->at(static_cast<int>(DbSig::Property::MAXIMUM))->setParam(&maximum, spr = {DB_GET_REAL_PARAM(CAPLVariable, CAPL_VARIABLE_VALUE), INT_MAX, SET_PARAM_ACTION_ADD})){
                    qDebug() << "Wrong DBSigInfo::Maximum = " << splittedSigInfo.at(static_cast<int>(Minimum_Maximum::MAXIMUM));
                    return config.ERROR_CALL(Error::ERROR);
                }
            }
        }
        {   // UNIT
            QString unit = interpreterData->lineData.at(static_cast<int>(DBSigData::UNIT));
            QStringList splittedSigInfo;

            if(!unit.isEmpty()){
                if(!static_cast<CAPLArray*>(dbSigVar->at(static_cast<int>(DbSig::Property::UNIT)))->copyStringData(unit))
                    return config.ERROR_CALL();
            }
        }
        //qDebug() << dbSigVar->toString();
        interpreterData->lastDbMsg->append(dbSigVar);
    }

    return true;
}

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::TX_MESSAGE_NODES_LIST>(){

    return true;
}

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::ENVIRONMENT_VARIABLE>(){

    return true;
}

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::UNKNOWN_LINE>(){
    switch (interpreterData->curStat) {
    case Stat::NODES_LIST:
    {
        config.stats.append(static_cast<int>(interpreterData->curStat) + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
    }
        break;
    default:
        break;
    }
    return true;
}


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_CANoeDBC_CANoeDBC::Config>> FSD_CANoeDBC_CANoeDBC::processingFunctions = {
    &FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::NONE>,
    &FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::NODES_LIST>,
    &FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::DBMSG>,
    &FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::SIGNAL_>,
    &FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::TX_MESSAGE_NODES_LIST>,
    &FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::ENVIRONMENT_VARIABLE>,
    &FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::UNKNOWN_LINE>,
};

template<>
template<>
bool FSD_CANoeDBC_CANoeDBC::Config::FSFunction<FSD_CANoeDBC::FileSpecificInterpreterStat::PROCESS_LINE>(){
    FRI_COM_Manager* COM = FRI_COM_Manager::managerAccess();
    if(!COM)
        return ERROR_CALL(Error::ERROR);
    NS_FRI_COM_Manager::ChannelCommand *dataCmd = COM->receive();
    if(!dataCmd || dataCmd->command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL(Error::ERROR);
    interpreterData->lineData = static_cast<QString*>(dataCmd->channelData)->split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
    if(!interpreterData->lineData.isEmpty()){
        int i = 0;
        for(i = 0; i < interpreterData->tagMapping.size(); i++){
            if(interpreterData->tagMapping.at(i).tagName == interpreterData->lineData.first()){
               stats.append(static_cast<int>((interpreterData->curStat = interpreterData->tagMapping.at(i).processingStat)) + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
               interpreterData->lineData.removeFirst();
               break;
            }
        }
        if(i == interpreterData->tagMapping.size()){
            stats.append(static_cast<int>(Stat::UNKNOWN_LINE) + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
        }
    }else{
        interpreterData->curStat = Stat::NONE;
    }
    return true;
}


template<>
QVarLengthArray<ProcessingFunctions_FRI<FSD_CANoeDBC_CANoeDBC::Config>, (int)FSD_CANoeDBC_CANoeDBC::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_CANoeDBC_CANoeDBC::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_CANoeDBC_CANoeDBC::Config::processingFunctions_FS = {
    &FSD_CANoeDBC_CANoeDBC::Config::FSFunction<FSD_CANoeDBC::FileSpecificInterpreterStat::PROCESS_LINE>,
};
