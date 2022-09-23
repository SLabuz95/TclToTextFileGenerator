#ifndef TCFILEMODIFIERCONFIGBASE_HPP
#define TCFILEMODIFIERCONFIGBASE_HPP

#include<QStringList>

namespace TcFileModifierConfigBase {
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
        // Config Public Stats
        // HELPER - SUPP
        SPLIT,  // (1): (Pattern)
        WRITE_ATTRIBUTE,
        // CONDITIONALS
        STARTS_WITH,        // "<any string>" -  just write,  "" - next argument take arguments in pattern
        COMPARE,
        // EXECUTABLES
        WRITE,  // "<any string>" -  just write,  "" - next argument take arguments in pattern
        CHANGE_PHASE,
        INTERPRET,
        // PRIVATE
        /*
        ENDS_WITH,  // "<any string>" -  just write,  "" - next argument take arguments in pattern
        */
        WRITE_TO_TC_INFO,   // (1): (number of stat)  (2 ...): "<any string>" -  just write,  "" - next argument take arguments in pattern
        COMMENT_OUT,
        /*
        SSTR_SAVE,   // Arguments like write
        SSTR_SLICE,
        */
        REPLACE_ALL,  // Works only for lineData (raw data) Arguments:(1)-before, (2)-after
        DEBUG,
        //FORMAT,
        /*
        COMPARE_REGEX,
        FORMAT,
        CONTAINS,
        REMOVE_SPLIT_RESULT_IF_ENDS_WITH,
        GET_MSG_BY_SIG_NAME,
        ADD_VARIABLE,
        */

    };
    /*enum class Phase{
        TEST_CASE_INFO,
        STANDARD,
        /*
        CHECK_SIGNAL,
        CHECK_SIGNAL__KWP_DIAGNOSTIC,
        SET_SIGNAL,
        PROC_E2P_CheckOrRestore,
        CALL,
        IF,
        IGNORE_UNTIL_END_OF_BLOCK,
    };*/

    class Format{
    public:
        enum class Rule : char{
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
            ATTRIBUTE = 'A',
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
        static const QString cast_target_str(const Target t){return QString::number(static_cast<uint>(t));}
        static inline std::underlying_type_t<Target> cast_target(const Target t){return static_cast<std::underlying_type_t<Target>>(t);}
        static inline Target castTo_target(const std::underlying_type_t<Target> t){return static_cast<Target>(t);}

        static inline std::underlying_type_t<Rule> cast_format_rule(const Rule t){return static_cast<std::underlying_type_t<Rule>>(t);}
        inline static const QString cast_format_rule_str(const Rule t){return QString(cast_format_rule(t));}
        inline static const QString cast_format_rule_str(const Rule t, const QString& data){
            return QString(cast_format_rule(t)) + data;
        }
        inline static const QString FORMAT_RULE_CALL(){return "";}
        inline static void addFormatRule(QStringList& parameters, const Rule rule, const QString& data){
            parameters << FORMAT_RULE_CALL() << cast_format_rule_str(rule, data);
        }
    };


    enum class ModifierRuleControl{
        BREAK_RULE_CHECK,
        NO_BREAK_RULE_CHECK,
        BREAK_RULE_CHECK_DONT_EXEC_ON_END_ACTIONS
    };

    struct ModifierRule{
        struct Action{
            ActionStat action;
            QStringList arguments;
        };
        using Actions = QVector<Action>;
        Actions conditions;
        Actions actions;
        ModifierRuleControl controlFlag = ModifierRuleControl::BREAK_RULE_CHECK;
    };
    using ModifierRule = ModifierRule;
    using ModifierRules = QVector<ModifierRule>;
    struct ModifierPhase{
        ModifierRules rules;
        struct OnEnd {
            ModifierRule::Actions onNoRules;
            ModifierRule::Actions onEndOfRulesCheck;
        } onEnd;
    };

    using ModifierPhases = QMap<QString, ModifierPhase>;
}

#endif // TCFILEMODIFIERCONFIGBASE_HPP
