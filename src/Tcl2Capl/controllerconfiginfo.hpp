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
        enum RulesCategories : qsizetype{   // TO control rules list
            OnEndOfCall = -2,
            UndefinedArgument = -1,
            //Dynamic = -1,
            // 0 > - Argument index value

        };
    public:
        // FOR NOW ALL AT NEW CONFIG ONLY , EVEN READ AND WRITE
        static constexpr qsizetype abs(qsizetype v){return (v < 0)? -(v+1) : v;}

        // Action 1. Edit Name of New Procedure (not in Config, in newConfig - onEndOfCall)
        // If acceptable,
        // - to new Procedure (not in config), change names in newConfig
        // - to removed procedure in Config,change names in newConfig -> merge (OnEnd - unset removed flag -> uneditted dont exist in newConfig (set removed if not)
        // -> noteditted exist in newConfig (unset removed)) -> editted exist and new index exist in newConfig (remove from editted ) and old index)

        bool addProcedure(QString name){
            // Preverification:
            /*if(editted.contains({name, RulesCategories::OnEndOfCall})){
                return false;
            }
            */
            // 1. Find key <name, OnEndOfCall> in config
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter configKeyIter;
            /*configKeyIter = configMap.find({name, RulesCategories::UndefinedArgument}); // To Check Edit status

            if(configKeyIter != configMap.end()){
                // TRUE (Found):
                // if removed ,  value is negative  -> OK
                if(configKeyIter.value() < 0){ // Edited name - value isnt negative
                    //return false;    // Its possible to create new procedure
                }else{ // Ignore - name is edited - New Procedure can be created
                    configKeyIter--; // Check On End of Call - is Removed
                    if(configKeyIter.value() < 0){
                        // Unlock and read - unlock by -(value + 1)
                        // TODO: Unlock in New Procedures Too
                        configKeyIter.value() = abs(configKeyIter.value());
                        return true;
                    }else{
                        return false;
                    }
                }
            }else{
                // FALSE (Not Found):
               // New Procedure - Continue
            }*/

            // Main:
            configKeyIter = newConfigMap.find({name, RulesCategories::OnEndOfCall}); //
            if(configKeyIter == newConfigMap.end()){
                // 1. Add new Keys:
                // iter1 = <name, OnEndOfCall>, - Value - Absolute rule position
                configKeyIter = newConfigMap.insert({name, RulesCategories::OnEndOfCall}, 0);
                // iter2 = <name, UndefinedArgument> -  Value - Offset of previous rule category = 0
                ConfigKeyIter configUndefinedKeyIter = newConfigMap.insert({name, RulesCategories::UndefinedArgument}, 0);
                // 2. Specify values for keys:
                // 2.1. Check if iter2 + 1 is valid key
                configUndefinedKeyIter++; // To +1
                if(configUndefinedKeyIter != newConfigMap.end()){
                    // TRUE:
                    // 2.2. Copy value of iter2 + 1 to value of iter1
                    configKeyIter.value() = abs(configUndefinedKeyIter.value());
                }else{
                    // FALSE:
                    // 2.2. Set value iter1 to rulesList.size
                    configKeyIter.value() = newRules.size();
                }
                configUndefinedKeyIter--;
                configUndefinedKeyIter.value() = configKeyIter.value();
                return true;
            }else{
                if(configKeyIter.value() < 0){
                    // Unlock and read - unlock by -(value + 1)
                    configKeyIter.value() = abs(configKeyIter.value());
                    return true;
                }else{
                    return false;
                }
            }
        }

        void clearAllNewRules(QString procedureName){
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;
            beginIter = newConfigMap.lowerBound({procedureName, static_cast<RulesCategories>(LONG_LONG_MIN)});
            if(beginIter != newConfigMap.end() and beginIter.key().first == procedureName){ // Any keys exist
                ConfigKeyIter savedConfigIter = beginIter;
                // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
                endIter = newConfigMap.lowerBound({procedureName, static_cast<RulesCategories>(LONG_LONG_MAX)});
                // 2. Calculate number of rules and remove it
                qsizetype numbOfRules = 0;
                if(endIter != newConfigMap.end()){
                    numbOfRules = abs(endIter.value()) - abs(beginIter.value());
                }else{
                    numbOfRules = newRules.size() - abs(beginIter.value());
                }
                newRules.remove(abs(beginIter.value()), numbOfRules);
                // 3. Recalculate all absolute values of rules after this procedure
                while(beginIter != newConfigMap.end()){
                    beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                    beginIter++;
                }
                // 4. Erase keys
                newConfigMap.erase(savedConfigIter, endIter);
            }
        }

        void clearAllNewRules(QString procedureName, qsizetype index){
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;
            beginIter = newConfigMap.find({procedureName, static_cast<RulesCategories>(index)});
            if(beginIter != newConfigMap.end()){ // Any rules for key
                ConfigKeyIter savedConfigIter = beginIter;
                // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
                beginIter++;
                endIter =beginIter;
                beginIter--;
                // 2. Calculate number of rules and remove it
                qsizetype numbOfRules = 0;
                if(endIter != newConfigMap.end()){
                    numbOfRules = abs(endIter.value()) - abs(beginIter.value());
                }else{
                    numbOfRules = newRules.size() - abs(beginIter.value());
                }
                if(numbOfRules != 0){
                    newRules.remove(abs(beginIter.value()), numbOfRules);
                    // 3. Recalculate all absolute values of rules after this procedure
                    while(beginIter != newConfigMap.end()){
                        beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                        beginIter++;
                    }
                }
                // 4. Erase keys
                newConfigMap.erase(savedConfigIter);
            }
        }

        void moveAllNewRules(QString oldName, QString newName){
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;
            beginIter = newConfigMap.lowerBound({oldName, RulesCategories::OnEndOfCall});
            if(beginIter != newConfigMap.end() and beginIter.key().first == oldName){
                endIter = newConfigMap.lowerBound({oldName, static_cast<RulesCategories>(LONG_LONG_MAX)});
                qsizetype position = abs(beginIter.value());
                qsizetype numbOfRules = 0;
                if(endIter != newConfigMap.end()){
                    numbOfRules = abs(endIter.value()) - position;
                }else{
                    numbOfRules = newRules.size() - position;
                }

                // Erase Keys
                QList<QPair<RulesCategories, qsizetype>> tempMapPairs;
                for(ConfigKeyIter tempIter = beginIter; tempIter != endIter; tempIter++){
                    tempMapPairs.append({tempIter.key().second, tempIter.value()});
                }
                endIter = newConfigMap.erase(beginIter, endIter);

                // Setup new keys
                // 1. Add new Keys:
                ConfigKeyIter newBeginIter = newConfigMap.insert({newName, tempMapPairs.first().first}, tempMapPairs.first().second);
                ConfigKeyIter tempIter = newBeginIter;
                for(decltype(tempMapPairs)::Iterator tempPairsIter = tempMapPairs.begin()+1; tempPairsIter != tempMapPairs.end(); tempPairsIter++){
                    tempIter = newConfigMap.insert({newName, tempPairsIter->first}, tempPairsIter->second);
                }
                // <name, Settings>, - Value - No User Iteraction as Default = 0
                tempIter++;
                ConfigKeyIter newEndIter = tempIter;
                bool changed = true;
                qsizetype diff = 0;

                // Up or down
                if(tempIter != newConfigMap.end()){
                    if(abs(tempIter.value()) - numbOfRules != position){
                        if(abs(tempIter.value()) < position){ // Up
                            diff = position - abs(tempIter.value());
                            tempIter = newBeginIter;
                            while(tempIter != newEndIter){
                                tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                                tempIter++;
                            }
                            while(tempIter != endIter){
                                tempIter.value() += (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                tempIter++;
                            }
                        }else{ // Down
                            newBeginIter--;
                            endIter--;
                            diff = position - abs(tempIter.value()) + numbOfRules;
                            tempIter = newEndIter;
                            tempIter--;
                            while(tempIter != newBeginIter){
                                tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                                tempIter--;
                            }
                            while(tempIter != endIter){
                                tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                tempIter--;
                            }
                        }
                    }else{
                        // No change
                        changed = false;
                    }
                }else{
                    if(newConfigMap.size() - numbOfRules != position){ // Up impossible? Only Down
                        newBeginIter--;
                        endIter--;
                        diff = position - newConfigMap.size() + numbOfRules;
                        tempIter = newEndIter;
                        tempIter--;
                        while(tempIter != newBeginIter){
                            tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                            tempIter--;
                        }
                        while(tempIter != endIter){
                            tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                            tempIter--;
                        }
                        newBeginIter++;
                    }else{// No change
                        changed = false;
                    }
                }

                // If Rules to move
                if(changed){
                    QList<void*> tempRules;
                    if(numbOfRules != 0){   // If rules to move
                        tempRules = newRules.mid(position, numbOfRules);
                        newRules.remove(position, numbOfRules);
                        QList<void*>::Iterator tempRulesIter = tempRules.begin();
                        QList<void*>::Iterator newRulesIter;
                        newRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                        newRulesIter = newRules.begin() + newBeginIter.value();
                        for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                            *newRulesIter = *tempRulesIter;
                        }
                    }
                }
            }
        }

        void moveAllNewRules(QString name, qsizetype oldIndex, qsizetype newIndex){
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;
            beginIter = newConfigMap.find({name, static_cast<RulesCategories>(oldIndex)});
            if(beginIter != newConfigMap.end()){
                endIter = ++beginIter;
                beginIter--;
                qsizetype position = abs(beginIter.value());
                qsizetype numbOfRules = 0;
                if(endIter != newConfigMap.end()){
                    numbOfRules = abs(endIter.value()) - position;
                }else{
                    numbOfRules = newRules.size() - position;
                }

                // Erase Keys
                qsizetype tempPosition = beginIter.value();
                endIter = newConfigMap.erase(beginIter);

                // Setup new keys
                // 1. Add new Keys:
                ConfigKeyIter newBeginIter = newConfigMap.insert({name, static_cast<RulesCategories>(newIndex)}, tempPosition);
                ConfigKeyIter tempIter = newBeginIter;
                tempIter++;

                ConfigKeyIter newEndIter = tempIter;
                bool changed = true;
                qsizetype diff = 0;

                // Up or down
                if(tempIter != newConfigMap.end()){
                    if(abs(tempIter.value()) - numbOfRules != position){
                        if(abs(tempIter.value()) < position){ // Up
                            diff = position - abs(tempIter.value());
                            tempIter = newBeginIter;
                            tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                            tempIter++;
                            while(tempIter != endIter){
                                tempIter.value() += (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                tempIter++;
                            }
                        }else{ // Down
                            newBeginIter--;
                            endIter--;
                            diff = position - abs(tempIter.value()) + numbOfRules;
                            tempIter--;
                            tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                            tempIter--;
                            while(tempIter != endIter){
                                tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                tempIter--;
                            }
                        }
                    }else{
                        // No change
                        changed = false;
                    }
                }else{
                    if(newConfigMap.size() - numbOfRules != position){ // Up impossible? Only Down
                        newBeginIter--;
                        endIter--;
                        diff = position - newConfigMap.size() + numbOfRules;
                        tempIter--;
                        tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                        tempIter--;
                        while(tempIter != endIter){
                            tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                            tempIter--;
                        }
                        newBeginIter++;
                    }else{// No change
                        changed = false;
                    }
                }

                // If Rules to move
                if(changed){
                    QList<void*> tempRules;
                    if(numbOfRules != 0){   // If rules to move
                        tempRules = newRules.mid(position, numbOfRules);
                        newRules.remove(position, numbOfRules);
                        QList<void*>::Iterator tempRulesIter = tempRules.begin();
                        QList<void*>::Iterator newRulesIter;
                        newRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                        newRulesIter = newRules.begin() + newBeginIter.value();
                        for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                            *newRulesIter = *tempRulesIter;
                        }
                    }
                }
            }
        }
/*
        void moveSavedProcedureToNewRules(QString name){
            using Config = decltype(configMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;
            beginIter = newConfigMap.lowerBound({oldName, RulesCategories::OnEndOfCall});
            if(beginIter != newConfigMap.end() and beginIter.key().first == oldName){
                endIter = newConfigMap.lowerBound({oldName, static_cast<RulesCategories>(LONG_LONG_MAX)});
                qsizetype position = abs(beginIter.value());
                qsizetype numbOfRules = 0;
                if(endIter != newConfigMap.end()){
                    numbOfRules = abs(endIter.value()) - position;
                }else{
                    numbOfRules = newRules.size() - position;
                }

                // Erase Keys
                QList<QPair<RulesCategories, qsizetype>> tempMapPairs;
                for(ConfigKeyIter tempIter = beginIter; tempIter != endIter; tempIter++){
                    tempMapPairs.append({tempIter.key().second, tempIter.value()});
                }
                endIter = newConfigMap.erase(beginIter, endIter);

                // Setup new keys
                // 1. Add new Keys:
                ConfigKeyIter newBeginIter = newConfigMap.insert({newName, tempMapPairs.first().first}, tempMapPairs.first().second);
                ConfigKeyIter tempIter = newBeginIter;
                for(decltype(tempMapPairs)::Iterator tempPairsIter = tempMapPairs.begin()+1; tempPairsIter != tempMapPairs.end(); tempPairsIter++){
                    tempIter = newConfigMap.insert({newName, tempPairsIter->first}, tempPairsIter->second);
                }
                // <name, Settings>, - Value - No User Iteraction as Default = 0
                tempIter++;
                ConfigKeyIter newEndIter = tempIter;
                bool changed = true;
                qsizetype diff = 0;

                // Up or down
                if(tempIter != newConfigMap.end()){
                    if(abs(tempIter.value()) - numbOfRules != position){
                        if(abs(tempIter.value()) < position){ // Up
                            diff = position - abs(tempIter.value());
                            tempIter = newBeginIter;
                            while(tempIter != newEndIter){
                                tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                                tempIter++;
                            }
                            while(tempIter != endIter){
                                tempIter.value() += (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                tempIter++;
                            }
                        }else{ // Down
                            newBeginIter--;
                            endIter--;
                            diff = position - abs(tempIter.value()) + numbOfRules;
                            tempIter = newEndIter;
                            tempIter--;
                            while(tempIter != newBeginIter){
                                tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                                tempIter--;
                            }
                            while(tempIter != endIter){
                                tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                tempIter--;
                            }
                        }
                    }else{
                        // No change
                        changed = false;
                    }
                }else{
                    if(newConfigMap.size() - numbOfRules != position){ // Up impossible? Only Down
                        newBeginIter--;
                        endIter--;
                        diff = position - newConfigMap.size() + numbOfRules;
                        tempIter = newEndIter;
                        tempIter--;
                        while(tempIter != newBeginIter){
                            tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                            tempIter--;
                        }
                        while(tempIter != endIter){
                            tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                            tempIter--;
                        }
                        newBeginIter++;
                    }else{// No change
                        changed = false;
                    }
                }

                // If Rules to move
                if(changed){
                    QList<void*> tempRules;
                    if(numbOfRules != 0){   // If rules to move
                        tempRules = newRules.mid(position, numbOfRules);
                        newRules.remove(position, numbOfRules);
                        QList<void*>::Iterator tempRulesIter = tempRules.begin();
                        QList<void*>::Iterator newRulesIter;
                        newRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                        newRulesIter = newRules.begin() + newBeginIter.value();
                        for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                            *newRulesIter = *tempRulesIter;
                        }
                    }
                }
            }
        }
        */
        bool editProcedureName(QString oldName, QString newName){
            // NewName verification
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;
            /*if(editted.contains({newName, RulesCategories::OnEndOfCall}) or
                    ((beginIter = configMap.find({newName, RulesCategories::UndefinedArgument})) != configMap.end() and beginIter.value() >= 0
                     and (static_cast<void>(beginIter--), beginIter.value() >= 0) ) or
                    ((beginIter = newConfigMap.find({newName, RulesCategories::OnEndOfCall})) != newConfigMap.end())  ) //
            {
                return false;
            }*/
            if((beginIter = newConfigMap.find({newName, RulesCategories::OnEndOfCall})) != newConfigMap.end() and
                    (static_cast<void>(beginIter--), beginIter.value() >= 0) )
            {
                return false;
            }

            // Remove all removed new index of new name procedure
            // 1. Find keys:
            // iter1 = <name, Settings>, - Value - Absolute rule position
            clearAllNewRules(newName);
            // 1. Old name is:
            // 1.1. Edited name - Change key in newNames -> New Procedures change old name to newName
            // 1.2. Non-edited in config - New key in newNames -> Negative value for Undefined in Config for oldName
            // -> NewConfig oldName to newName
            // 1.3. Non in config, only new - Change name in new Config
            /*using EditNameIter = decltype(editted)::Iterator;
            EditNameIter editNameIter = editted.find({oldName, RulesCategories::OnEndOfCall });
            if(editNameIter != editted.end()){
                // - 1
                QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator tempV = editNameIter.value();
                editted.erase(editNameIter);
                editNameIter = editted.insert({newName, RulesCategories::OnEndOfCall}, tempV);
                // - 2 - Move all rules               
            }else{
                ConfigKeyIter beginIter = configMap.find({oldName, RulesCategories::UndefinedArgument});
                if(beginIter != configMap.end()){
                    // 1 .
                    editted.insert({newName, RulesCategories::OnEndOfCall}, beginIter);
                    // 2.
                    beginIter.value() = abs(beginIter.value());
                }
            }*/
            moveAllNewRules(oldName, newName);

            return true;
        }

        bool removeProcedure(QString name){
            // 1. Old name is:
            // 1.1. Edited name - Get iter from newNames -> Remove from newNames -> Change in config -> Remove from newRules
            // 1.2. Non-edited in config - Negative value for OnEndOfCall in Config
            // -> If Config is removed, new rules dont require Removed flag too, save it to keep removed indexes
            // 1.3. Non in config, only new - Just Remove
            // Preverification:
            // 1. Find key <name, OnEndOfCall> in config
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            using EditNameIter = QMap<QString, QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator>::Iterator;
            ConfigKeyIter configKeyIter;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;
            /*EditNameIter editNameIter = newNames.find(name);
            if(editNameIter != newNames.end()){
                // - 1
                QString removedName = editNameIter.key();
                configKeyIter = editNameIter.value();
                // - 2
                newNames.erase(editNameIter);
                // - 3
                configKeyIter.value() = abs(configKeyIter.value()); // Remove editted name flag
                configKeyIter--;
                configKeyIter.value() = -(configKeyIter.value() + 1); // Set Removed
                // - 4
                beginIter = newConfigMap.lowerBound({removedName, RulesCategories::OnEndOfCall});
                if(beginIter != newConfigMap.end() and beginIter.key().first == removedName){
                    endIter = newConfigMap.lowerBound({removedName, static_cast<RulesCategories>(LONG_LONG_MAX)});
                    qsizetype position = abs(beginIter.value());
                    qsizetype numbOfRules = 0;
                    if(endIter != newConfigMap.end()){
                        numbOfRules = abs(endIter.value()) - position;
                    }else{
                        numbOfRules = newRules.size() - position;
                    }
                    newRules.remove(position, numbOfRules);

                    // Erase Keys
                    QList<QPair<RulesCategories, qsizetype>> tempMapPairs;
                    for(ConfigKeyIter tempIter = beginIter; tempIter != endIter; tempIter++){
                        tempMapPairs.append({tempIter.key().second, tempIter.value()});
                    }
                    beginIter = newConfigMap.erase(beginIter, endIter);
                    // Update positions in map
                    while(beginIter != newConfigMap.end()){
                        beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                        beginIter--;
                    }
                }
            }else{
                configKeyIter = configMap.find({name, RulesCategories::OnEndOfCall});
                if(configKeyIter != configMap.end()){
                    // 1 .
                    configKeyIter.value() = -(configKeyIter.value() + 1); // Set Removed
                }else{*/
                    configKeyIter = newConfigMap.find({name, RulesCategories::OnEndOfCall});
                    if(configKeyIter != newConfigMap.end()){
                        configKeyIter.value() = -(configKeyIter.value() + 1); // Set Removed
                    }
                /*}
            }
*/
            return true;
        }

        bool addIndex(QString name, qsizetype index){
            // Preverification:
            // 1. Find key <name, index> in config
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            using EditIndexIter = QMap<QPair<QString, qsizetype>, QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator>::Iterator;
            //EditIndexIter editIndexIter = newNames.find(name);
            ConfigKeyIter configKeyIter;
            QString oldName;
            bool found = false;
            /*if(editNameIter != newNames.end()){
                oldName = editNameIter.value().key().first;
                configKeyIter = editNameIter.value();
            }else{
                oldName = name;
            }
            configKeyIter = configMap.find({oldName, static_cast<RulesCategories>(index)});
            if(configKeyIter != configMap.end()){
                // TRUE (Found):
                found = true;
                if(configKeyIter.value() < 0){
                    // Removed - restore
                    configKeyIter.value() = abs(configKeyIter.value());
                }else{
                    return false; // Duplicated
                }
            }*/
            configKeyIter = newConfigMap.find({name, static_cast<RulesCategories>(index)});
            if(configKeyIter != newConfigMap.end()){
                found = true;
                if(configKeyIter.value() < 0){
                    // Removed - restore
                    configKeyIter.value() = abs(configKeyIter.value());
                }else{
                    return false; // Duplicated
                }
            }
            if(not found)
            {
                configKeyIter = newConfigMap.insert({name, static_cast<RulesCategories>(index)}, 0);
                configKeyIter++;
                qsizetype value;
                if(configKeyIter != newConfigMap.end()){
                    value = configKeyIter.value();
                }else{
                    value = newRules.size();
                }

                configKeyIter--;
                configKeyIter.value() = abs(value);
            }
            return true;
        }

        bool editIndex(QString name, qsizetype oldIndex, qsizetype newIndex){
            // 1.1. Edited index - Change key in editted
            // 1.2. config without newRules priority (no index in newRules) - New key in newNames -> Negative value for index in Config
            // 1.21. config with newRules priority - Change index in new Config if new index not in newRules or new index not in editted or removed index in config
            // -> Config index change to removed
            // 1.3. Non in config, only new - Change index in new Config if new index not in newRules or new index not in editted or (removed index in config - set not removed - new rules priority here)
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter beginIter;
            ConfigKeyIter endIter;


            // Check if procedure name is not editted
/*
            if(editted.contains({name, static_cast<RulesCategories>(newIndex)}) or
                    ((beginIter = configMap.find({name, static_cast<RulesCategories>(newIndex)})) != configMap.end() and beginIter.value() >= 0) or
                    ((beginIter = newConfigMap.find({name, static_cast<RulesCategories>(newIndex)})) != newConfigMap.end() and beginIter.value() >= 0)  ) //
            {
                return false;
            }*/
            if((beginIter = newConfigMap.find({name, static_cast<RulesCategories>(newIndex)})) != newConfigMap.end() and
                    (static_cast<void>(beginIter--), beginIter.value() >= 0) )
            {
                return false;
            }

            /*using EditNameIter = decltype(editted)::Iterator;
            EditNameIter editNameIter = editted.find({name, static_cast<RulesCategories>(oldIndex)});
            if(editNameIter != editted.end()){
                // - 1
                QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator tempV = editNameIter.value();
                editted.erase(editNameIter);
                editNameIter = editted.insert({name, static_cast<RulesCategories>(newIndex)}, tempV);
                // - 2 - Move all rules
                beginIter = newConfigMap.find({name, static_cast<RulesCategories>(newIndex)});
                if(beginIter != newConfigMap.end()){ // Any rules for key
                    ConfigKeyIter savedConfigIter = beginIter;
                    // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
                    beginIter++;
                    endIter =beginIter;
                    beginIter--;
                    // 2. Calculate number of rules and remove it
                    qsizetype numbOfRules = 0;
                    if(endIter != newConfigMap.end()){
                        numbOfRules = abs(endIter.value()) - abs(beginIter.value());
                    }else{
                        numbOfRules = newRules.size() - abs(beginIter.value());
                    }
                    if(numbOfRules != 0){
                        newRules.remove(abs(beginIter.value()), numbOfRules);
                        // 3. Recalculate all absolute values of rules after this procedure
                        while(beginIter != newConfigMap.end()){
                            beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                            beginIter++;
                        }
                    }
                    // 4. Erase keys
                    newConfigMap.erase(savedConfigIter);
                }
            }else{
                ConfigKeyIter configIter = configMap.find({name, static_cast<RulesCategories>(oldIndex)});
                ConfigKeyIter newConfigIter;
                if(configIter != configMap.end()){ // In Config*/



                    // Cannot be removed - I could not call edit for that index
                    //newConfigIter = newConfigMap.find({name, static_cast<RulesCategories>(oldIndex)});
                    //if( newConfigIter != newConfigMap.end()){   // In Config with new Rules Priority
                        // Clear rules created for new index
                        clearAllNewRules(name, newIndex);
                        // Config New index - Set in not removed state
                        //configIter = configMap.find({name, static_cast<RulesCategories>(newIndex)});
                        //configIter.value() = abs(configIter.value());
                        // New Config new index - Move old rules to newIndex ----------------------------------------------------
                        moveAllNewRules(name, oldIndex, newIndex);
                    /*        // -------------------------------------------------------------------
                    }else{ // No new rules priority - Move to Edit mode
                        editted.insert({name, static_cast<RulesCategories>(newIndex)}, configIter);
                        configIter.value() = -(configIter.value() + 1);// Set negative value to inform about edit mode
                    }
                }else{ // No config - only new config rules
                    newConfigIter = newConfigMap.find({name, static_cast<RulesCategories>(oldIndex)});
                    if( newConfigIter != newConfigMap.end()){   // only new config rules
                        // Clear rules created for new index
                        beginIter = newConfigMap.find({name, static_cast<RulesCategories>(newIndex)});
                        if(beginIter != newConfigMap.end()){ // Any rules for key
                            ConfigKeyIter savedConfigIter = beginIter;
                            // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
                            beginIter++;
                            endIter =beginIter;
                            beginIter--;
                            // 2. Calculate number of rules and remove it
                            qsizetype numbOfRules = 0;
                            if(endIter != newConfigMap.end()){
                                numbOfRules = abs(endIter.value()) - abs(beginIter.value());
                            }else{
                                numbOfRules = newRules.size() - abs(beginIter.value());
                            }
                            if(numbOfRules != 0){
                                newRules.remove(abs(beginIter.value()), numbOfRules);
                                // 3. Recalculate all absolute values of rules after this procedure
                                while(beginIter != newConfigMap.end()){
                                    beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                                    beginIter++;
                                }
                            }
                            // 4. Erase keys
                            newConfigMap.erase(savedConfigIter);
                        }
                        // New Config new index - Move old rules to newIndex ----------------------------------------------------
                        beginIter = newConfigMap.find({name, static_cast<RulesCategories>(oldIndex)});
                        if(beginIter != newConfigMap.end()){
                            endIter = ++beginIter;
                            beginIter--;
                            qsizetype position = abs(beginIter.value());
                            qsizetype numbOfRules = 0;
                            if(endIter != newConfigMap.end()){
                                numbOfRules = abs(endIter.value()) - position;
                            }else{
                                numbOfRules = newRules.size() - position;
                            }

                            // Erase Keys
                            qsizetype tempPosition = beginIter.value();
                            endIter = newConfigMap.erase(beginIter);

                            // Setup new keys
                            // 1. Add new Keys:
                            ConfigKeyIter newBeginIter = newConfigMap.insert({name, static_cast<RulesCategories>(newIndex)}, tempPosition);
                            ConfigKeyIter tempIter = newBeginIter;
                            tempIter++;

                            ConfigKeyIter newEndIter = tempIter;
                            bool changed = true;
                            qsizetype diff = 0;

                            // Up or down
                            if(tempIter != newConfigMap.end()){
                                if(abs(tempIter.value()) - numbOfRules != position){
                                    if(abs(tempIter.value()) < position){ // Up
                                        diff = position - abs(tempIter.value());
                                        tempIter = newBeginIter;
                                        tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                                        tempIter++;
                                        while(tempIter != endIter){
                                            tempIter.value() += (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                            tempIter++;
                                        }
                                    }else{ // Down
                                        newBeginIter--;
                                        endIter--;
                                        diff = position - abs(tempIter.value()) + numbOfRules;
                                        tempIter--;
                                        tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                                        tempIter--;
                                        while(tempIter != endIter){
                                            tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                            tempIter--;
                                        }
                                    }
                                }else{
                                    // No change
                                    changed = false;
                                }
                            }else{
                                if(newConfigMap.size() - numbOfRules != position){ // Up impossible? Only Down
                                    newBeginIter--;
                                    endIter--;
                                    diff = position - newConfigMap.size() + numbOfRules;
                                    tempIter--;
                                    tempIter.value() -= (tempIter.value() < 0)? -diff : diff;
                                    tempIter--;
                                    while(tempIter != endIter){
                                        tempIter.value() -= (tempIter.value() < 0)? -numbOfRules : numbOfRules;
                                        tempIter--;
                                    }
                                    newBeginIter++;
                                }else{// No change
                                    changed = false;
                                }
                            }

                            // If Rules to move
                            if(changed){
                                QList<void*> tempRules;
                                if(numbOfRules != 0){   // If rules to move
                                    tempRules = newRules.mid(position, numbOfRules);
                                    newRules.remove(position, numbOfRules);
                                    QList<void*>::Iterator tempRulesIter = tempRules.begin();
                                    QList<void*>::Iterator newRulesIter;
                                    newRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                                    newRulesIter = newRules.begin() + newBeginIter.value();
                                    for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                                        *newRulesIter = *tempRulesIter;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            */

            return true;
        }

        void removeIndex(QString name, qsizetype index){
            // Preverification:
            // 1. Find key <name, index> in config
            using Config = decltype(newConfigMap);
            using ConfigKeyIter = Config::Iterator;
            ConfigKeyIter configKeyIter;
/*
            ConfigKeyIter configKeyIter = configMap.find({name, static_cast<RulesCategories>(index)});
            if(configKeyIter != configMap.end()){
                // TRUE (Found):
                // iter - begin = index -> removed[index] = true
                // RETURN
            }else{
                // FALSE (Not Found):

            }*/
            // Main:
            // 1. Find keys:
            configKeyIter = newConfigMap.find({name, static_cast<RulesCategories>(index)});

            if(configKeyIter != newConfigMap.end()){
                configKeyIter.value() = -(configKeyIter.value() + 1);
            }
        }

    protected:

        // Removed Rules mask
        // Created based on number of rules from configuration - it will never change til change of configuration
        //using RemovedIndexes = QBitArray; // Only for Saved Config
        using UserInterationProcedures = QBitArray; // Only for Saved Config
        //using RemovedRules = QBitArray; // Maybe int to control number of removed rules
        //using NewIndexes = QList<uint>; // Only information about new indexes - there are sorted
        //using NewRules = QList<Rule*>;
        // Probably i can calculate position of rules even for new indexes
        // New Index > Old Index -> Right position

        struct ProcedureRuleCategoryKey : public QPair<QString, RulesCategories>{
        public:
            using QPair<QString, RulesCategories>::QPair;
            friend bool operator<(const ProcedureRuleCategoryKey& lhs, const ProcedureRuleCategoryKey& rhs){
                return (lhs.second < rhs.second and rhs.first == lhs.first)
                        or lhs.first < rhs.first;
            }
        };

        // Local changes
        QList<void*> newRules;
        //RemovedRules removedRules; // Only for Saved Config
        //RemovedIndexes removedIndexes; // Only for Saved Config - Removed = -index
        //QMap<QPair<QString, RulesCategories>, QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator> editted;
        //QMap<QString, int> newSettings;
        //UserInterationProcedures userInterationProceduresChanged; // Only for Saved Config
        QMap<ProcedureRuleCategoryKey, qsizetype> newConfigMap;// Pos/Size Pair
        //NewIndexes newIndexes;

        // Const config infos - in config
        //int numbOfProcedures; - Can be derived from userInteractionProcedures size
        //QMap<ProcedureRuleCategoryKey, qsizetype> configMap;// Pos/Size Pair

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
