#ifndef FRI_FSD_CUSTOM_CANOEDBC_HPP
#define FRI_FSD_CUSTOM_CANOEDBC_HPP

#include"FileReader/FilesSpecificData/CANoeDBC/FRI_FSD_CANoeDBC.hpp"
#include"CANoe/DBCObject/DBCNetwork/dbcnetwork.hpp"
#include"FileReader/FilesSpecificData/FSDConfig.hpp"


// CANoe Config ------------------------------------------------------------------------
using  FSD_CANoeDBC_CANoeDBC = FSD_CANoeDBC::Interpreter<DBCNetwork>;

template <>
QString FSD_CANoeDBC_CANoeDBC::Config::fileFilter();


template<>
template<>
enum class FSD_CANoeDBC_CANoeDBC::Stat{
    NONE,
    NODES_LIST,
    DBMSG,
    SIGNAL_,
    TX_MESSAGE_NODES_LIST,
    ENVIRONMENT_VARIABLE,

    UNKNOWN_LINE,

    SIZE
};
template<>
template<>
struct FSD_CANoeDBC_CANoeDBC::Data{
    struct TagMapping{
        QString tagName;
        Stat processingStat = Stat::SIZE;
    };

    /*enum class ManualControlStat{
        AUTOMATIC,
        WAIT_FOR_STRING,
        IGNORE_LINES,
        READ_LINE

    } readControl = ManualControlStat::AUTOMATIC;*/

    bool readControl_setErrorIfBeginOrEndOfObject = false;
    FSD_CANoeDBC_CANoeDBC::Stat statForManualControl;
    int ignoreLines = 0;
    QString manualControl_string;

  FSD_CANoeDBC_CANoeDBC::DataModel* dbNetwork = nullptr;
  DbMsgVar* lastDbMsg = nullptr;
  Stat curStat = Stat::NONE;
  //QVector<int> objectMappingTrace;
  static const QVector<TagMapping> tagMapping;

  QStringList lineData;
  int curNetworkInfo = 0;
  QString lastVDataBase;

  // Temp Solution
  QStringList blackList;
};

template<>
template<>
const InterpreterStatsConfiguration FSD_CANoeDBC_CANoeDBC::statConfig;

template<>
template<>
enum class FSD_CANoeDBC_CANoeDBC::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template<>
bool FSD_CANoeDBC_CANoeDBC::Config::initialize();

template<>
bool FSD_CANoeDBC_CANoeDBC::Config::deinitialize();

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::NONE>();

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::NODES_LIST>();

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::DBMSG>();

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::SIGNAL_>();

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::TX_MESSAGE_NODES_LIST>();

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::ENVIRONMENT_VARIABLE>();

template<>template<>template<>
bool FSD_CANoeDBC_CANoeDBC::processingFunction<FSD_CANoeDBC_CANoeDBC::Stat::UNKNOWN_LINE>();


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_CANoeDBC_CANoeDBC::Config>> FSD_CANoeDBC_CANoeDBC::processingFunctions;

template<>
template<>
struct FSD_CANoeDBC_CANoeDBC::Functions{

};

// -----------------------------------------------------------------------------------------------

template<>
template<>
bool FSD_CANoeDBC_CANoeDBC::Config::FSFunction<FSD_CANoeDBC::FileSpecificInterpreterStat::PROCESS_LINE>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_CANoeDBC_CANoeDBC>>, (int)FSD_CANoeDBC_CANoeDBC::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_CANoeDBC_CANoeDBC::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_CANoeDBC_CANoeDBC::Config::processingFunctions_FS;



#endif // FRI_FSD_CUSTOM_CANOEDBC_HPP
