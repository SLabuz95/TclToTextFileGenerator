#ifndef CONTROLLERCONFIGINFO_HPP
#define CONTROLLERCONFIGINFO_HPP

#include"External/FileReader/FilesSpecificData/FSDTemplate.hpp"
#include<QString>
#include<QDateTime>
#include"Tcl2Capl/controllerconfig.hpp"
#include<QFileInfo>
#include<QBitArray>
#include<QMultiMap>
#include"controllerconfigmanager.hpp"

// Move to other file ControllerConfigInfo
class ControllerConfigInfo : public FSD_DataModelBase<ControllerConfigInfo>{
public:
    enum RulesCategories : qsizetype{   // To control rules list in
        OnEndOfCall = -2,
        UndefinedArgument = -1,
        //Dynamic = -1,
        // 0 > - Argument index value
    };

    enum ModifierRulesCategories : qsizetype{
        OnNoRulesSatisfied = -2,
        LowestValue = OnNoRulesSatisfied,
        OnEndOfRulesCheck = -1,
        PhaseRules = 0
    };

private:
    struct ProcedureRuleCategoryKey : public QPair<QString, RulesCategories>{
    public:
        using QPair<QString, RulesCategories>::QPair;
        friend bool operator<(const ProcedureRuleCategoryKey& lhs, const ProcedureRuleCategoryKey& rhs){
            return (lhs.second < rhs.second and rhs.first == lhs.first)
                    or lhs.first < rhs.first;
        }
    };

    struct PhaseModifierRuleCategoryKey : public QPair<QString, ModifierRulesCategories>{
    public:
        using QPair<QString, ModifierRulesCategories>::QPair;
        friend bool operator<(const PhaseModifierRuleCategoryKey& lhs, const PhaseModifierRuleCategoryKey& rhs){
            return (lhs.second < rhs.second and rhs.first == lhs.first)
                    or lhs.first < rhs.first;
        }
    };
public:
    using Config = Tcl2CaplControllerConfig;

    using ModifierPhaseMap = QMap<PhaseModifierRuleCategoryKey, qsizetype>;
    using ModifierSavedRules = Config::ModifierConfigRawRules;
    using ModifierNewRules = Config::ModifierConfigRules;
    using ModifierActions = Config::ModifierConfigActions;
    using ModifierRulesView = QPair<ModifierNewRules::Iterator, ModifierNewRules::Iterator>;

    struct ModifierRulesFromConfigFileView{
        ModifierRulesCategories category;
        ModifierNewRules rules;
    };

    struct PhaseView{
        ModifierPhaseMap::Iterator phase;
        ModifierRulesView rulesView;
    };

    using ProcedureMap = QMap<ProcedureRuleCategoryKey, qsizetype>;
    using DefaultProcedureMap = QMap<RulesCategories, qsizetype>;
    using SavedRules = Config::RawRules;
    using NewRules = Config::DynamicRawRules;
    using RulesView = QPair<NewRules::Iterator, NewRules::Iterator>;
    struct RulesFromConfigFileView{
        RulesCategories index;
        NewRules rules;
    };

    struct ProcedureView{
        ProcedureMap::Iterator procedure;
        RulesView rulesView;
    };
    using ConfigFile = ControllerConfigManager::ConfigFile;

protected:
    qsizetype numbOfExistingPhases = 0;
    qsizetype numbOfAllPhases = 0;

    qsizetype numbOfExistingProcedures = 0;
    qsizetype numbOfAllProcedures = 0;

    inline void countUp_NewPhase(){numbOfAllPhases++; numbOfExistingPhases++;}
    inline void countUp_RestoredPhase(){numbOfExistingPhases++;}
    inline void countDown_HidePhase(){numbOfExistingPhases--;}
    inline void countDown_RemoveHiddenPhase(){numbOfAllPhases--;}

    inline void countUp_NewProcedure(){numbOfAllProcedures++; numbOfExistingProcedures++;}
    inline void countUp_RestoredProcedure(){numbOfExistingProcedures++;}
    inline void countDown_HideProcedure(){numbOfExistingProcedures--;}
    inline void countDown_RemoveHiddenProcedure(){numbOfAllProcedures--;}
public:
    ControllerConfigInfo();

    public:
        // FOR NOW ALL AT NEW CONFIG ONLY , EVEN READ AND WRITE
        static constexpr qsizetype abs(qsizetype v){return (v < 0)? -(v+1) : v;}

        // Action 1. Edit Name of New Procedure (not in Config, in newConfig - onEndOfCall)
        // If acceptable,
        // - to new Procedure (not in config), change names in newConfig
        // - to removed procedure in Config,change names in newConfig -> merge (OnEnd - unset removed flag -> uneditted dont exist in newConfig (set removed if not)
        // -> noteditted exist in newConfig (unset removed)) -> editted exist and new index exist in newConfig (remove from editted ) and old index)

        // -------------------------- INTERFACE OF TCL CONFIG ------------------------------------------------

        bool addProcedure(QString name);
        void clearAllNewRules(QString procedureName);
        void clearAllNewRules(QString procedureName, qsizetype index);
        void clearAllNewRules(qsizetype index);
        void moveAllNewRules(QString oldName, QString newName);
        void moveAllNewRules(QString name, qsizetype oldIndex, qsizetype newIndex);
        void moveAllNewRules(qsizetype oldIndex, qsizetype newIndex);
        bool editProcedureName(QString oldName, QString newName);
        bool removeProcedure(QString name);
        bool addIndex(QString name, qsizetype index);
        bool addIndex(qsizetype index); // DefaultProcedure
        bool editIndex(QString name, qsizetype oldIndex, qsizetype newIndex);
        bool editIndex(qsizetype oldIndex, qsizetype newIndex); // DefaultProcedure
        void removeIndex(QString name, qsizetype index);
        void removeIndex(qsizetype index); // DefaultProcedure
        void clearIndexes(QString name);
        void clearIndexes(); // DefaultProcedure
        void clearProcedures();
        void clear();

        inline bool isProcedureExist(QString procedure){return newProceduresMap.contains({procedure, RulesCategories::OnEndOfCall});}
        bool isEmpty()const{
            return _settings.isEmpty() and
                    newProceduresMap.isEmpty() and
                    newDefaultProcedureMap.size() == 2 and
                    newDefaultProcedureRules.isEmpty(); // 2 Categories and no rules
        }
        bool isLocalConfig()const{
            return not configFile.isConfigured();
        }

        void loadNewRules(QString, RulesCategories, NewRules&);
        void loadNewRules(RulesCategories, NewRules&);
        //void loadNewRules(QString, RulesForArg&);
        RulesView readRules(QString, RulesCategories);
        RulesView readRules(RulesCategories);
        using ProceduresView = QPair<ProcedureMap, DefaultProcedureMap>;
        ProceduresView readProceduresInfo(){
            return ProceduresView{newProceduresMap, newDefaultProcedureMap};
        }

        inline qsizetype getNumbOfExistingProcedures(){return numbOfExistingProcedures;}
        inline qsizetype getNumbOfAllProcedures(){return numbOfAllProcedures;}


        void readProcedures(QList<UserProcedure>&);
        UserProcedure readDefaultProcedure();
        inline Settings::WriteOnlyProcedures& writeOnlyProcedures(){return _settings.writeOnlyProcedures();}
        inline Settings& settings(){return _settings;}

        inline const QString& filePath()const{return configFile.configPath();}

        // XML Section        
        bool addIndex(QString name, RulesFromConfigFileView& rulesView);
        bool addIndex(RulesFromConfigFileView& rulesView); // DefaultProcedure

        void toXmlContent(QXmlStreamWriter& xmlWriter);
        void writeSettingsToXML(QXmlStreamWriter&);
        void writeProceduresToXML(QXmlStreamWriter&);
        void writeDefaultProcedureToXML(QXmlStreamWriter&);

        void changeConfigFile(QString newPath){configFile.changeConfigPath(newPath);}

        // -------------------------- INTERFACE OF MODIFIER CONFIG ------------------------------------------------
        bool addPhase(QString name);
        void clearAllNewModifierRules(QString name);
        void clearAllNewModifierRules(QString name, ModifierRulesCategories index);
        void moveAllNewModifierRules(QString oldName, QString newName);
        bool editPhaseName(QString oldName, QString newName);
        bool removePhase(QString name);
        void clearPhases();

        inline bool isPhaseExist(QString phase){return newPhasesMap.contains({phase, ModifierRulesCategories()});}

        void loadNewModifierRules(QString, ModifierRulesCategories, ModifierNewRules&);
        ModifierRulesView readModifierRules(QString, ModifierRulesCategories);

        using PhasesView = ModifierPhaseMap;
        PhasesView readPhasesInfo(){
            return PhasesView{newPhasesMap};
        }

        inline qsizetype getNumbOfExistingPhases(){return numbOfExistingPhases;}
        inline qsizetype getNumbOfAllPhases(){return numbOfAllPhases;}


        void readPhases(TcFileModifierConfigBase::ModifierPhases&);
        // XML Section
        bool addCategory(QString name, ModifierRulesFromConfigFileView& rulesView);
        void writePhasesToXML(QXmlStreamWriter&);

    protected:
        ConfigFile configFile;

        // --------------------- DATA OF MODIFIER CONFIG -------------------------------------------
        ModifierPhaseMap newPhasesMap;// phases map
        ModifierNewRules newModifierRules;

        // --------------------- DATA OF TCL CONFIG -------------------------------------------
        // Local changes
        Settings _settings;
        ProcedureMap newProceduresMap;// procedures map
        NewRules newRules;
        DefaultProcedureMap newDefaultProcedureMap;// default procedure map
        NewRules newDefaultProcedureRules;

};


#endif // CONTROLLERCONFIGINFO_HPP
