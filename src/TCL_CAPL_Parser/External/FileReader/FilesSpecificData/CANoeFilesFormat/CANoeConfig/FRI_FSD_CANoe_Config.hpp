#ifndef FRI_FSD_CANOE_CONFIG_HPP
#define FRI_FSD_CANOE_CONFIG_HPP

#include"FileReader/FilesSpecificData/CANoeFilesFormat/FRI_FSD_CANoe.hpp"
#include"CANoe/Configuration/canoeconfig.hpp"
#include"FileReader/FilesSpecificData/FSDConfig.hpp"

class CANoeConfig;
// CANoe Config ------------------------------------------------------------------------

using  FSD_CANoe_CANoeConfig = FSD_CANoe::Interpreter<CANoeConfig>;

template <>
QString FSD_CANoe_CANoeConfig::Config::fileFilter();


template<>
template<>
enum class FSD_CANoe_CANoeConfig::Stat{
    STANDARD,
    SIZE_FOR_FUNCTIONS,
    TEST_ENVIRONMENT = SIZE_FOR_FUNCTIONS,
    VSYSVAR,
    DBC,
    CAPL_NODE,
    EOF_MBSSDATA_FOR_NETWORKS_READ,
    NUMB_OF_NETWORKS_READ,
    NETWORK_NAME_READ,
    NETWORK_TYPE_READ,  // CAN  = 1, Ethernet = 11
    NETWORK_CHANNEL_READ,
    EOF_BUS_READ,
    VDATABASE,
    VDATABASE_DATABASE_NAME_READ,
    VDATABASE_NETWORK_NAME_READ,
    VSIMULATION_NODE,
    VSIMULATION_NODE_DB_NODE_NAME_READ,
    VSIMULATION_NODE_DB_NAME_READ,
    VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES,
    VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ,
    VSIMULATION_NODE_NETWORK_INDEX_READ,

    SIZE
};
template<>
template<>
struct FSD_CANoe_CANoeConfig::Data{
    enum class ManualControlStat{
        AUTOMATIC,
        WAIT_FOR_STRING,
        IGNORE_LINES,
        READ_LINE

    } readControl = ManualControlStat::AUTOMATIC;

    bool readControl_setErrorIfBeginOrEndOfObject = false;
    bool readControl_ignoreIfBeginOrEndOfObject = false;
    FSD_CANoe_CANoeConfig::Stat statForManualControl;
    int ignoreLines = 0;
    QString manualControl_string;

  QVector<FSD_CANoe_CANoeConfig::DataModel::DataModelBase*> dmStats;
  QVector<int> objectMappingTrace;
  static const CANoeFileObjectMapping objectsMapping;
  const CANoeFileObjectMapping* curObjectMapping = &objectsMapping;
  QString ignoreTil;
  QString tempPathHelper;
  int curNetworkInfo = 0;
  QString lastVDataBase;

  // Temp Solution
  QStringList blackList;
};

template<>
template<>
const InterpreterStatsConfiguration FSD_CANoe_CANoeConfig::statConfig;

template<>
template<>
enum class FSD_CANoe_CANoeConfig::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template<>
bool FSD_CANoe_CANoeConfig::Config::initialize();

template<>
bool FSD_CANoe_CANoeConfig::Config::deinitialize();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::STANDARD>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::TEST_ENVIRONMENT>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSYSVAR>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::DBC>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::CAPL_NODE>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::EOF_MBSSDATA_FOR_NETWORKS_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NUMB_OF_NETWORKS_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_NAME_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_TYPE_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::NETWORK_CHANNEL_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::EOF_BUS_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE_DATABASE_NAME_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VDATABASE_NETWORK_NAME_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_DB_NODE_NAME_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_DB_NAME_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ>();

template<>template<>template<>
bool FSD_CANoe_CANoeConfig::processingFunction<FSD_CANoe_CANoeConfig::Stat::VSIMULATION_NODE_NETWORK_INDEX_READ>();


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_CANoe_CANoeConfig::Config>> FSD_CANoe_CANoeConfig::processingFunctions;

template<>
template<>
struct FSD_CANoe_CANoeConfig::Functions{

};

// -----------------------------------------------------------------------------------------------

template<>
template<>
bool FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::BEGIN_OF_OBJECT>();

template<>
template<>
bool FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::END_OF_OBJECT>();

template<>
template<>
bool FSD_CANoe_CANoeConfig::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::DATA>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_CANoe_CANoeConfig>>, (int)FSD_CANoe_CANoeConfig::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_CANoe_CANoeConfig::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_CANoe_CANoeConfig::Config::processingFunctions_FS;


#endif // FRI_FSD_CANOE_CONFIG_HPP
