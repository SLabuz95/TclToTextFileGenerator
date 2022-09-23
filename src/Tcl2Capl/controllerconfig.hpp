#ifndef CONTROLLERCONFIG_H
#define CONTROLLERCONFIG_H

#include"TclInterpreter/tclToCAPL.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"
#include"Tcl2Capl/Config/Rules/FCT_products.hpp"
#include"TcFileModifier/Config/Rules/FCT_products.hpp"

class Tcl2CaplControllerConfig{
public:
    using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
    using Settings = Tcl::Interpreter::Command::Settings;
    using WriteOnlyProcedures = Settings::WriteOnlyProcedures;
    using UserProcedure = Tcl::Interpreter::Command::Definition;
    using UserProcedures = Tcl::Interpreter::Command::CommandDefinitions;
    using UserDefaultProcedure = UserProcedure;
    using Predefinitions = Tcl::Interpreter::PredefinitionsController::Predefinitions;
    using InterpreterRule = UserProcedure::Rule;
    using InterpreterRules = QList<InterpreterRule>;
    using Mode = Settings::InterpreterMode;
    using ControlFlag = InterpreterRule::Control;
    using UserInteraction = Tcl::Interpreter::UserInteraction;
    using UserInteractionStatus = Tcl::Interpreter::UserInteractionStatus;
    using ConditionalActions = InterpreterRule::ConditionalActions;
    using ExecutableActions = InterpreterRule::ExecutableActions;

public:    


    using ModifierConfigRawRule = ModifierRulesFactory::Product<ModifierRulesTypes::RawRule>;
    using ModifierConfigRawRules = ModifierRulesFactory::List<ModifierRulesTypes::RawRule>;
    using ModifierConfigRules = ModifierRulesFactory::ListOfBases;
    using ModifierConfigActions = ModifierActionsFactory::ListOfBases;

    using RulesForArgument = RulesFactory::Product<RulesTypes::RulesForArgument>;
    using RawRule = RulesFactory::Product<RulesTypes::RawRule>;
    using RawRules = RulesFactory::List<RulesTypes::RawRule>; // Stack oriented
    using DynamicRawRules = RulesFactory::ListOfBases; // Heap oriented
    using RulesForArguments = RulesFactory::List<RulesTypes::RulesForArgument>;

    struct Attribute{
        QString value;
    };

    using Attributes = QMap<QString, Attribute>;

    class Procedure{    // For config only
    public:

    protected:
        using Name = QString;

        Name name_;
        UserInteractionStatus userInteraction = UserInteraction::defaultStatus();
        DynamicRawRules rules_;

        //RawRules rulesOnEndOfCall_;
        //RulesForArguments rulesForArguments_;

    public:
        /*struct RulesView{
            RawRules const& rulesOnEndOfCall;
            RulesForArguments const& rulesForArguments;
        };*/

        Procedure(Name name = QString(), UserInteractionStatus userInteraction = UserInteraction::defaultStatus())
            : name_(name), userInteraction(userInteraction){}

        Procedure(const Procedure& procedureRef, Name newName);

        UserProcedure toProcedureWithRawRules();
        //inline RulesForArguments& rulesForArguments(){return rulesForArguments_;}
        //inline RawRules& rulesOnEndOfCall(){return rulesOnEndOfCall_;}

        inline bool operator==(const Procedure& rhs){
            return name_ == rhs.name_;
        }

        inline QString name()const{return name_;}
/*
        inline RulesView rules(){
            return RulesView{rulesOnEndOfCall_, rulesForArguments_};
        }

        inline bool isDefault(){
            return userInteraction == UserInteraction::defaultStatus() and
                    rulesOnEndOfCall_.isEmpty() and
                    rulesForArguments_.isEmpty();
        }
*/
        void toXmlContent(QXmlStreamWriter& xmlWriter);
    };    
    // For now created Dynamicly - saved static, new dynamic
public:
    class Procedures : protected QList<Procedure*>{
        using Super = QList<Procedure*>;
    public:
        using Super::size_type;
        using Super::size;
        using Super::contains;
        using Super::indexOf;
        using Super::append;
        using Super::at;
        using Super::Type;
        using Super::Iterator;
        using Super::begin;
        using Super::end;
        using Super::clear;
        using Super::isEmpty;
        using Super::value_type;
        using Super::last;
        operator QStringList(){
            QStringList procedures(size());
            QStringList::Iterator procedureName = procedures.begin();
            for(Super::Iterator procedure = begin(); procedure != end(); procedure++, procedureName++)
               (*procedureName) = (*procedure)->name();
            return procedures;
        }
        inline void toXmlContent(QXmlStreamWriter& xmlWriter){
            for(Iterator procedure = begin(); procedure != end(); procedure++){

            }
        }
    };
    Tcl2CaplControllerConfig(){}
    //Tcl2CaplControllerConfig(Tcl2CaplControllerConfigXmlData&);
    ~Tcl2CaplControllerConfig(){}


protected:
    // Dynamic Property (Not for File)
    Mode _mode = Mode::TestCase;
    // Properties For File
    Attributes _attributes;
    Settings _settings;
    Procedures _userProcedures;
    Procedure _userDefaultProcedure;

public:
    inline Procedures& userProcedures(){return _userProcedures;}
    inline Procedures::size_type getNumbOfUserProcedures()const{return _userProcedures.size();}
    inline Procedure& defaultProcedure(){return _userDefaultProcedure;}
    inline void setMode(Mode m){_mode = m;}
    inline Mode mode(){return _mode;}
    inline Attributes& attributes(){return _attributes;}
    inline WriteOnlyProcedures& writeOnlyProcedures(){return _settings.writeOnlyProcedures();}
    inline Settings& settings(){return _settings;}

    inline bool  isDefault(){
        return _settings.writeOnlyProcedures().isEmpty() and
                _userProcedures.isEmpty() /*and
                _userDefaultProcedure.isDefault();*/;
    }

    //bool writeToFile(QFile file);
    QString toXmlContent();

};

bool operator==(const Tcl2CaplControllerConfig::Procedure& lhs, const Tcl2CaplControllerConfig::Procedure& rhs);
bool operator==(const Tcl2CaplControllerConfig::Attribute& attribute, const QString& rhs);
#endif // CONTROLLERCONFIG_H
