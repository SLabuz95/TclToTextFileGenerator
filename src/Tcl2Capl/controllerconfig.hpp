#ifndef CONTROLLERCONFIG_H
#define CONTROLLERCONFIG_H

#include"TclInterpreter/tclToCAPL.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"
#include"Tcl2Capl/Config/Rules/FCT_products.hpp"

class Tcl2CaplControllerConfigXmlData;
class Tcl2CaplControllerConfig{
public:
    using TclProcedureInterpreter = Tcl::Interpreter::CommandsController::Controller;
    using Settings = TclProcedureInterpreter::ProdecuresSettings;
    using WriteOnlyProcedures = Settings::WriteOnlyProcedures;
    using UserProcedure = Tcl::Interpreter::CommandsController::Command::Definition;
    using UserProcedures = Tcl::Interpreter::CommandsController::CommandDefinitions;
    using UserDefaultProcedure = Tcl::Interpreter::CommandsController::Command::Definition;
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

    using RawRule = RulesFactory::ProductBase;
    using QuickRule = RulesFactory::Product<RulesTypes::QuickRule>;
    using AdvancedRule = RulesFactory::Product<RulesTypes::RawRule>;
    using RulesForArgument = RulesFactory::Product<RulesTypes::RulesForArgument>;

    using RawRuleRef = RawRule*;
    using RawRuleRefs = QList<RawRuleRef>;
    using RulesForArguments = QList<RulesForArgument>;

    class Procedure{    // For config only
        using Name = QString;

        Name name_;
        UserInteractionStatus userInteraction = UserInteraction::defaultStatus();
        RawRuleRefs rulesOnEndOfCall_;
        RulesForArguments rulesForArguments_;

    public:
        struct RulesView{
            RawRuleRefs const& rulesOnEndOfCall;
            RulesForArguments const& rulesForArguments;
        };

        Procedure(Name name = QString(), UserInteractionStatus userInteraction = UserInteraction::defaultStatus())
            : name_(name), userInteraction(userInteraction){}

        Procedure(const Procedure& procedureRef, Name newName);

        UserProcedure toProcedureWithRawRules();
        inline RulesForArguments& rulesForArguments(){return rulesForArguments_;}
        inline RawRuleRefs& rulesOnEndOfCall(){return rulesOnEndOfCall_;}

        inline bool operator==(const Procedure& rhs){
            return name_ == rhs.name_;
        }

        inline QString name()const{return name_;}

        inline bool allQuickRules(){
            for(RawRuleRefs::Iterator rule = rulesOnEndOfCall_.begin();
                rule != rulesOnEndOfCall_.end(); rule++)
            {
                //if(not ((*rule)->type() == RulesType::QuickRule))
                  //  return false;
            }
            return true;
        }
        inline RulesView rules(){
            return RulesView{rulesOnEndOfCall_, rulesForArguments_};
        }

        inline bool isDefault(){
            return userInteraction == UserInteraction::defaultStatus() and
                    rulesOnEndOfCall_.isEmpty() and
                    rulesForArguments_.isEmpty();
        }

        void toXmlContent(QXmlStreamWriter& xmlWriter);
    };    

public:
    class Procedures : protected QList<Procedure>{
        using Super = QList<Procedure>;        
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
               (*procedureName) = (*procedure).name();
            return procedures;
        }
        inline void toXmlContent(QXmlStreamWriter& xmlWriter){
            for(Iterator procedure = begin(); procedure != end(); procedure++){

            }
        }
    };
    Tcl2CaplControllerConfig(){}
    Tcl2CaplControllerConfig(Tcl2CaplControllerConfigXmlData&);
    ~Tcl2CaplControllerConfig(){}


protected:
    // Dynamic Property (Not for File)
    Mode _mode = Mode::TestCase;
    // Properties For File
    Settings _settings;
    Procedures _userProcedures;
    Procedure _userDefaultProcedure;

public:
    inline Procedures& userProcedures(){return _userProcedures;}
    inline Procedures::size_type getNumbOfUserProcedures()const{return _userProcedures.size();}
    inline Procedure& defaultProcedure(){return _userDefaultProcedure;}
    inline void setMode(Mode m){_mode = m;}
    inline Mode mode(){return _mode;}
    inline WriteOnlyProcedures& writeOnlyProcedures(){return _settings.writeOnlyProcedures();}
    inline Settings& settings(){return _settings;}

    inline bool  isDefault(){
        return _settings.writeOnlyProcedures().isEmpty() and
                _userProcedures.isEmpty() and
                _userDefaultProcedure.isDefault();
    }

    //bool writeToFile(QFile file);
    QString toXmlContent();

};

bool operator==(const Tcl2CaplControllerConfig::Procedure& lhs, const Tcl2CaplControllerConfig::Procedure& rhs);

#endif // CONTROLLERCONFIG_H
