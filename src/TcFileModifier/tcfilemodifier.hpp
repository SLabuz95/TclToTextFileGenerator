#ifndef TCFILEMODIFIER_HPP
#define TCFILEMODIFIER_HPP

#include"tcfilemodifierconfigbase.hpp"
#include"tcfilereaderconfig.hpp"
#include"tcfilemodifierdata.hpp"
#include"External/FileReader/FilesSpecificData/FSDConfig.hpp"
#include"TclInterpreter/tclToCAPL.hpp"
//#include"Tcl2Capl/controller.hpp"
//#include"Tcl2Capl/Result/tcl2caplresult.hpp"
#include"Tcl2Capl/Result/tcl2caplreaddata.hpp"


// CANoe Config ------------------------------------------------------------------------
using FSD_ByLine_TcFileModifierData = FSD_ByLine::Interpreter<Tcl2CaplReadData>;
using TcFileModifier = FSD_ByLine_TcFileModifierData;

template <>
QString FSD_ByLine_TcFileModifierData::Config::fileFilter();

#define TC_MODIFIER_VER 0
#if TC_MODIFIER_VER == 0
template<>
template<>
enum class FSD_ByLine_TcFileModifierData::Stat{
    REPLACE_BY_MAPPING,
    PROCESSING_ACTION_START,
    ACTION_SPLIT = PROCESSING_ACTION_START,
    ACTION_WRITE_ATTRIBUTE,
    ACTION_STARTS_WITH,
    ACTION_COMPARE,
    ACTION_WRITE,
    ACTION_CHANGE_PHASE,
    ACTION_INTERPRET,
    /*
    ACTION_ENDS_WITH,
    */
    ACTION_WRITE_TO_TC_INFO,
    /*
    ACTION_TC_INFO_TO_FILE,
    */
    ACTION_COMMENT_OUT,
    /*
    ACTION_SSTR_SAVE,
    ACTION_SSTR_SLICE,
    */
    ACTION_REPLACE_ALL,
    ACTION_DEBUG,
    //ACTION_FORMAT,
    /*
    ACTION_COMPARE_REGEX,
    ACTION_FORMAT,
    ACTION_CONTAINS,
    ACTION_REMOVE_SPLIT_RESULT_IF_ENDS_WITH,
    ACTION_GET_MSG_BY_SIG_NAME,
    ACTION_ADD_VARIABLE,
    */

    SIZE
};

using namespace Tcl;
using namespace TcFileModifierConfigBase;
template<>
template<>
struct FSD_ByLine_TcFileModifierData::Data{

    Data(UserInputConfig& userConfig, FunctionDefinitionsRef caplFunctionDefinitionsRef)
        : tclToCaplInterpreter_(userConfig, caplFunctionDefinitionsRef),
          userConfig_(userConfig)
    {}


    QDir fileDir;
    QString lineData;
    //QString sstr;
    QStringList arguments;
    bool conditionResult = false;
    QStringList lastActionResponse;
    TC_Info tcData;
    ModifierPhases::ConstIterator curPhase;
    TCLInterpreter tclToCaplInterpreter_;
    int curLine = 0;
    UserInputConfig& userConfig_;
    //Variables variables;
    //QString tcContent;


    int createAndAssignString(QString&, QStringList);
    void writeTCInfo(FSD_ByLine_TcFileModifierData::DataModel& dataModel);
    void writeTcToFile();
    bool toDefaultPhase();
};

// Stat Function Definitions

#endif

template<>
template<>
const InterpreterStatsConfiguration FSD_ByLine_TcFileModifierData::statConfig;

/*
template<>
template<>
enum class FSD_ByLine_TcFileModifierData::Error{
    NO_ERROR,
    ERROR,

    SIZE
};
*/
template<>
bool FSD_ByLine_TcFileModifierData::Config::initialize();

template<>
bool FSD_ByLine_TcFileModifierData::Config::deinitialize();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::REPLACE_BY_MAPPING>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_STARTS_WITH>();
/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_ENDS_WITH>();
*/
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE_TO_TC_INFO>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMMENT_OUT>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_CHANGE_PHASE>();
/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SSTR_SAVE>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SSTR_SLICE>();
*/
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_REPLACE_ALL>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_DEBUG>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMPARE>();

/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMPARE_REGEX>();


template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_CONTAINS>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_REMOVE_SPLIT_RESULT_IF_ENDS_WITH>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_GET_MSG_BY_SIG_NAME>();

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_ADD_VARIABLE>();
// ---*/
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_INTERPRET>();

template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_ByLine_TcFileModifierData::Config>> FSD_ByLine_TcFileModifierData::processingFunctions;

template<>
template<>
struct FSD_ByLine_TcFileModifierData::Functions{

};

// -----------------------------------------------------------------------------------------------

template<>
template<>
bool FSD_ByLine_TcFileModifierData::Config::FSFunction<FSD_ByLine::FileSpecificInterpreterStat::PROCESS_LINE>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_ByLine_TcFileModifierData>>, (int)FSD_ByLine_TcFileModifierData::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_ByLine_TcFileModifierData::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_ByLine_TcFileModifierData::Config::processingFunctions_FS;

#endif // TCFILEMODIFIER_HPP
