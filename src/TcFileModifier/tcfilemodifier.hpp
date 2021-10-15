#ifndef TCFILEMODIFIER_HPP
#define TCFILEMODIFIER_HPP

#include"tcfilereaderconfig.hpp"
#include"tcfilemodifierdata.hpp"
#include"External/FileReader/FilesSpecificData/FSDConfig.hpp"
#include"tclToCAPL.hpp"
#include"Tcl2Capl/controller.hpp"
#include"Tcl2Capl/Result/tcl2caplresult.hpp"


// CANoe Config ------------------------------------------------------------------------
using FSD_ByLine_TcFileModifierData = FSD_ByLine::Interpreter<Tcl2CaplResult::Tcl2CaplReadData>;
using TcFileModifier = FSD_ByLine_TcFileModifierData;

template <>
QString FSD_ByLine_TcFileModifierData::Config::fileFilter();

#define TC_MODIFIER_VER 0
#if TC_MODIFIER_VER == 0
template<>
template<>
enum class FSD_ByLine_TcFileModifierData::Stat{
    REPLACE_BY_MAPPING,
    ACTION_STARTS_WITH,
    /*
    ACTION_ENDS_WITH,
    */
    ACTION_WRITE_TO_TC_INFO,
    /*
    ACTION_TC_INFO_TO_FILE,
    */
    ACTION_WRITE,
    ACTION_SPLIT,
    ACTION_COMMENT_OUT,
    ACTION_CHANGE_PHASE,
    /*
    ACTION_SSTR_SAVE,
    ACTION_SSTR_SLICE,
    */
    ACTION_REPLACE_ALL,
    ACTION_DEBUG,
    ACTION_COMPARE,
    //ACTION_FORMAT,
    /*
    ACTION_COMPARE_REGEX,
    ACTION_FORMAT,
    ACTION_CONTAINS,
    ACTION_REMOVE_SPLIT_RESULT_IF_ENDS_WITH,
    ACTION_GET_MSG_BY_SIG_NAME,
    ACTION_ADD_VARIABLE,
    */
    ACTION_INTERPRET,

    SIZE
};
template<>
template<>
struct FSD_ByLine_TcFileModifierData::Data{
    Data(UserInputConfig& userConfig, CAPLFunctionDefinitionsRef caplFunctionDefinitionsRef)
        : tclToCaplInterpreter_(userConfig, caplFunctionDefinitionsRef){}

    using Variables = QStringList;
    struct CAN{        
        using VtSignals = QMap<QString, QStringList>;
        static VtSignals _signals;
    };    

    enum class TC_Info_Data{
        NAME,
        TITLE,
        DESCRIPTION,
        TC_DOMAIN,
        INTEGRATION,
        REGRESSION,
        REQUIREMENTS,
        DOCUMENTS,
        AUTHOR,
        VERSION,
        TYPE,
        SIZE
    };

    struct TC_Info{
        QString name;
        QString title;
        QString description;
        QString domain;
        QString integration;
        QString regression;
        QString requirements;
        QString documents;
        QString author;
        QString version;
        QString type;
    };
    enum class ActionStat{
        STARTS_WITH,        // "<any string>" -  just write,  "" - next argument take arguments in pattern
        /*
        ENDS_WITH,  // "<any string>" -  just write,  "" - next argument take arguments in pattern
        */
        WRITE_TO_TC_INFO,   // (1): (number of stat)  (2 ...): "<any string>" -  just write,  "" - next argument take arguments in pattern
        WRITE,  // "<any string>" -  just write,  "" - next argument take arguments in pattern
        SPLIT,  // (1): (Pattern)
        COMMENT_OUT,
        CHANGE_PHASE,
        /*
        SSTR_SAVE,   // Arguments like write
        SSTR_SLICE,
        */
        REPLACE_ALL,  // Works only for lineData (raw data) Arguments:(1)-before, (2)-after
        DEBUG,
        COMPARE,
        //FORMAT,
        /*
        COMPARE_REGEX,
        FORMAT,
        CONTAINS,
        REMOVE_SPLIT_RESULT_IF_ENDS_WITH,
        GET_MSG_BY_SIG_NAME,
        ADD_VARIABLE,
        */
        INTERPRET

    };
    enum class Phase{
        TEST_CASE_INFO,
        STANDARD,
        /*
        CHECK_SIGNAL,
        CHECK_SIGNAL__KWP_DIAGNOSTIC,
        SET_SIGNAL,
        PROC_E2P_CheckOrRestore,
        CALL,
        IF,
        IGNORE_UNTIL_END_OF_BLOCK,*/
    };

    class Format{
    public:
        enum class Rule{
            INDEX_OR_FULL_LINE = '=',
            ARGS_AFTER_INDEX = '>',
            /*
            ARG_IN_RANGE_P1 = '<',
            ARG_IN_RANGE_P2 = '>',
            CUT_INDEX = '/',  // / - arguments (1: index to cut, 2: numb of signs to cut)
            CUT_AFTER_INDEX = 'C',
            */
            SEPARATOR = '@',
            TARGET = 'T',
            /*
            SPLIT = 'S',
            SLICED = '\\'
            */
        };
        enum class Target{
            RAW,
            SPLITTED_RAW,
            //SSTR,
        };
        static inline uint cast_target(const Target t){return static_cast<uint>(t);}
        static const QString cast_target_str(const Target t){return QString::number(static_cast<uint>(t));}

    };


    enum class ModifierRuleControl{
        NO_BREAK_RULE_CHECK,
        BREAK_RULE_CHECK,
        BREAK_RULE_CHECK_DONT_EXEC_ON_END_ACTIONS
    };

    struct _ModifierRule{
        struct Action{
            ActionStat action;
            QStringList arguments;
        };
        using Actions = QVector<Action>;
        Actions conditions;
        Actions actions;
        ModifierRuleControl controlFlag = ModifierRuleControl::BREAK_RULE_CHECK;
    };
    using ModifierRule = _ModifierRule;
    using ModifierRules = QVector<ModifierRule>;
    struct ModifierPhase{
        ModifierRules rules;
        struct OnEnd {
            ModifierRule::Actions onNoRules;
            ModifierRule::Actions onEndOfRulesCheck;
        } onEnd;
    };


    using ModifierPhases = QVector<ModifierPhase>;
    QDir fileDir;
    QString lineData;
    //QString sstr;
    static const ModifierPhases phases;
    QStringList arguments;
    bool conditionResult = false;
    QStringList lastActionResponse;
    TC_Info tcData;
    Phase curPhase = Phase::TEST_CASE_INFO;
    TCLInterpreter tclToCaplInterpreter_;
    int curLine = 0;
    //Variables variables;
    //QString tcContent;


    bool createAndAssignString(QString&, QStringList);
    void writeTCInfo(FSD_ByLine_TcFileModifierData::DataModel& dataModel);
    void writeTcToFile();
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
