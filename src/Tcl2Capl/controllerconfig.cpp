#include"controllerconfig.hpp"
#include<QFileInfo>
#include<QXmlStreamWriter>

Tcl2CaplControllerConfig::Tcl2CaplControllerConfig(Tcl2CaplControllerConfigXmlData& xmlData)
{

}

Tcl2CaplControllerConfig::Procedure::Procedure(const Procedure& procedureRef, Name newName) : Procedure(procedureRef){
    name_ = newName;
}

Tcl2CaplControllerConfig::UserProcedure Tcl2CaplControllerConfig::Procedure::toProcedureWithRawRules()
{    
    using ProcedureName = UserProcedure::ProcedureName;
    using RulesForArguments = UserProcedure::RulesForArguments;
    using RulesForArgument = UserProcedure::RulesForArgument;
    using RulesOnEndOfCall = UserProcedure::RulesOnEndOfCall;

    using Rules = UserProcedureRules;

    RulesForArguments rulesForArguments;
    RulesForArgument rulesForArgument;
    RulesOnEndOfCall rulesOnEndOfCall;

    // Parse rulesForArguments and forArgument (unspecified)

    // Not implemented yet

    // Rules on end of call
    rulesOnEndOfCall = Rules(this->rulesOnEndOfCall().size());
    using RawRuleRef = RawRuleRefs::Iterator;
    RawRuleRef rawRuleRef = this->rulesOnEndOfCall().begin();
    Rules::Iterator newRule = rulesOnEndOfCall.begin();
    for(; rawRuleRef < this->rulesOnEndOfCall().end(); rawRuleRef++, newRule++)
    {
        (*rawRuleRef)->toRule((*newRule));
    }

    return UserProcedure(name(), rulesForArguments, rulesForArgument, rulesOnEndOfCall);
}


void Tcl2CaplControllerConfig::QuickRule::toRule(UserProcedureRule &rule)
{
    using ConditionalActions = Tcl2CaplControllerConfig::ConditionalActions;
    using ExecutableActions = Tcl2CaplControllerConfig::ExecutableActions;

    using ConditionalType = ConditionalActions::Type::ActionType;

    ConditionalActions conditions;
    ExecutableActions executables;

    // Parse conditionalActions

    if(numbOfArguments == 0){   // For no arguments condition - ignore expected arguments
        conditions = ConditionalActions{
                ConditionalActions::Type(
                        ConditionalType::CompareNumbOfArguments,
                        {
                            QString::number(0)
                        }
                    )
                };
    }else{
        // Calculate numb of conditions

        conditions = ConditionalActions(numbOfAcceptableConditions);

        // Build conditions
        if(numbOfArguments == -1){  // For any numb of arguments, numb of conditions equals numb of keys (indexes)
            using Index = ExpectedArgumentsByIndex::key_value_iterator;
            NumbOfAcceptableConditions currentCondition = 0;
            for(Index index = expectedArgumentsByIndex.keyValueBegin();
                index != expectedArgumentsByIndex.keyValueEnd(); index++)
            {
                if(not index->second.isEmpty()){
                    conditions[currentCondition] =
                            TclProcedureInterpreter::newCompareRule(
                                // Strings to Compare
                                index->second,
                                // Format to get string to compare
                                {
                                    UserProcedure::Format::FORMAT_RULE_CALL(),
                                    UserProcedure::Format::cast_format_rule_str(UserProcedure::Format::Rule::TARGET) +
                                        UserProcedure::Format::cast_target_str(UserProcedure::Format::Target::Command),
                                    UserProcedure::Format::cast_format_rule_str(UserProcedure::Format::Rule::INDEX_OR_FULL_LINE) +
                                        QString::number(index->first)
                                }
                                );
                    currentCondition++;
                }
            }
        }else{  // There is constraint for numb of arugments
            using Index = ExpectedArgumentsByIndex::key_value_iterator;
            for(Index index = expectedArgumentsByIndex.keyValueBegin(); index != expectedArgumentsByIndex.keyValueEnd(); index++)
            {
                if( not index->second.isEmpty() and index->first < numbOfArguments)
                {
                    // For negative indexes
                    if( index->first < 0 and numbOfArguments + index->first/*Negative value*/ < 0 ){
                         break;  // Constraints not satisifed
                    }

                    NumbOfAcceptableConditions currentCondition = 1;
                    conditions[0] = // For Numb of Arguments
                            ConditionalActions::Type(
                                    ConditionalType::CompareNumbOfArguments,
                                    {
                                        QString::number(numbOfArguments)
                                    }
                                );

                    conditions[currentCondition] =
                            TclProcedureInterpreter::newCompareRule(
                                // Strings to Compare
                                index->second,
                                // Format to get string to compare
                                {
                                    UserProcedure::Format::FORMAT_RULE_CALL(),
                                    UserProcedure::Format::cast_format_rule_str(UserProcedure::Format::Rule::TARGET) +
                                        UserProcedure::Format::cast_target_str(UserProcedure::Format::Target::Command),
                                    UserProcedure::Format::cast_format_rule_str(UserProcedure::Format::Rule::INDEX_OR_FULL_LINE) +
                                        QString::number(index->first)
                                }
                                );

                    currentCondition++; // Constraints satisfied
                }
            }
        }

    }
    // Parse executableActions
    executables = writeActions();


    rule = UserProcedureRule(conditions, executables, controlFlag());
}


void RulesFactory::Product<RulesType::RawRule>::toRule(UserProcedureRule &rule)
{
    using ConditionalActions = Rule::ConditionalActions;
    using ExecutableActions = Rule::ExecutableActions;
    ConditionalActions conditions;
    ExecutableActions executables;

    // TODO: AdvancedRule for CotrollerConfig required

    rule = Rule(conditions, executables, RulesFactory::FactoryCommonInterface::controlFlag());
}


void Tcl2CaplControllerConfig::RulesForArgument::toRule(UserProcedureRule &rule)
{

}

bool operator==(const Tcl2CaplControllerConfig::Procedure& lhs, const Tcl2CaplControllerConfig::Procedure& rhs){
    return lhs.name() == rhs.name();
}

bool Tcl2CaplControllerConfig::writeToFile(QFile file){
    QFileInfo fileInfo(file);

    //Negative tests
    if(fileInfo.completeSuffix() != "xml"){
        return false;
    }

    // Write


}

QString Tcl2CaplControllerConfig::toXmlContent(){
    QString xmlContent;
    QXmlStreamWriter xmlWriter(&xmlContent);

    xmlWriter.setAutoFormatting(true);

    // Start Doc
    xmlWriter.writeStartDocument();

    // Content
    // WriteOnlyProcedures Tag
    _settings.toXmlContent(xmlWriter);
    // Procedures
    _userProcedures.toXmlContent(xmlWriter);
    // Default Procedure
    _userDefaultProcedure.toXmlContent(xmlWriter);

    // End Doc
    xmlWriter.writeEndDocument();

    return xmlContent;
}

void Tcl2CaplControllerConfig::Procedure::toXmlContent(QXmlStreamWriter& xmlWriter){
    // CONFIG_TAG
    if(name().size() == 0){ // Default procedure
        xmlWriter.writeStartElement("defaultProcedure");
    }else{  // User Procedures
        xmlWriter.writeStartElement("procedure");
    }
    // User Interaction
    if(userInteraction != UserInteraction::defaultStatus()){
        QString userInteractionStr = TclProcedureInterpreter::toStr(userInteraction);
        Q_ASSERT_X(userInteractionStr.length() != 0, "Procedure::toXmlContent", "Internal error: Unknown UserInteractionStatus");
        xmlWriter.writeAttribute("userInteraction", userInteractionStr);
    }
    // Rules
    for(RawRuleRefs::Iterator ruleOnEndOfCall = rulesOnEndOfCall_.begin(); ruleOnEndOfCall != rulesOnEndOfCall_.end(); ruleOnEndOfCall++){
        (*ruleOnEndOfCall)->toXmlContent(xmlWriter);
    }
    for(RulesForArguments::Iterator ruleForArgument = rulesForArguments().begin(); ruleForArgument != rulesForArguments().end(); ruleForArgument++){
        ruleForArgument->toXmlContent(xmlWriter);
    }
    // End of Element
    xmlWriter.writeEndElement();

}




