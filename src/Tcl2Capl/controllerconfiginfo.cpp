#include"controllerconfiginfo.hpp"
/*
ControllerConfigInfo::ControllerConfigInfo(ConfigPath path, RegisterFileModificationTime modificationTime, Tcl2CaplControllerConfig config)
    : configPath(path), modificationTime(modificationTime), _controllerConfig(config)
{

}
*/
ControllerConfigInfo::ControllerConfigInfo()

{
    newDefaultProcedureMap.insert(RulesCategories::OnEndOfCall, 0);
    newDefaultProcedureMap.insert(RulesCategories::UndefinedArgument, 0);
}

bool ControllerConfigInfo::addProcedure(QString name){
    qDebug() << "addProcedure begin" << newProceduresMap;
    // Preverification:
    /*if(editted.contains({name, RulesCategories::OnEndOfCall})){
        return false;
    }
    */
    // 1. Find key <name, OnEndOfCall> in config
    using Config = decltype(newProceduresMap);
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
    configKeyIter = newProceduresMap.find({name, RulesCategories::OnEndOfCall}); //
    if(configKeyIter == newProceduresMap.end()){
        // 1. Add new Keys:
        // iter1 = <name, OnEndOfCall>, - Value - Absolute rule position
        configKeyIter = newProceduresMap.insert({name, RulesCategories::OnEndOfCall}, 0);
        // iter2 = <name, UndefinedArgument> -  Value - Offset of previous rule category = 0
        ConfigKeyIter configUndefinedKeyIter = newProceduresMap.insert({name, RulesCategories::UndefinedArgument}, 0);
        qDebug() << configUndefinedKeyIter.key();
        // 2. Specify values for keys:
        // 2.1. Check if iter2 + 1 is valid key
        configUndefinedKeyIter++; // To +1
        if(configUndefinedKeyIter != newProceduresMap.end()){
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
    }else{
        if(configKeyIter.value() < 0){
            // Unlock and read - unlock by -(value + 1)
            configKeyIter.value() = abs(configKeyIter.value());
            // Restore
            countUp_RestoredProcedure();
            return true;
        }else{
            qDebug() << "addProcedure end" << newProceduresMap;
            return false;
        }
    }
    countUp_NewProcedure();
    qDebug() << "addProcedure end" << newProceduresMap;
    return true;
}

void ControllerConfigInfo::clearAllNewRules(QString procedureName){
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "clearAllNewRules begin" << newProceduresMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newProceduresMap.lowerBound({procedureName, static_cast<RulesCategories>(LONG_LONG_MIN)});
    if(beginIter != newProceduresMap.end() and beginIter.key().first == procedureName){ // Any keys exist
        ConfigKeyIter savedConfigIter = beginIter;
        // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
        endIter = newProceduresMap.lowerBound({procedureName, static_cast<RulesCategories>(LONG_LONG_MAX)});
        // 2. Calculate number of rules and remove it
        qsizetype numbOfRules = 0;
        qsizetype position = abs(beginIter.value());
        if(endIter != newProceduresMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newRules.size() - position;
        }
        NewRules::Iterator tempRuleIter = newRules.begin() + position;
        NewRules::Iterator endRuleIter = tempRuleIter + numbOfRules;

        for( ; tempRuleIter < endRuleIter; tempRuleIter++)
            delete *tempRuleIter;

        newRules.remove(position, numbOfRules);
        // 3. Recalculate all absolute values of rules after this procedure
        while(beginIter != newProceduresMap.end()){
            beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
            beginIter++;
        }
        // 4. Erase keys
        newProceduresMap.erase(savedConfigIter, endIter);

        countDown_RemoveHiddenProcedure();
    }
    qDebug() << "clearAllNewRules end" << newProceduresMap;
}

void ControllerConfigInfo::clearAllNewRules(qsizetype index){
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "clearAllNewRules begin" << newProceduresMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(index));
    if(beginIter != newDefaultProcedureMap.end()){ // Any rules for key
        ConfigKeyIter savedConfigIter = beginIter;
        // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
        beginIter++;
        endIter =beginIter;
        beginIter--;
        // 2. Calculate number of rules and remove it
        qsizetype numbOfRules = 0;
        qsizetype position = abs(beginIter.value());
        if(endIter != newDefaultProcedureMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newDefaultProcedureRules.size() - position;
        }
        if(numbOfRules != 0){
            NewRules::Iterator tempRuleIter = newDefaultProcedureRules.begin() + position;
            NewRules::Iterator endRuleIter = tempRuleIter + numbOfRules;

            for( ; tempRuleIter < endRuleIter; tempRuleIter++)
                delete *tempRuleIter;

            newDefaultProcedureRules.remove(position, numbOfRules);
            // 3. Recalculate all absolute values of rules after this procedure
            while(beginIter != newDefaultProcedureMap.end()){
                beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                beginIter++;
            }
        }
        // 4. Erase keys
        newDefaultProcedureMap.erase(savedConfigIter);
    }
    qDebug() << "clearAllNewRules end" << newProceduresMap;
}

void ControllerConfigInfo::clearAllNewRules(QString procedureName, qsizetype index){
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "clearAllNewRules begin" << newProceduresMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newProceduresMap.find({procedureName, static_cast<RulesCategories>(index)});
    if(beginIter != newProceduresMap.end()){ // Any rules for key
        ConfigKeyIter savedConfigIter = beginIter;
        // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
        beginIter++;
        endIter = beginIter;
        beginIter--;
        // 2. Calculate number of rules and remove it
        qsizetype numbOfRules = 0;
        qsizetype position = abs(beginIter.value());
        if(endIter != newProceduresMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newRules.size() - position;
        }
        if(numbOfRules != 0){
            NewRules::Iterator tempRuleIter = newRules.begin() + position;
            NewRules::Iterator endRuleIter = tempRuleIter + numbOfRules;

            for( ; tempRuleIter < endRuleIter; tempRuleIter++)
                delete *tempRuleIter;

            newRules.remove(position, numbOfRules);
            // 3. Recalculate all absolute values of rules after this procedure
            while(beginIter != newProceduresMap.end()){
                beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                beginIter++;
            }
        }
        // 4. Erase keys
        newProceduresMap.erase(savedConfigIter);
    }
    qDebug() << "clearAllNewRules end" << newProceduresMap;
}

void ControllerConfigInfo::moveAllNewRules(QString oldName, QString newName){
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "moveAllNewRules begin" << newProceduresMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newProceduresMap.find({oldName, RulesCategories::OnEndOfCall});
    if(beginIter != newProceduresMap.end() and beginIter.key().first == oldName){
        endIter = newProceduresMap.upperBound({oldName, static_cast<RulesCategories>(LONG_LONG_MAX)});
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newProceduresMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newRules.size() - position;
        }

        // Erase Keys
        QList<QPair<RulesCategories, qsizetype>> tempMapPairs;
        for(ConfigKeyIter tempIter = beginIter; tempIter != endIter; tempIter++){
            tempMapPairs.append({tempIter.key().second, tempIter.value()});
        }
        endIter = newProceduresMap.erase(beginIter, endIter);

        // Setup new keys
        // 1. Add new Keys:
        ConfigKeyIter newBeginIter = newProceduresMap.insert({newName, tempMapPairs.first().first}, tempMapPairs.first().second);
        ConfigKeyIter tempIter = newBeginIter;
        for(decltype(tempMapPairs)::Iterator tempPairsIter = tempMapPairs.begin()+1; tempPairsIter != tempMapPairs.end(); tempPairsIter++){
            tempIter = newProceduresMap.insert({newName, tempPairsIter->first}, tempPairsIter->second);
        }
        // <name, Settings>, - Value - No User Iteraction as Default = 0
        tempIter++;
        ConfigKeyIter newEndIter = tempIter;
        bool changed = true;
        qsizetype diff = 0;

        // Up or down
        if(tempIter != newProceduresMap.end()){
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
            if(newRules.size() - numbOfRules != position){ // Up impossible? Only Down
                newBeginIter--;
                endIter--;
                diff = position - newRules.size() + numbOfRules;
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
            NewRules tempRules;
            if(numbOfRules != 0){   // If rules to move
                tempRules = newRules.mid(position, numbOfRules);
                newRules.remove(position, numbOfRules);
                NewRules::Iterator tempRulesIter = tempRules.begin();
                NewRules::Iterator newRulesIter;
                newRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                newRulesIter = newRules.begin() + newBeginIter.value();
                for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                    *newRulesIter = *tempRulesIter;
                }
            }
        }
    }
    qDebug() << "moveAllNewRules end" << newProceduresMap;
}

void ControllerConfigInfo::moveAllNewRules(QString name, qsizetype oldIndex, qsizetype newIndex){
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "moveAllNewRules begin" << newProceduresMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newProceduresMap.find({name, static_cast<RulesCategories>(oldIndex)});
    if(beginIter != newProceduresMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newProceduresMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newRules.size() - position;
        }

        // Erase Keys
        qsizetype tempPosition = beginIter.value();
        endIter = newProceduresMap.erase(beginIter);

        // Setup new keys
        // 1. Add new Keys:
        ConfigKeyIter newBeginIter = newProceduresMap.insert({name, static_cast<RulesCategories>(newIndex)}, tempPosition);
        ConfigKeyIter tempIter = newBeginIter;
        tempIter++;

        ConfigKeyIter newEndIter = tempIter;
        bool changed = true;
        qsizetype diff = 0;

        // Up or down
        if(tempIter != newProceduresMap.end()){
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
            if(newProceduresMap.size() - numbOfRules != position){ // Up impossible? Only Down
                newBeginIter--;
                endIter--;
                diff = position - newProceduresMap.size() + numbOfRules;
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
            NewRules tempRules;
            if(numbOfRules != 0){   // If rules to move
                tempRules = newRules.mid(position, numbOfRules);
                newRules.remove(position, numbOfRules);
                NewRules::Iterator tempRulesIter = tempRules.begin();
                NewRules::Iterator newRulesIter;
                newRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                newRulesIter = newRules.begin() + newBeginIter.value();
                for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                    *newRulesIter = *tempRulesIter;
                }
            }
        }
    }
    qDebug() << "moveAllNewRules end" << newProceduresMap;
}

void ControllerConfigInfo::moveAllNewRules( qsizetype oldIndex, qsizetype newIndex){
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "moveAllNewRules begin" << newProceduresMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(oldIndex));
    if(beginIter != newDefaultProcedureMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newDefaultProcedureMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newDefaultProcedureRules.size() - position;
        }

        // Erase Keys
        qsizetype tempPosition = beginIter.value();
        endIter = newDefaultProcedureMap.erase(beginIter);

        // Setup new keys
        // 1. Add new Keys:
        ConfigKeyIter newBeginIter = newDefaultProcedureMap.insert(static_cast<RulesCategories>(newIndex), tempPosition);
        ConfigKeyIter tempIter = newBeginIter;
        tempIter++;

        ConfigKeyIter newEndIter = tempIter;
        bool changed = true;
        qsizetype diff = 0;

        // Up or down
        if(tempIter != newDefaultProcedureMap.end()){
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
            if(newProceduresMap.size() - numbOfRules != position){ // Up impossible? Only Down
                newBeginIter--;
                endIter--;
                diff = position - newProceduresMap.size() + numbOfRules;
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
            NewRules tempRules;
            if(numbOfRules != 0){   // If rules to move
                tempRules = newDefaultProcedureRules.mid(position, numbOfRules);
                newDefaultProcedureRules.remove(position, numbOfRules);
                NewRules::Iterator tempRulesIter = tempRules.begin();
                NewRules::Iterator newRulesIter;
                newDefaultProcedureRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                newRulesIter = newDefaultProcedureRules.begin() + newBeginIter.value();
                for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                    *newRulesIter = *tempRulesIter;
                }
            }
        }
    }
    qDebug() << "moveAllNewRules end" << newProceduresMap;
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
bool ControllerConfigInfo::editProcedureName(QString oldName, QString newName){
    // NewName verification
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "editProcedureName begin" << newProceduresMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    /*if(editted.contains({newName, RulesCategories::OnEndOfCall}) or
            ((beginIter = configMap.find({newName, RulesCategories::UndefinedArgument})) != configMap.end() and beginIter.value() >= 0
             and (static_cast<void>(beginIter--), beginIter.value() >= 0) ) or
            ((beginIter = newConfigMap.find({newName, RulesCategories::OnEndOfCall})) != newConfigMap.end())  ) //
    {
        return false;
    }*/
    qDebug() << oldName << newName;
    if(((beginIter = newProceduresMap.find({newName, RulesCategories::OnEndOfCall})) != newProceduresMap.end() and
            ( beginIter.value() >= 0)) )
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
    qDebug() << "editProcedureName end" << newProceduresMap;

    return true;
}

bool ControllerConfigInfo::removeProcedure(QString name){
    // 1. Old name is:
    // 1.1. Edited name - Get iter from newNames -> Remove from newNames -> Change in config -> Remove from newRules
    // 1.2. Non-edited in config - Negative value for OnEndOfCall in Config
    // -> If Config is removed, new rules dont require Removed flag too, save it to keep removed indexes
    // 1.3. Non in config, only new - Just Remove
    // Preverification:
    // 1. Find key <name, OnEndOfCall> in config
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    using EditNameIter = QMap<QString, QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator>::Iterator;
    qDebug() << "removeProcedure begin" << newProceduresMap;
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
        configKeyIter = newProceduresMap.find({name, RulesCategories::OnEndOfCall});
        if(configKeyIter != newProceduresMap.end()){
            configKeyIter.value() = -(configKeyIter.value() + 1); // Set Removed
            countDown_HideProcedure();
        }
        /*}
    }
*/
    qDebug() << "removeProcedure end" << newProceduresMap;
    return true;
}

bool ControllerConfigInfo::addIndex(QString name, qsizetype index){
    // Preverification:
    // 1. Find key <name, index> in config
    qDebug() << "addIndex begin" << newProceduresMap;
    using Config = decltype(newProceduresMap);
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
    configKeyIter = newProceduresMap.find({name, static_cast<RulesCategories>(index)});
    if(configKeyIter != newProceduresMap.end()){
        found = true;
        if(configKeyIter.value() < 0){
            // Removed - restore
            configKeyIter.value() = abs(configKeyIter.value());
        }else{
            qDebug() << "addIndex begin" << newProceduresMap;
            return false; // Duplicated
        }
    }
    if(not found)
    {
        configKeyIter = newProceduresMap.insert({name, static_cast<RulesCategories>(index)}, 0);
        configKeyIter++;
        qsizetype value;
        if(configKeyIter != newProceduresMap.end()){
            value = configKeyIter.value();
        }else{
            value = newRules.size();
        }

        configKeyIter--;
        configKeyIter.value() = abs(value);
    }
    qDebug() << "addIndex begin" << newProceduresMap;
    return true;
}

bool ControllerConfigInfo::addIndex(qsizetype index){
    // Preverification:
    // 1. Find key <name, index> in config
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    using EditIndexIter = QMap<QPair<QString, qsizetype>, QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator>::Iterator;
    //EditIndexIter editIndexIter = newNames.find(name);
    qDebug() << "addIndex begin" << newDefaultProcedureMap;
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
    configKeyIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(index));
    if(configKeyIter != newDefaultProcedureMap.end()){
        found = true;
        if(configKeyIter.value() < 0){
            // Removed - restore
            configKeyIter.value() = abs(configKeyIter.value());
        }else{
            qDebug() << "addIndex end" << newDefaultProcedureMap;
            return false; // Duplicated
        }
    }
    if(not found)
    {
        configKeyIter = newDefaultProcedureMap.insert(static_cast<RulesCategories>(index), 0);
        configKeyIter++;
        qsizetype value;
        if(configKeyIter != newDefaultProcedureMap.end()){
            value = configKeyIter.value();
        }else{
            value = newDefaultProcedureRules.size();
        }

        configKeyIter--;
        configKeyIter.value() = abs(value);
    }
    qDebug() << "addIndex end" << newDefaultProcedureMap;
    return true;
}

bool ControllerConfigInfo::editIndex(qsizetype oldIndex, qsizetype newIndex){
    // 1.1. Edited index - Change key in editted
    // 1.2. config without newRules priority (no index in newRules) - New key in newNames -> Negative value for index in Config
    // 1.21. config with newRules priority - Change index in new Config if new index not in newRules or new index not in editted or removed index in config
    // -> Config index change to removed
    // 1.3. Non in config, only new - Change index in new Config if new index not in newRules or new index not in editted or (removed index in config - set not removed - new rules priority here)
    using Config = decltype(newDefaultProcedureMap);
    qDebug() << "editIndex begin" << newDefaultProcedureMap;
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
    if((beginIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(newIndex))) != newDefaultProcedureMap.end() and
            (beginIter.value() >= 0) )
    {
        qDebug() << "editIndex end" << newDefaultProcedureMap;
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
                clearAllNewRules(newIndex);
                // Config New index - Set in not removed state
                //configIter = configMap.find({name, static_cast<RulesCategories>(newIndex)});
                //configIter.value() = abs(configIter.value());
                // New Config new index - Move old rules to newIndex ----------------------------------------------------
                moveAllNewRules(oldIndex, newIndex);
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
                qDebug() << "editIndex end" << newDefaultProcedureMap;

    return true;
}

bool ControllerConfigInfo::editIndex(QString name, qsizetype oldIndex, qsizetype newIndex){
    // 1.1. Edited index - Change key in editted
    // 1.2. config without newRules priority (no index in newRules) - New key in newNames -> Negative value for index in Config
    // 1.21. config with newRules priority - Change index in new Config if new index not in newRules or new index not in editted or removed index in config
    // -> Config index change to removed
    // 1.3. Non in config, only new - Change index in new Config if new index not in newRules or new index not in editted or (removed index in config - set not removed - new rules priority here)
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "editIndex begin" << newProceduresMap;
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
    if((beginIter = newProceduresMap.find({name, static_cast<RulesCategories>(newIndex)})) != newProceduresMap.end() and
            (beginIter.value() >= 0) )
    {
        qDebug() << "editIndex end" << newProceduresMap;
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

                qDebug() << "editIndex end" << newProceduresMap;
    return true;
}

void ControllerConfigInfo::removeIndex(QString name, qsizetype index){
    // Preverification:
    // 1. Find key <name, index> in config
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "removeIndex begin" << newProceduresMap;
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
    configKeyIter = newProceduresMap.find({name, static_cast<RulesCategories>(index)});

    if(configKeyIter != newProceduresMap.end()){
        configKeyIter.value() = -(configKeyIter.value() + 1);
    }
    qDebug() << "removeIndex end" << newProceduresMap;
}

void ControllerConfigInfo::removeIndex(qsizetype index){
    // Preverification:
    // 1. Find key <name, index> in config
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "removeIndex begin" << newDefaultProcedureMap;
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
    configKeyIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(index));

    if(configKeyIter != newDefaultProcedureMap.end()){
        configKeyIter.value() = -(configKeyIter.value() + 1);
    }
    qDebug() << "removeIndex end" << newDefaultProcedureMap;
}

void ControllerConfigInfo::clearIndexes(QString name){
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "clearIndexes begin" << newProceduresMap;
    ConfigKeyIter configKeyIter;

    configKeyIter = newProceduresMap.upperBound({name, RulesCategories::UndefinedArgument});

    while( configKeyIter != newProceduresMap.end() and configKeyIter.key().second != RulesCategories::OnEndOfCall){
        if(configKeyIter.value() >= 0){
            configKeyIter.value() = -(configKeyIter.value() + 1);
        }
        configKeyIter++;
    }    
    qDebug() << "clearIndexes end" << newProceduresMap;
}

void ControllerConfigInfo::clearIndexes(){
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "clearIndexes begin" << newDefaultProcedureMap;
    ConfigKeyIter configKeyIter;
    ConfigKeyIter endIter;

    configKeyIter = newDefaultProcedureMap.upperBound(RulesCategories::UndefinedArgument);

    while( configKeyIter != newDefaultProcedureMap.end() and configKeyIter.key() != RulesCategories::OnEndOfCall){
        if(configKeyIter.value() >= 0){
            configKeyIter.value() = -(configKeyIter.value() + 1);
        }
        configKeyIter++;
    }
    qDebug() << "clearIndexes end" << newDefaultProcedureMap;
}

void ControllerConfigInfo::clearProcedures(){
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    qDebug() << "clearProcedures begin" << newProceduresMap;
    ConfigKeyIter configKeyIter;
    ConfigKeyIter endIter;

    configKeyIter = newProceduresMap.begin();

    while( configKeyIter != newProceduresMap.end()){
        if(configKeyIter.key().second == RulesCategories::OnEndOfCall and configKeyIter.value() >= 0){
            configKeyIter.value() = -(configKeyIter.value() + 1);
        }
        configKeyIter++;
    }
    numbOfExistingProcedures = 0;
    qDebug() << "clearProcedures end" << newProceduresMap;
}

void ControllerConfigInfo::clear(){
    settings().writeOnlyProcedures().clear();
    numbOfExistingProcedures = 0;
    numbOfAllProcedures = 0;
    newProceduresMap.clear();
    newDefaultProcedureMap.clear();
    for(NewRules::Iterator rule = newRules.begin(); rule != newRules.end(); rule++)
        delete (*rule);
    for(NewRules::Iterator rule = newDefaultProcedureRules.begin(); rule != newDefaultProcedureRules.end(); rule++)
        delete (*rule);
}

void ControllerConfigInfo::loadNewRules(QString name, RulesCategories index, NewRules& rules){
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    qDebug() << "loadNewRules begin" << newProceduresMap << newRules;
    beginIter = newProceduresMap.find({name, static_cast<RulesCategories>(index)});
    if(beginIter != newProceduresMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newProceduresMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newRules.size() - position;
        }
        qsizetype diff = rules.size() - numbOfRules;
        if(diff != 0){
            beginIter++;
            while(beginIter != newProceduresMap.end()){
                beginIter.value() += (beginIter.value() < 0)? -diff : diff;
                beginIter++;
            }
        }
        NewRules::Iterator tempRulesIter = newRules.begin() + position;
        NewRules::Iterator newRulesIter = tempRulesIter + numbOfRules;
        for( ; tempRulesIter < newRulesIter; tempRulesIter++){
            delete *tempRulesIter;
        }
        newRules.remove(position, numbOfRules);
        tempRulesIter = rules.begin();
        newRules.insert(position, rules.size(), nullptr);
        newRulesIter = newRules.begin() + position;
        for(; tempRulesIter < rules.end(); tempRulesIter++, newRulesIter++){
            *newRulesIter = *tempRulesIter;
        }
    }

    qDebug() << "loadNewRules end" << newProceduresMap << newRules;
}

void ControllerConfigInfo::loadNewRules(RulesCategories index, NewRules& rules){
    qDebug() << "loadNewRules begin" << newDefaultProcedureMap << newDefaultProcedureRules;
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;

    beginIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(index));
    if(beginIter != newDefaultProcedureMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newDefaultProcedureMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newDefaultProcedureRules.size() - position;
        }
        qsizetype diff = rules.size() - numbOfRules;
        if(diff != 0){
            beginIter++;
            while(beginIter != newDefaultProcedureMap.end()){
                beginIter.value() += (beginIter.value() < 0)? -diff : diff;
                beginIter++;
            }
        }
        NewRules::Iterator tempRulesIter = newDefaultProcedureRules.begin() + position;
        NewRules::Iterator newRulesIter = tempRulesIter + numbOfRules;
        for( ; tempRulesIter < newRulesIter; tempRulesIter++){
            delete *tempRulesIter;
        }
        newDefaultProcedureRules.remove(position, numbOfRules);
        tempRulesIter = rules.begin();
        newDefaultProcedureRules.insert(position, rules.size(), nullptr);
        newRulesIter = newDefaultProcedureRules.begin() + position;
        for(; tempRulesIter < rules.end(); tempRulesIter++, newRulesIter++){
            *newRulesIter = *tempRulesIter;
        }
    }

    qDebug() << "loadNewRules end" << newDefaultProcedureMap << newDefaultProcedureRules;
}

ControllerConfigInfo::RulesView ControllerConfigInfo::readRules(QString name, RulesCategories index){
    qDebug() << "readRules begin" << newProceduresMap << newRules;

    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    ConfigKeyIter beginIter;

    beginIter = newProceduresMap.find({name, index});
    if(beginIter != newProceduresMap.end()){
    qsizetype position = abs(beginIter.value());
    qsizetype numbOfRules = 0;
    beginIter++;
        if(beginIter != newProceduresMap.end()){
            numbOfRules = abs(beginIter.value()) - position;
        }else{
            numbOfRules = newRules.size() - position;
        }

        return RulesView(newRules.begin() + position, newRules.begin() + position + numbOfRules);
    }

    return RulesView();
    qDebug() << "readRules end" << newProceduresMap << newRules;
}

ControllerConfigInfo::RulesView ControllerConfigInfo::readRules(RulesCategories index){
    qDebug() << "readRules begin" << newDefaultProcedureMap << newDefaultProcedureRules;
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    ConfigKeyIter beginIter;

    beginIter = newDefaultProcedureMap.find(index);
    if(beginIter != newDefaultProcedureMap.end()){
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        beginIter++;
        if(beginIter != newDefaultProcedureMap.end()){
            numbOfRules = abs(beginIter.value()) - position;
        }else{
            numbOfRules = newDefaultProcedureRules.size() - position;
        }

        return RulesView(newDefaultProcedureRules.begin() + position, newDefaultProcedureRules.begin() + position + numbOfRules);
    }

    return RulesView();
    qDebug() << "readRules end" << newDefaultProcedureMap << newDefaultProcedureRules;
}


void ControllerConfigInfo::readProcedures(QList<UserProcedure>& userProcedures){
    using UserProcedures = QList<UserProcedure>;
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    using ProcedureName = UserProcedure::ProcedureName;
    using RulesForArguments = UserProcedure::RulesForArguments;
    using RulesForArgument = UserProcedure::RulesForArgument;
    using RulesOnEndOfCall = UserProcedure::RulesOnEndOfCall;
    using Rules = UserProcedureRules;
    Q_ASSERT_X(userProcedures.capacity() == getNumbOfExistingProcedures(), "ConfigInfo", "UserProcedures list capacity is not prepared");
    Q_ASSERT_X(userProcedures.size() == getNumbOfExistingProcedures(), "ConfigInfo", "UserProcedures list size is not prepared");

    RulesForArguments rulesForArguments;
    RulesForArgument rulesForDefaultArgument;
    RulesOnEndOfCall rulesOnEndOfCall;

    QString name;
    qsizetype numbOfRules;
    UserProcedures::Iterator userProcedureIter = userProcedures.begin();
    NewRules::Iterator newRule;
    NewRules::Iterator endRule;
    Rules::Iterator ruleForInterpreter;
    RulesForArguments::Iterator rulesForArgument;
    qsizetype numbOfIndexes;
    // Prepare containers
    // On end of Call
    ConfigKeyIter configProcedureStartIter = newProceduresMap.begin();
    ConfigKeyIter configMapIter = configProcedureStartIter;

    while(configMapIter != newProceduresMap.end()){
        configProcedureStartIter = configMapIter;
        name = configMapIter.key().first;
        if(configMapIter.value() >= 0){ // Check if Exists
            newRule = newRules.begin() + configMapIter.value();
            configMapIter++;
            endRule = newRules.begin() + configMapIter.value(); // Can be read cause Undefined Argument cant be nagative
            numbOfRules = endRule - newRule;
            // Reserve and resize
            rulesOnEndOfCall.reserve(numbOfRules);
            rulesOnEndOfCall.resize(numbOfRules);
            ruleForInterpreter = rulesOnEndOfCall.begin();
            for(; newRule < endRule; newRule++, ruleForInterpreter++)
            {
                (*newRule)->toRule((*ruleForInterpreter));
            }

            // Undefined arguments
            newRule = newRules.begin() + configMapIter.value();
            configMapIter++;
            if(configMapIter != newProceduresMap.end()){
                endRule = newRules.begin() + abs(configMapIter.value());
            }else{
                endRule = newRules.begin() + newRules.size();
            }
            numbOfRules = endRule - newRule;
            // Reserve and resize
            rulesForDefaultArgument.rulesOnMoveArgument.reserve(numbOfRules);
            rulesForDefaultArgument.rulesOnMoveArgument.resize(numbOfRules);
            ruleForInterpreter = rulesForDefaultArgument.rulesOnMoveArgument.begin();
            for(; newRule < endRule; newRule++, ruleForInterpreter++)
            {
                (*newRule)->toRule((*ruleForInterpreter));
            }

            // Rules for arguments
            numbOfIndexes = 0;
            while(configMapIter != newProceduresMap.end() and configMapIter.key().second != RulesCategories::OnEndOfCall){
                if(configMapIter.value() >= 0) // Index exists
                    numbOfIndexes++;
                configMapIter++;
            }
            rulesForArguments.reserve(numbOfIndexes);
            rulesForArguments.resize(numbOfIndexes);

            configMapIter = configProcedureStartIter; // On End
            configMapIter++; // Undefined
            configMapIter++; // First Index
            rulesForArgument = rulesForArguments.begin();
            while(configMapIter != newProceduresMap.end() and configMapIter.key().second != RulesCategories::OnEndOfCall){
                if(configMapIter.value() >= 0) // Index exists
                {
                    // Set Index
                    rulesForArgument->index = configMapIter.key().second;
                    // Get numb of rules + configure newRules iterators
                    newRule = newRules.begin() + configMapIter.value();
                    configMapIter++;
                    if(configMapIter != newProceduresMap.end()){
                        endRule = newRules.begin() + abs(configMapIter.value());
                    }else{
                        endRule = newRules.end();
                    }
                    numbOfRules = endRule - newRule;

                    // Set rules for interpreter vector size
                    rulesForArgument->rulesOnMoveArgument.reserve(numbOfRules);
                    rulesForArgument->rulesOnMoveArgument.resize(numbOfRules);
                    ruleForInterpreter = rulesForArgument->rulesOnMoveArgument.begin();

                    for(; newRule < endRule; newRule++, ruleForInterpreter++)
                    {
                        (*newRule)->toRule((*ruleForInterpreter));
                    }
                    rulesForArgument++;
                }else{
                    configMapIter++;
                }
            }
            (*userProcedureIter) = UserProcedure(name, rulesForArguments, rulesForDefaultArgument, rulesOnEndOfCall);
            userProcedureIter++;
        }else{ // Not exist - move to next procedure
            configMapIter++;
            while(configMapIter != newProceduresMap.end() and configMapIter.key().second != RulesCategories::OnEndOfCall){
                configMapIter++;
            }
            // Do not increment again
        }
    }
}

UserProcedure ControllerConfigInfo::readDefaultProcedure(){

    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    using ProcedureName = UserProcedure::ProcedureName;
    using RulesForArguments = UserProcedure::RulesForArguments;
    using RulesForArgument = UserProcedure::RulesForArgument;
    using RulesOnEndOfCall = UserProcedure::RulesOnEndOfCall;

    using Rules = UserProcedureRules;

    RulesForArguments rulesForArguments;
    RulesForArgument rulesForDefaultArgument;
    RulesOnEndOfCall rulesOnEndOfCall;

    // Prepare containers
    // On end of Call
    ConfigKeyIter configMapIter = newDefaultProcedureMap.begin();
    configMapIter++;
    rulesOnEndOfCall.reserve(configMapIter.value());
    rulesOnEndOfCall.resize(rulesOnEndOfCall.capacity());
    // Undefined Argument
    configMapIter++;
    rulesForDefaultArgument.rulesOnMoveArgument.reserve((configMapIter != newDefaultProcedureMap.end())?
                                                            abs(configMapIter.value()) - rulesOnEndOfCall.size() :
                                                            newDefaultProcedureRules.size() - rulesOnEndOfCall.size());
    rulesForDefaultArgument.rulesOnMoveArgument.resize(rulesForDefaultArgument.rulesOnMoveArgument.capacity());
    // Rules for arguments
    qsizetype numbOfIndexes = 0;
    while(configMapIter != newDefaultProcedureMap.end()){
        if(configMapIter.value() >= 0){
            numbOfIndexes++;
        }
        configMapIter++;
    }
    rulesForArguments.reserve(numbOfIndexes);
    rulesForArguments.resize(rulesForArguments.capacity());

    // Load rules
    // On End of Call;
    NewRules::Iterator newRule = newDefaultProcedureRules.begin();
    NewRules::Iterator endRule = newRule + rulesOnEndOfCall.size();
    Rules::Iterator ruleForInterpreter = rulesOnEndOfCall.begin();

    for(; newRule < endRule; newRule++, ruleForInterpreter++)
    {
        (*newRule)->toRule((*ruleForInterpreter));
    }

    // On default argument
    newRule = endRule;
    endRule = newRule + rulesForDefaultArgument.rulesOnMoveArgument.size();
    ruleForInterpreter = rulesForDefaultArgument.rulesOnMoveArgument.begin();

    for(; newRule < endRule; newRule++, ruleForInterpreter++)
    {
        (*newRule)->toRule((*ruleForInterpreter));
    }

    // On arguments
    configMapIter = newDefaultProcedureMap.begin(); // On End
    configMapIter++; // Undefined
    configMapIter++; // First Index
    qsizetype numbOfRules = 0;
    RulesForArguments::Iterator rulesForArgument = rulesForArguments.begin();
    while(configMapIter != newDefaultProcedureMap.end()){
        if(configMapIter.value() >= 0){
            // Set Index
            rulesForArgument->index = configMapIter.key();
            // Get numb of rules + configure newRules iterators
            newRule = newDefaultProcedureRules.begin() + configMapIter.value();
            configMapIter++;
            if(configMapIter != newDefaultProcedureMap.end()){
                endRule = newDefaultProcedureRules.begin() + configMapIter.value();
            }else{
                endRule = newDefaultProcedureRules.end();
            }
            numbOfRules = endRule - newRule;

            // Set rules for interpreter vector size
            rulesForArgument->rulesOnMoveArgument.reserve(numbOfRules);
            rulesForArgument->rulesOnMoveArgument.resize(numbOfRules);
            ruleForInterpreter = rulesForArgument->rulesOnMoveArgument.begin();

            for(; newRule < endRule; newRule++, ruleForInterpreter++)
            {
                (*newRule)->toRule((*ruleForInterpreter));
            }
            rulesForArgument++;
        }else{
            configMapIter++;
        }
    }

    return UserProcedure("", rulesForArguments, rulesForDefaultArgument, rulesOnEndOfCall);

}


bool ControllerConfigInfo::addIndex(QString name, RulesFromConfigFileView& rulesView)
{
    qDebug() << "addIndex begin" << newProceduresMap;
    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    using EditIndexIter = QMap<QPair<QString, qsizetype>, QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator>::Iterator;
    //EditIndexIter editIndexIter = newNames.find(name);
    ConfigKeyIter configKeyIter;
    QString oldName;
    bool found = false;
    NewRules& rules = rulesView.rules;
    const qsizetype& index = static_cast<qsizetype>(rulesView.index);
    configKeyIter = newProceduresMap.find({name, static_cast<RulesCategories>(index)});
    if(index > -1){
        if(configKeyIter != newProceduresMap.end()){
            return false; // Duplicated
        }

        configKeyIter = newProceduresMap.insert({name, static_cast<RulesCategories>(index)}, 0);
        configKeyIter++;
        qsizetype value;
        if(configKeyIter != newProceduresMap.end()){
            value = configKeyIter.value();
        }else{
            value = newRules.size();
        }

        configKeyIter--;
        configKeyIter.value() = abs(value);
    }

    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;

    beginIter = newProceduresMap.find({name, static_cast<RulesCategories>(index)});
    if(beginIter != newProceduresMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newProceduresMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newRules.size() - position;
        }
        qsizetype diff = rules.size() - numbOfRules;
        if(diff != 0){
            beginIter++;
            while(beginIter != newProceduresMap.end()){
                beginIter.value() += (beginIter.value() < 0)? -diff : diff;
                beginIter++;
            }
        }
        NewRules::Iterator tempRulesIter = newRules.begin() + position;
        NewRules::Iterator newRulesIter = tempRulesIter + numbOfRules;
        for( ; tempRulesIter < newRulesIter; tempRulesIter++){
            delete *tempRulesIter;
        }
        newRules.remove(position, numbOfRules);
        tempRulesIter = rules.begin();
        newRules.insert(position, rules.size(), nullptr);
        newRulesIter = newRules.begin() + position;
        for(; tempRulesIter < rules.end(); tempRulesIter++, newRulesIter++){
            *newRulesIter = *tempRulesIter;
        }
    }

    return true;
}

bool ControllerConfigInfo::addIndex(RulesFromConfigFileView& rulesView){
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    using EditIndexIter = QMap<QPair<QString, qsizetype>, QMap<ProcedureRuleCategoryKey, qsizetype>::Iterator>::Iterator;

    ConfigKeyIter configKeyIter;
    QString oldName;
    bool found = false;
    NewRules& rules = rulesView.rules;
    const qsizetype& index = static_cast<qsizetype>(rulesView.index);

    configKeyIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(index));
    if(index > -1){
    if(configKeyIter != newDefaultProcedureMap.end()){
        return false; // Duplicated
    }
        configKeyIter = newDefaultProcedureMap.insert(static_cast<RulesCategories>(index), 0);
        configKeyIter++;
        qsizetype value;
        if(configKeyIter != newDefaultProcedureMap.end()){
            value = configKeyIter.value();
        }else{
            value = newDefaultProcedureRules.size();
        }

        configKeyIter--;
        configKeyIter.value() = abs(value);
    }

    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;

    beginIter = newDefaultProcedureMap.find(static_cast<RulesCategories>(index));
    if(beginIter != newDefaultProcedureMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newDefaultProcedureMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newDefaultProcedureRules.size() - position;
        }
        qsizetype diff = rules.size() - numbOfRules;
        if(diff != 0){
            beginIter++;
            while(beginIter != newDefaultProcedureMap.end()){
                beginIter.value() += (beginIter.value() < 0)? -diff : diff;
                beginIter++;
            }
        }
        NewRules::Iterator tempRulesIter = newDefaultProcedureRules.begin() + position;
        NewRules::Iterator newRulesIter = tempRulesIter + numbOfRules;
        for( ; tempRulesIter < newRulesIter; tempRulesIter++){
            delete *tempRulesIter;
        }
        newDefaultProcedureRules.remove(position, numbOfRules);
        tempRulesIter = rules.begin();
        newDefaultProcedureRules.insert(position, rules.size(), nullptr);
        newRulesIter = newDefaultProcedureRules.begin() + position;
        for(; tempRulesIter < rules.end(); tempRulesIter++, newRulesIter++){
            *newRulesIter = *tempRulesIter;
        }
    }

    return true;
}



