#ifndef FRI_FSD_XML_VSYSVAR_HPP
#define FRI_FSD_XML_VSYSVAR_HPP
#include"FileReader/FilesSpecificData/XML/FRI_FSD_XML.hpp"
#include"FileReader/FilesSpecificData/FSDConfig.hpp"

// VSYSVAR Section ----------------------------------------------------------
using  FSD_XML_CAPLVSysvarInterpreter = FSD_XML::Interpreter<CAPLVSysvar>;

template<>
template<>
enum class FSD_XML_CAPLVSysvarInterpreter::Stat{
    SYSTEM_VARIABLES,
    NAMESPACE,
    VARIABLE,
    VALUE_TABLE,
    VALUE_TABLE_ENTRY,
    STRUCT,
    STRUCT_MEMBER,

    SIZE
};
template<>
template<>
struct FSD_XML_CAPLVSysvarInterpreter::Data{
  struct DataModelStat{
      FSD_XML_CAPLVSysvarInterpreter::DataModel::DataModelBase* dataModel = nullptr;
      FSD_XML_CAPLVSysvarInterpreter::Stat stat = FSD_XML_CAPLVSysvarInterpreter::Stat::SIZE;
  };
   using DMStats = QVector<DataModelStat>;
  QVector<DataModelStat> dmStats;
  QString fullNamespaceStr;
  QString fullStructMemberNameStr;
};
template<>
template<>
const InterpreterStatsConfiguration FSD_XML_CAPLVSysvarInterpreter::statConfig;

template<>
template<>
enum class FSD_XML_CAPLVSysvarInterpreter::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::SYSTEM_VARIABLES>();

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::NAMESPACE>();

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VARIABLE>();

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VALUE_TABLE>();

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::VALUE_TABLE_ENTRY>();

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::STRUCT>();

template<>template<>template<>
bool FSD_XML_CAPLVSysvarInterpreter::processingFunction<FSD_XML_CAPLVSysvarInterpreter::Stat::STRUCT_MEMBER>();


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_XML_CAPLVSysvarInterpreter::Config>> FSD_XML_CAPLVSysvarInterpreter::processingFunctions;

template<>
template<>
struct FSD_XML_CAPLVSysvarInterpreter::Functions{

};
// ---------------------------------------------------------------------------------------


// Config_Virtuals Section -----------------------------------------------------------


template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::initialize();

template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::deinitialize();

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>();

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>();

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>();

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>();

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>();

template<>
template<>
bool FSD_XML_CAPLVSysvarInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_XML_CAPLVSysvarInterpreter>>, (int)FSD_XML_CAPLVSysvarInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_XML_CAPLVSysvarInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_XML_CAPLVSysvarInterpreter::Config::processingFunctions_FS;


using CAPLVSysVarFileInterpreterConfig = FileReaderInterpreter_Configuration<FSD_XML_CAPLVSysvarInterpreter>;


#endif // FRI_FSD_XML_VSYSVAR_HPP
