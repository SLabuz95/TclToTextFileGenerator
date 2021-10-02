#ifndef FRI_FSD_CUSTOM_CANOEDBC_HPP
#define FRI_FSD_CUSTOM_CANOEDBC_HPP

#include"FileReader/FilesSpecificData/Custom/FRI_FSD_Custom.hpp"
#include"CANoe/Configuration/canoeconfig.hpp"
#include"FileReader/FilesSpecificData/FSDConfig.hpp"

class CANoeDBC;
// CANoe Config ------------------------------------------------------------------------

using  FSD_Custom_CANoeDBC = FSD_Custom::Interpreter<CANoeDBC>;

template <>
QString FSD_Custom_CANoeDBC::Config::fileFilter();


template<>
template<>
enum class FSD_Custom_CANoeDBC::Stat{
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
struct FSD_Custom_CANoeDBC::Data{
    enum class ManualControlStat{
        AUTOMATIC,
        WAIT_FOR_STRING,
        IGNORE_LINES,
        READ_LINE

    } readControl = ManualControlStat::AUTOMATIC;

    bool readControl_setErrorIfBeginOrEndOfObject = false;
    FSD_Custom_CANoeDBC::Stat statForManualControl;
    int ignoreLines = 0;
    QString manualControl_string;

  QVector<FSD_Custom_CANoeDBC::DataModel::DataModelBase*> dmStats;
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
const InterpreterStatsConfiguration FSD_Custom_CANoeDBC::statConfig;

template<>
template<>
enum class FSD_Custom_CANoeDBC::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template<>
bool FSD_Custom_CANoeDBC::Config::initialize();

template<>
bool FSD_Custom_CANoeDBC::Config::deinitialize();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::STANDARD>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::TEST_ENVIRONMENT>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VSYSVAR>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::DBC>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::CAPL_NODE>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::EOF_MBSSDATA_FOR_NETWORKS_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::NUMB_OF_NETWORKS_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::NETWORK_NAME_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::NETWORK_TYPE_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::NETWORK_CHANNEL_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::EOF_BUS_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VDATABASE>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VDATABASE_DATABASE_NAME_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VDATABASE_NETWORK_NAME_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VSIMULATION_NODE>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VSIMULATION_NODE_DB_NODE_NAME_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VSIMULATION_NODE_DB_NAME_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VSIMULATION_NODE_EOF_MBSSDATA_LINE_BEFORE_NETWORK_INDEXES>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VSIMULATION_NODE_NUMB_OF_NETWORK_INDEXES_READ>();

template<>template<>template<>
bool FSD_Custom_CANoeDBC::processingFunction<FSD_Custom_CANoeDBC::Stat::VSIMULATION_NODE_NETWORK_INDEX_READ>();


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_Custom_CANoeDBC::Config>> FSD_Custom_CANoeDBC::processingFunctions;

template<>
template<>
struct FSD_Custom_CANoeDBC::Functions{

};

// -----------------------------------------------------------------------------------------------

template<>
template<>
bool FSD_Custom_CANoeDBC::Config::FSFunction<FSD_Custom::FileSpecificInterpreterStat::PROCESS_LINE>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_Custom_CANoeDBC>>, (int)FSD_Custom_CANoeDBC::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_Custom_CANoeDBC::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_Custom_CANoeDBC::Config::processingFunctions_FS;



#endif // FRI_FSD_CUSTOM_CANOEDBC_HPP
