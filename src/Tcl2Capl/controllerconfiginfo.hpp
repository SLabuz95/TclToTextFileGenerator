#ifndef CONTROLLERCONFIGINFO_HPP
#define CONTROLLERCONFIGINFO_HPP

#include<QString>
#include<QDateTime>
#include"Tcl2Capl/controllerconfig.hpp"
#include<QFileInfo>
#include<QBitArray>
#include<QMultiMap>

// Move to other file ControllerConfigInfo
class ControllerConfigInfo;
class Rule;
class ControllerConfigInfo2{
public:
    ControllerConfigInfo2(ControllerConfigInfo&);

    ControllerConfigInfo& configInfo;

    class DefaultProcedureLocalChanges{
    protected:
        enum RulesCategories : qsizetype{   // TO control rules list
           // OnEndOfCall = -2, Position is always 0 for that case other function will be created?
           UndefinedArgument = -1,
           //Dynamic = -1,
           // 0 > - Argument index value

       };
        // Removed Rules mask
        // Created based on number of rules from configuration - it will never change til change of configuration
        using RemovedProcedures = QBitArray; // Maybe int to control number of removed rules
        using UserInterationProcedures = QBitArray;
        using RemovedRules = QBitArray; // Maybe int to control number of removed rules
        //using NewIndexes = QList<uint>; // Only information about new indexes - there are sorted
        //using NewRules = QList<Rule*>;
        // Probably i can calculate position of rules even for new indexes
        // New Index > Old Index -> Right position

        struct ProcedureRuleCategoryKey : private QPair<QString, RulesCategories>{
        public:
            using QPair<QString, RulesCategories>::QPair;
            friend bool operator<(const ProcedureRuleCategoryKey& lhs, const ProcedureRuleCategoryKey& rhs){
                return (lhs.second < rhs.second and rhs.first == lhs.first)
                        or lhs.first < rhs.first;
            }
        };

        // Local changes
        RemovedProcedures removedProcedures;
        RemovedRules removedRules;
        UserInterationProcedures userInterationProceduresChanged;
        QMap<RulesCategories, QPair<qsizetype, qsizetype>> mapWithNewRules;// Pos/Size Pair
        //NewIndexes newIndexes;

        // Const config infos - in config
        //int numbOfProcedures; - Can be derived from userInteractionProcedures size
        QMap<RulesCategories, qsizetype> mapWithRuleCategoriesPositions;// Pos/Size Pair

    };

};

// Change to ControllerConfigFileInfo
class ControllerConfigInfo{
    // Ver 2 VVVVVVVVVVVVVVVVV

    /* Ver 1 VVVVVVVVVVVVVVVVV
public:
    using ConfigPath = QString;
    using RegisterFileModificationTime = QDateTime;
    using ControllerConfigRef = Tcl2CaplControllerConfig&;

protected:
    ConfigPath configPath;
    RegisterFileModificationTime modificationTime;
    Tcl2CaplControllerConfig _controllerConfig;

public:
    ControllerConfigInfo(){}
    ControllerConfigInfo(ConfigPath, RegisterFileModificationTime, Tcl2CaplControllerConfig);

    using Self = ControllerConfigInfo;
    using SelfPtr = ControllerConfigInfo*;
    using SelfRef = ControllerConfigInfo&;

    inline bool operator==(QString rhs){return this->configPath == rhs;}
    inline ControllerConfigRef controllerConfig(){return _controllerConfig;}
    inline void updateConfig(Tcl2CaplControllerConfig& cnf){_controllerConfig = cnf;}
    inline void updateModificationTime(){modificationTime = QFileInfo(configPath).lastModified();}
    inline bool isFileUpdated(){return QFileInfo(configPath).lastModified() > modificationTime;}
    inline bool isFileConfiguration(){return not configPath.isEmpty();}
    inline bool isTemporaryConfiguration(){return configPath.isEmpty();}
    inline ConfigPath path(){return configPath;}
    inline bool isDefaultConfig(){
        return _controllerConfig.isDefault();
    }*/
};

#endif // CONTROLLERCONFIGINFO_HPP
