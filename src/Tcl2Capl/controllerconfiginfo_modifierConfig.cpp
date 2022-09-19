#include"controllerconfiginfo.hpp"


bool ControllerConfigInfo::addPhase(QString name){
    qDebug() << "addPhase begin" << newPhasesMap;

    // 1. Find key <name, OnEndOfCall> in config
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    ConfigKeyIter configKeyIter;
    ConfigKeyIter configKeyHelperIter;

    // Preverification - only alphanumeric charactes + _ signs
    if(not QRegularExpression(RegExpCore::regExprForPhaseName).match(name).hasMatch()){
        qDebug() << "addPhase end - incorrect phase name" << name;
        return false;
    }

    // Main:
    configKeyIter = newPhasesMap.find({name, Category::LowestValue});
    if(configKeyIter == newPhasesMap.end()){ // Not Found
        // 1. Add new Keys:
        // iter1 = <name, OnEndOfCall>, - Value - Absolute rule position
        configKeyIter = newPhasesMap.insert({name, Category::LowestValue}, 0);
        configKeyHelperIter = configKeyIter; // To +1
        configKeyHelperIter++;
        // 2. Specify values for keys:
        // 2.1. Check if iter2 + 1 is valid key
        if(configKeyHelperIter != newPhasesMap.end()){
            // TRUE:
            // 2.2. Copy value of iter2 + 1 to value of iter1
            configKeyIter.value() = abs(configKeyHelperIter.value());
        }else{
            // FALSE:
            // 2.2. Set value iter1 to rulesList.size
            configKeyIter.value() = newModifierRules.size();
        }
        configKeyHelperIter = configKeyIter;
        configKeyHelperIter = newPhasesMap.insert({name, Category::OnEndOfRulesCheck}, configKeyIter.value());
        configKeyHelperIter = newPhasesMap.insert({name, Category::OnNoRulesSatisfied}, configKeyIter.value());

        ModifierNewRules newRules{ModifierRulesFactory::create(ModifierRulesFactory::ProductTypeEnum::RawRule)};
        loadNewModifierRules(name, Category::OnEndOfRulesCheck, newRules);
        newRules = {ModifierRulesFactory::create(ModifierRulesFactory::ProductTypeEnum::RawRule)};
        loadNewModifierRules(name, Category::OnNoRulesSatisfied, newRules);

    }else{
        if(configKeyIter.value() < 0){
            // Unlock and read - unlock by -(value + 1)
            configKeyIter.value() = abs(configKeyIter.value());
            // Restore
            countUp_RestoredPhase();
            return true;
        }else{
            qDebug() << "addPhase end" << newPhasesMap;
            return false;
        }
    }
    countUp_NewPhase();
    qDebug() << "addPhase end" << newPhasesMap;
    return true;
}

void ControllerConfigInfo::clearAllNewModifierRules(QString name){
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    qDebug() << "clearAllNewModifierRules begin" << newPhasesMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newPhasesMap.lowerBound({name, static_cast<Category>(LONG_LONG_MIN)});
    if(beginIter != newPhasesMap.end() and beginIter.key().first == name){ // Any keys exist
        ConfigKeyIter savedConfigIter = beginIter;
        // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
        endIter = newPhasesMap.lowerBound({name, static_cast<Category>(LONG_LONG_MAX)});
        // 2. Calculate number of rules and remove it
        qsizetype numbOfRules = 0;
        qsizetype position = abs(beginIter.value());
        if(endIter != newPhasesMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newModifierRules.size() - position;
        }
        ModifierNewRules::Iterator tempRuleIter = newModifierRules.begin() + position;
        ModifierNewRules::Iterator endRuleIter = tempRuleIter + numbOfRules;

        for( ; tempRuleIter < endRuleIter; tempRuleIter++)
            delete *tempRuleIter;

        newModifierRules.remove(position, numbOfRules);
        // 3. Recalculate all absolute values of rules after this procedure
        while(beginIter != newPhasesMap.end()){
            beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
            beginIter++;
        }
        // 4. Erase keys
        newPhasesMap.erase(savedConfigIter, endIter);

        countDown_RemoveHiddenPhase();
    }
    qDebug() << "clearAllNewModifierRules end" << newPhasesMap;
}

void ControllerConfigInfo::clearAllNewModifierRules(QString name, ModifierRulesCategories index){
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    qDebug() << "clearAllNewModifierRules begin" << newPhasesMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newPhasesMap.find({name, index});
    if(beginIter != newPhasesMap.end()){ // Any rules for key
        ConfigKeyIter savedConfigIter = beginIter;
        // iter2 = <name, MAX_VALUE> - 1 -  Value - Offset of previous rule category = 0
        beginIter++;
        endIter = beginIter;
        beginIter--;
        // 2. Calculate number of rules and remove it
        qsizetype numbOfRules = 0;
        qsizetype position = abs(beginIter.value());
        if(endIter != newPhasesMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newModifierRules.size() - position;
        }
        if(numbOfRules != 0){
            ModifierNewRules::Iterator tempRuleIter = newModifierRules.begin() + position;
            ModifierNewRules::Iterator endRuleIter = tempRuleIter + numbOfRules;

            for( ; tempRuleIter < endRuleIter; tempRuleIter++)
                delete *tempRuleIter;

            newModifierRules.remove(position, numbOfRules);
            // 3. Recalculate all absolute values of rules after this procedure
            while(beginIter != newPhasesMap.end()){
                beginIter.value() -= (beginIter.value() < 0)? -numbOfRules : numbOfRules;
                beginIter++;
            }
        }
        // 4. Erase keys
        newPhasesMap.erase(savedConfigIter);
    }
    qDebug() << "clearAllNewModifierRules end" << newPhasesMap;
}

void ControllerConfigInfo::moveAllNewModifierRules(QString oldName, QString newName){
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    qDebug() << "moveAllNewModifierRules begin" << newPhasesMap;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    beginIter = newPhasesMap.find({oldName, Category::LowestValue});
    if(beginIter != newPhasesMap.end() and beginIter.key().first == oldName){
        endIter = newPhasesMap.upperBound({oldName, static_cast<Category>(LONG_LONG_MAX)});
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newPhasesMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newModifierRules.size() - position;
        }

        // Erase Keys
        QList<QPair<Category, qsizetype>> tempMapPairs;
        for(ConfigKeyIter tempIter = beginIter; tempIter != endIter; tempIter++){
            tempMapPairs.append({tempIter.key().second, tempIter.value()});
        }
        endIter = newPhasesMap.erase(beginIter, endIter);

        // Setup new keys
        // 1. Add new Keys:
        ConfigKeyIter newBeginIter = newPhasesMap.insert({newName, tempMapPairs.first().first}, tempMapPairs.first().second);
        ConfigKeyIter tempIter = newBeginIter;
        for(decltype(tempMapPairs)::Iterator tempPairsIter = tempMapPairs.begin()+1; tempPairsIter != tempMapPairs.end(); tempPairsIter++){
            tempIter = newPhasesMap.insert({newName, tempPairsIter->first}, tempPairsIter->second);
        }
        // <name, Settings>, - Value - No User Iteraction as Default = 0
        tempIter++;
        ConfigKeyIter newEndIter = tempIter;
        bool changed = true;
        qsizetype diff = 0;

        // Up or down
        if(tempIter != newPhasesMap.end()){
            if(abs(tempIter.value()) - numbOfRules != position){
                if(abs(tempIter.value()) <= position){ // Up
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
            if(newModifierRules.size() - numbOfRules != position){ // Up impossible? Only Down
                newBeginIter--;
                endIter--;
                diff = position - newModifierRules.size() + numbOfRules;
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
            ModifierNewRules tempRules;
            if(numbOfRules != 0){   // If rules to move
                tempRules = newModifierRules.mid(position, numbOfRules);
                newModifierRules.remove(position, numbOfRules);
                ModifierNewRules::Iterator tempRulesIter = tempRules.begin();
                ModifierNewRules::Iterator newRulesIter;
                newModifierRules.insert(newBeginIter.value(), tempRules.size(), nullptr);
                newRulesIter = newModifierRules.begin() + newBeginIter.value();
                for( ; tempRulesIter < tempRules.end(); tempRulesIter++, newRulesIter++){
                    *newRulesIter = *tempRulesIter;
                }
            }
        }
    }
    qDebug() << "moveAllNewModifierRules end" << newPhasesMap;
}

bool ControllerConfigInfo::editPhaseName(QString oldName, QString newName){
    // NewName verification
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    qDebug() << "editPhaseName begin" << newPhasesMap;

    if(not QRegularExpression(RegExpCore::regExprForPhaseName).match(newName).hasMatch()){
        qDebug() << "addPhase end - incorrect phase name" << newName;
        return false;
    }
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;

    qDebug() << oldName << newName;
    if(((beginIter = newPhasesMap.find({newName, Category::LowestValue})) != newPhasesMap.end() and
            ( beginIter.value() >= 0)) )
    {
        return false;
    }

    // Remove all removed new index of new name procedure
    // 1. Find keys:
    // iter1 = <name, Settings>, - Value - Absolute rule position
    clearAllNewModifierRules(newName);
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
    moveAllNewModifierRules(oldName, newName);
    qDebug() << "editPhaseName end" << newPhasesMap;

    return true;
}

bool ControllerConfigInfo::removePhase(QString name){
    // 1. Old name is:
    // 1.1. Edited name - Get iter from newNames -> Remove from newNames -> Change in config -> Remove from newRules
    // 1.2. Non-edited in config - Negative value for OnEndOfCall in Config
    // -> If Config is removed, new rules dont require Removed flag too, save it to keep removed indexes
    // 1.3. Non in config, only new - Just Remove
    // Preverification:
    // 1. Find key <name, OnEndOfCall> in config
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    using EditNameIter = QMap<QString, QMap<PhaseModifierRuleCategoryKey, qsizetype>::Iterator>::Iterator;
    qDebug() << "removePhase begin" << newPhasesMap;
    ConfigKeyIter configKeyIter;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;

    configKeyIter = newPhasesMap.find({name, Category::LowestValue});
    if(configKeyIter != newPhasesMap.end()){
        configKeyIter.value() = -(configKeyIter.value() + 1); // Set Removed
        countDown_HidePhase();
    }
    qDebug() << "removePhase end" << newPhasesMap;
    return true;
}


void ControllerConfigInfo::clearPhases(){
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    qDebug() << "clearPhases begin" << newPhasesMap;
    ConfigKeyIter configKeyIter;
    ConfigKeyIter endIter;

    configKeyIter = newPhasesMap.begin();

    while( configKeyIter != newPhasesMap.end()){
        if(configKeyIter.key().second == Category::LowestValue and configKeyIter.key().first != "Default"  and configKeyIter.value() >= 0){
            configKeyIter.value() = -(configKeyIter.value() + 1);
        }
        configKeyIter++;
    }
    numbOfExistingPhases = 1;
    qDebug() << "clearPhases end" << newPhasesMap;
}


void ControllerConfigInfo::loadNewModifierRules(QString name, ModifierRulesCategories index, ModifierNewRules& rules){
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;
    qDebug() << "loadNewModifierRules begin" << newPhasesMap << newModifierRules;
    beginIter = newPhasesMap.find({name, static_cast<Category>(index)});
    if(beginIter != newPhasesMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newPhasesMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newModifierRules.size() - position;
        }
        qsizetype diff = rules.size() - numbOfRules;
        if(diff != 0){
            beginIter++;
            while(beginIter != newPhasesMap.end()){
                beginIter.value() += (beginIter.value() < 0)? -diff : diff;
                beginIter++;
            }
        }
        ModifierNewRules::Iterator tempRulesIter = newModifierRules.begin() + position;
        ModifierNewRules::Iterator newRulesIter = tempRulesIter + numbOfRules;
        for( ; tempRulesIter < newRulesIter; tempRulesIter++){
            delete *tempRulesIter;
        }
        newModifierRules.remove(position, numbOfRules);
        tempRulesIter = rules.begin();
        newModifierRules.insert(position, rules.size(), nullptr);
        newRulesIter = newModifierRules.begin() + position;
        for(; tempRulesIter < rules.end(); tempRulesIter++, newRulesIter++){
            *newRulesIter = *tempRulesIter;
        }
    }

    qDebug() << "loadNewModifierRules end" << newPhasesMap << newModifierRules;
}

ControllerConfigInfo::ModifierRulesView ControllerConfigInfo::readModifierRules(QString name, ModifierRulesCategories index){
    qDebug() << "readModifierRules begin" << newPhasesMap << newModifierRules;

    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using Category = ModifierRulesCategories;
    ConfigKeyIter beginIter;

    beginIter = newPhasesMap.find({name, index});
    if(beginIter != newPhasesMap.end()){
    qsizetype position = abs(beginIter.value());
    qsizetype numbOfRules = 0;
    beginIter++;
        if(beginIter != newPhasesMap.end()){
            numbOfRules = abs(beginIter.value()) - position;
        }else{
            numbOfRules = newModifierRules.size() - position;
        }

        return ModifierRulesView(newModifierRules.begin() + position, newModifierRules.begin() + position + numbOfRules);
    }

    return ModifierRulesView();
    qDebug() << "readModifierRules end" << newPhasesMap << newModifierRules;
}


void ControllerConfigInfo::readPhases(TcFileModifierConfigBase::ModifierPhases& userPhases){
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using ProcedureName = UserProcedure::ProcedureName;
    using ConfigRules = TcFileModifierConfigBase::ModifierRules;
    using ConfigActions = TcFileModifierConfigBase::ModifierRule::Actions;
    using ModifierRules = ModifierNewRules;
    using ModifierActions = ModifierActions;

    QString name;
    qsizetype numbOfRulesOrActions;
    TcFileModifierConfigBase::ModifierPhases::Iterator userPhaseIter;
    ModifierRules::Iterator newRule;
    ModifierRules::Iterator endRule;
    ConfigRules::Iterator ruleForInterpreter;
    // Prepare containers
    // On No Rules Satisfied
    ConfigKeyIter configPhaseStartIter = newPhasesMap.begin();
    ConfigKeyIter configMapIter = configPhaseStartIter;

    while(configMapIter != newPhasesMap.end()){
        configPhaseStartIter = configMapIter;
        name = configMapIter.key().first;
        if(configMapIter.value() >= 0){ // Check if Exists
            // Add Phase
            userPhaseIter = userPhases.insert(name, TcFileModifierConfigBase::ModifierPhase());

            // Rules
            newRule = newModifierRules.begin() + configMapIter.value();
            configMapIter++;
            if(configMapIter != newPhasesMap.end()){
                endRule = newModifierRules.begin() + abs(configMapIter.value());
            }else{
                endRule = newModifierRules.end();
            }
            numbOfRulesOrActions = endRule - newRule;

            // Set rules for interpreter vector size
            userPhaseIter->rules.reserve(numbOfRulesOrActions);
            userPhaseIter->rules.resize(numbOfRulesOrActions);

            ruleForInterpreter = userPhaseIter->rules.begin();

            for(; newRule < endRule; newRule++, ruleForInterpreter++)
            {
                (*newRule)->toRule((*ruleForInterpreter));
            }
            // On No Rules Satisfied - Actions - Only 1 Rule - Container only
            newRule = newModifierRules.begin() + configMapIter.value();
            configMapIter++;
            // Reserve and resize
            (*newRule)->toActions(userPhaseIter->onEnd.onNoRules);

            // On end of Rules check
            newRule = newModifierRules.begin() + configMapIter.value();
            configMapIter++;

            // Reserve and resize
            (*newRule)->toActions(userPhaseIter->onEnd.onEndOfRulesCheck);


            userPhaseIter++;
        }else{ // Not exist - move to next phase
            configMapIter++;
            while(configMapIter != newPhasesMap.end() and configMapIter.key().second != ModifierRulesCategories::LowestValue){
                configMapIter++;
            }
            // Do not increment again
        }
    }
}


bool ControllerConfigInfo::addCategory(QString name, ModifierRulesFromConfigFileView& rulesView)
{
    qDebug() << "addCategory begin" << newPhasesMap;
    using Config = decltype(newPhasesMap);
    using ConfigKeyIter = Config::Iterator;
    using EditIndexIter = QMap<QPair<QString, qsizetype>, QMap<PhaseModifierRuleCategoryKey, qsizetype>::Iterator>::Iterator;
    //EditIndexIter editIndexIter = newNames.find(name);
    ConfigKeyIter configKeyIter;
    QString oldName;
    bool found = false;
    ModifierNewRules& rules = rulesView.rules;
    const qsizetype& category = static_cast<qsizetype>(rulesView.category);
    //configKeyIter = newPhasesMap.find({name, static_cast<ModifierRulesCategories>(category)});
    /*if(category > ModifierRulesCategories::PhaseRules){
        if(configKeyIter != newPhasesMap.end()){
            return false; // Duplicated
        }

        configKeyIter = newPhasesMap.insert({name, static_cast<RulesCategories>(index)}, 0);
        configKeyIter++;
        qsizetype value;
        if(configKeyIter != newPhasesMap.end()){
            value = configKeyIter.value();
        }else{
            value = newRules.size();
        }

        configKeyIter--;
        configKeyIter.value() = abs(value);
    }*/

    ConfigKeyIter beginIter;
    ConfigKeyIter endIter;

    beginIter = newPhasesMap.find({name, static_cast<ModifierRulesCategories>(category)});
    if(beginIter != newPhasesMap.end()){
        endIter = ++beginIter;
        beginIter--;
        qsizetype position = abs(beginIter.value());
        qsizetype numbOfRules = 0;
        if(endIter != newPhasesMap.end()){
            numbOfRules = abs(endIter.value()) - position;
        }else{
            numbOfRules = newModifierRules.size() - position;
        }
        qsizetype diff = rules.size() - numbOfRules;
        if(diff != 0){
            beginIter++;
            while(beginIter != newPhasesMap.end()){
                beginIter.value() += (beginIter.value() < 0)? -diff : diff;
                beginIter++;
            }
        }
        ModifierNewRules::Iterator tempRulesIter = newModifierRules.begin() + position;
        ModifierNewRules::Iterator newRulesIter = tempRulesIter + numbOfRules;
        for( ; tempRulesIter < newRulesIter; tempRulesIter++){
            delete *tempRulesIter;
        }
        newModifierRules.remove(position, numbOfRules);
        tempRulesIter = rules.begin();
        newModifierRules.insert(position, rules.size(), nullptr);
        newRulesIter = newModifierRules.begin() + position;
        for(; tempRulesIter < rules.end(); tempRulesIter++, newRulesIter++){
            *newRulesIter = *tempRulesIter;
        }
    }

    return true;
}



