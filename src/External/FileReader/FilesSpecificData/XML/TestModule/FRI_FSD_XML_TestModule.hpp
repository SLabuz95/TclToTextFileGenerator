#ifndef FRI_FSD_XML_TESTMODULE_HPP
#define FRI_FSD_XML_TESTMODULE_HPP
#include"FileReader/FilesSpecificData/XML/FRI_FSD_XML.hpp"
#include"FileReader/FilesSpecificData/FSDConfig.hpp"

// Test Module Section ------------------------------------------------------
using  FSD_XML_TestModuleInterpreter = FSD_XML::Interpreter<TestModule>;

template <>
QString FSD_XML_TestModuleInterpreter::Config::fileFilter();

template<>
template<>
enum class FSD_XML_TestModuleInterpreter::Stat{
    TEST_MODULE,
    TEST_GROUP,
    CAPL_TEST_CASE,
    INFO,
    VARIANTS,
    VARIANT,
    PREPARATION,
    INIT,
    SYSVAR,
    TEST_DESCRIPTION,
    CAPL_PARAM,
    MISCINFO,
    NAME,
    COMPLETION,
    ENGINEER,
    CAPL_TEST_FUNCTION,

    SIZE
};
template<>
template<>
struct FSD_XML_TestModuleInterpreter::Data{
  struct DataModelStat{
      FSD_XML_TestModuleInterpreter::DataModel::DataModelBase* dataModel = nullptr;
      FSD_XML_TestModuleInterpreter::Stat stat = FSD_XML_TestModuleInterpreter::Stat::SIZE;
  };
  QVector<DataModelStat> dmStats;
};
template<>
template<>
const InterpreterStatsConfiguration FSD_XML_TestModuleInterpreter::statConfig;

template<>
template<>
enum class FSD_XML_TestModuleInterpreter::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_MODULE>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_GROUP>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_TEST_CASE>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::INFO>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::VARIANTS>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::VARIANT>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::PREPARATION>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::INIT>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::SYSVAR>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::TEST_DESCRIPTION>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_PARAM>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::MISCINFO>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::NAME>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::COMPLETION>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::ENGINEER>();

template<>template<>template<>
bool FSD_XML_TestModuleInterpreter::processingFunction<FSD_XML_TestModuleInterpreter::Stat::CAPL_TEST_FUNCTION>();


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_XML_TestModuleInterpreter::Config>> FSD_XML_TestModuleInterpreter::processingFunctions;

template<>
template<>
struct FSD_XML_TestModuleInterpreter::Functions{

};

// TestModule ----------------------------------------------------------------
template<>
bool FSD_XML_TestModuleInterpreter::Config::initialize();

template<>
bool FSD_XML_TestModuleInterpreter::Config::deinitialize();

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>();

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>();

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>();

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>();

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>();

template<>
template<>
bool FSD_XML_TestModuleInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_XML_TestModuleInterpreter>>, (int)FSD_XML_TestModuleInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_XML_TestModuleInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_XML_TestModuleInterpreter::Config::processingFunctions_FS;


using TestModuleFileInterpreterConfig = FileReaderInterpreter_Configuration<FSD_XML_TestModuleInterpreter>;


#endif // FRI_FSD_XML_TESTMODULE_HPP
