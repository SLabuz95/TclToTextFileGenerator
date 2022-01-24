#ifndef FRI_FSD_XML_TCL_CAPL_PARSER_CONFIG_H
#define FRI_FSD_XML_TCL_CAPL_PARSER_CONFIG_H
#include"External/FileReader/FilesSpecificData/XML/FRI_FSD_XML.hpp"
#include"External/FileReader/FilesSpecificData/FSDConfig.hpp"
#include"Tcl2Capl/controllerconfigxmldata.hpp"
#include"External/XML/XMLDefinition.hpp"
#include<QStack>

// Test Module Section ------------------------------------------------------
using  FSD_XML_TclCaplParserConfigInterpreter = FSD_XML::Interpreter<Tcl2CaplControllerConfigXmlData>;

template <>
QString FSD_XML_TclCaplParserConfigInterpreter::Config::fileFilter();

template<>
template<>
enum class FSD_XML_TclCaplParserConfigInterpreter::Stat{
    UserConfig,
    Settings,
    WriteOnlyFunctions,
    Procedure,
    DefaultProcedure,
    RulesForArgument,
    RulesForUnspecifiedArgument,
    RulesOnEndOfCall,
    DynamicRules,
    RulesOnMove,
    Rule,
    ConditionalAction,
    ExecutableAction,
    ActionParameter,
    FormatRule,
    ParametersList,

    Forbidden,
    SIZE
};
// Find CONFIG_TAG to change tag name (Temporary Solution)

template<>
template<>
struct FSD_XML_TclCaplParserConfigInterpreter::Data{
    using Interpreter = Data;
    /* using Action = DataModel::UserProceduresConfig::Type::Action;
    using Rule = DataModel::UserProceduresConfig::Type::Rule;
    using Conditional = DataModel::UserProceduresConfig::Type::Rule::ConditionalActions::Type;
    using Executable = DataModel::UserProceduresConfig::Type::Rule::ExecutableActions::Type;

   enum class ConversionType{
        Int,
        Uint,
        AnyParam,   // String
        ParamOnly,  // FormatParam is forbidden
    };

    template<typename _ActionType>
    requires (std::is_same_v<_ActionType, Action::Conditional>
              or std::is_same_v<_ActionType, Action::Executable>)
    struct ActionDefinition{
        using ActionType = _ActionType;
        struct ParameterInfo{
            ConversionType conversionType = ConversionType::AnyParam;
            //Stat expectedStat = Stat::SIZE;
            int expectedSize = INT_MAX;
            bool addListSizes = false;
        };
        using Parameters = QVector<ParameterInfo>;

        Parameters parameters;
    };
    const QString checkAndAddActionParameter(const Stat I_processingStat, QString I_processsingStr);
    inline void incrementCurActionParameterIndex(){currentActionParameterIndex++;}
    inline void clearCurActionParameterIndex(){currentActionParameterIndex = -1;}

    using ConditionalActionDefinitions = QVector<ActionDefinition<Action::Conditional>>;
    using ExecutableActionDefinitions = QVector<ActionDefinition<Action::Executable>>;

    static  ConditionalActionDefinitions conditionalActionDefinitions;
    static  ExecutableActionDefinitions executableActionDefinitions;
*/    
    using XMLAttributes = const QStringList;

    struct XMLToken{
    public:
        class XMLObject;
        using ObjectRef = const XMLObject&;
        using DefinitionRef = const XMLToken&;
        using Tokens = const QList<const XMLToken * >;
        using Attributes =  XMLAttributes;
        using ProcessingFunction = QString (Interpreter::*)(ObjectRef, ObjectRef);

        /*inline XMLToken(const QString name) :
            name(name){}
        inline XMLToken(const QString name, const Tokens requiredTokens, const Tokens optionalTokens) :
            XMLToken(name), requieredTokens(requiredTokens), optionalTokens(optionalTokens) {}*/
        const QString name;
        Tokens requiredTokens;
        Tokens optionalTokens;
        Attributes requiredAttributes;
        Attributes optionalAttributes;
        const bool requiredTextContent = false;
        const bool expectedNoDuplicates = false;
        QList<ProcessingFunction> onCreate;
        QList<ProcessingFunction> onDestroy;

        class XMLObject{
            using XMLDefinition = const XMLToken *;
            using XMLDefinitions = QList<XMLDefinition>;

            QString initialize(XMLDefinition definition){ // For Global definition only
                informations = QBitArray(
                            definition->requiredTokens.size() +
                            ((definition->expectedNoDuplicates)? definition->optionalTokens.size() : 0) +
                            definition->optionalAttributes.size() + 1 /* textOnly */
                            );
            }

            QString initialize(XMLDefinition definition, QXmlStreamReader& reader){ // For Global definition only
                informations = QBitArray(
                            definition->requiredTokens.size() +
                            ((definition->expectedNoDuplicates)? definition->optionalTokens.size() : 0) +
                            definition->optionalAttributes.size() + 1 /* textOnly */
                            );
            }

            QString findToken(QXmlStreamReader& reader, XMLDefinition definition)
            {
                // Find Token
                using XMLToken = XMLToken::Tokens::ConstIterator;


                return QString();
            }

        public:
            inline static  XMLObject createXmlObject(QString& errorMsg, XMLDefinitions definitions){
                XMLObject object;
                errorMsg = object.initialize(definitions.last());
                return object;
            }

            inline static  XMLObject createXmlObject(QXmlStreamReader& reader,  QString& errorMsg, XMLDefinitions definitions){
                XMLObject object;
                errorMsg = object.initialize(definitions.last(), reader);
                return object;
            }

        protected:
            // Information have to read about token from Reader
            // -- RequiredTokens +  WARNING
            // -- (only if no duplicates) OptionalTokens +
            // -- optionalAttributes
            // -- textOnly
            QBitArray informations;

        };
    };
    using XMLObject = XMLToken::XMLObject;
    using ObjectRef = XMLToken::ObjectRef;
    using DefinitionRef = XMLToken::DefinitionRef;

   static const XMLToken paramToken;
   static const XMLToken formatParamToken;
   static const XMLToken conditionalActionToken;
   static const XMLToken executableActionToken;
   static const XMLToken ruleToken;
   static const XMLToken dynamicRulesToken;
   static const XMLToken rulesOnMoveToken;
   static const XMLToken rulesForArgumentToken;
   static const XMLToken rulesForUnspecifiedArgumentToken;
   static const XMLToken rulesOnEndOfCallToken;
   static const XMLToken procedureToken;
   static const XMLToken defaultProcedureToken;
   static const XMLToken writeOnlyProcedureToken;
   static const XMLToken writeOnlyProceduresToken;
   static const XMLToken settingsToken;
   static const XMLToken userConfigToken;
   static const XMLToken xmlDefinition;


    template<DefinitionRef parent, DefinitionRef current>
    static QString processingFunction(ObjectRef, ObjectRef);

    class ParametersList{
        QStringList parameters;
    public:
        inline void append(QString str){parameters.append(str);}
        inline void append(QStringList strList){parameters.append(strList);}
        inline QStringList getParametersPrependedWithSize(){return QStringList({QString::number(parameters.size())}) + parameters;}
        inline QStringList getParameters(){return parameters;}

    } parametersList;

    static constexpr std::underlying_type_t<Stat> stat2type(Stat stat){return static_cast<std::underlying_type_t<Stat>>(stat);}
    struct DataModelStat{
         void* dataModel = nullptr;    // Unsafe but not abstruct data
         FSD_XML_TclCaplParserConfigInterpreter::Stat stat = FSD_XML_TclCaplParserConfigInterpreter::Stat::SIZE;
    };
    using DataModelStats = QVector<DataModelStat>;
    DataModelStats dmStats;
    //Rule tempRule;
    QStringList listOfDefinitions;
    int currentActionParameterIndex = -1;
    QVector<int> arguments;
    bool defaultProcedureUsed = false;
    bool rulesForArgumentsUsed = false;
    bool rulesForUnspecifiedArgumentUsed = false;
    bool rulesOnEndOfCallUsed = false;
    bool dynamicRulesUsed = false;
    bool onMoveRulesUsed = false;

    QStack<XMLObject> xmlObjects;

};
template<>
template<>
const InterpreterStatsConfiguration FSD_XML_TclCaplParserConfigInterpreter::statConfig;
/*
template<>
template<>
enum class FSD_XML_TclCaplParserConfigInterpreter::Error{
    NO_ERROR,
    ERROR,

    SIZE
};*/

template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_XML_TclCaplParserConfigInterpreter::Config>> FSD_XML_TclCaplParserConfigInterpreter::processingFunctions;

template<>
template<>
struct FSD_XML_TclCaplParserConfigInterpreter::Functions{

};

// TestModule ----------------------------------------------------------------
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::initialize();

template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::deinitialize();

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::NO_TOKEN>();

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::INVALID>();

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_DOCUMENT>();

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_DOCUMENT>();

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::START_ELEMENT>();

template<>
template<>
bool FSD_XML_TclCaplParserConfigInterpreter::Config::FSFunction<FSD_XML::FileSpecificInterpreterStat::END_ELEMENT>();

template<>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<FSD_XML_TclCaplParserConfigInterpreter>>, (int)FSD_XML_TclCaplParserConfigInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_XML_TclCaplParserConfigInterpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_XML_TclCaplParserConfigInterpreter::Config::processingFunctions_FS;


using TestModuleFileInterpreterConfig = FileReaderInterpreter_Configuration<FSD_XML_TclCaplParserConfigInterpreter>;


#endif // FRI_FSD_XML_TCL_CAPL_PARSER_CONFIG_H
