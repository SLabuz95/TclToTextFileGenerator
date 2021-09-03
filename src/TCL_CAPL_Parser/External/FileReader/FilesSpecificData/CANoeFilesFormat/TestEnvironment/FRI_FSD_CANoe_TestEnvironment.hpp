#ifndef FRI_FSD_CANOE_TESTENVIRONMENT_HPP
#define FRI_FSD_CANOE_TESTENVIRONMENT_HPP

#include"FileReader/FilesSpecificData/CANoeFilesFormat/FRI_FSD_CANoe.hpp"
#include"CANoe/Other/canoetestenvironment.hpp"
#include"FileReader/FilesSpecificData/FSDConfig.hpp"

//  Test Environment ----------------------------------------------------------------------------
using  FSD_CANoe_CANoeTestEnvironment = FSD_CANoe::Interpreter<CANoeTestEnvironment>;

template <>
QString FSD_CANoe_CANoeTestEnvironment::Config::fileFilter();


template<>
template<>
enum class FSD_CANoe_CANoeTestEnvironment::Stat{
    STANDARD,
    //TEST_ENVIRONMENT,
    TEST_MODULE,
    CAPL_SCRIPT,

    SIZE
};
template<>
template<>
struct FSD_CANoe_CANoeTestEnvironment::Data{
  QVector<FSD_CANoe_CANoeTestEnvironment::DataModel::DataModelBase*> dmStats;
  QVector<int> objectMappingTrace;
  static const CANoeFileObjectMapping objectsMapping;
  const CANoeFileObjectMapping* curObjectMapping = &objectsMapping;
  QString ignoreTil;
  QString tempPathHelper;
  // Temp Solution
  QStringList blackList;
  QVector<FSD_CANoe_CANoeTestEnvironment::DataModel::DataModelBase*> dmStats_blackList;

};

template<>
template<>
const InterpreterStatsConfiguration FSD_CANoe_CANoeTestEnvironment::statConfig;

template<>
template<>
enum class FSD_CANoe_CANoeTestEnvironment::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template<>
bool FSD_CANoe_CANoeTestEnvironment::Config::initialize();

template<>
bool FSD_CANoe_CANoeTestEnvironment::Config::deinitialize();



template<>template<>template<>
bool FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::STANDARD>();

template<>template<>template<>
bool FSD_CANoe_CANoeTestEnvironment::processingFunction<FSD_CANoe_CANoeTestEnvironment::Stat::TEST_MODULE>();


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_CANoe_CANoeTestEnvironment::Config>> FSD_CANoe_CANoeTestEnvironment::processingFunctions;

template<>
template<>
struct FSD_CANoe_CANoeTestEnvironment::Functions{

};

// ---------------------------------------------------------------------------------------------

template<>
template<>
bool FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::BEGIN_OF_OBJECT>();

template<>
template<>
bool FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::END_OF_OBJECT>();

template<>
template<>
bool FSD_CANoe_CANoeTestEnvironment::Config::FSFunction<FSD_CANoe::FileSpecificInterpreterStat::DATA>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_CANoe_CANoeTestEnvironment>>, (int)FSD_CANoe_CANoeTestEnvironment::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_CANoe_CANoeTestEnvironment::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_CANoe_CANoeTestEnvironment::Config::processingFunctions_FS;


#endif // FRI_FSD_CANOE_TESTENVIRONMENT_HPP
