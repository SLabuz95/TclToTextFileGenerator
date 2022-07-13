//#include"../controllerconfig.hpp"

#include"FCT_products.hpp"

using RuleType = RulesFactory::ProductTypeEnum;

template<>
RulesFactory::CreateFunctionTable RulesFactory::createFunctionTable =
{
    &RulesFactory::create<RuleType::RawRule>,
   // &RulesFactory::create<RuleType::RulesForArgument>
};


/*

void FactoryProductDefinition<RulesType>::ProductInterface::toXmlContent(QXmlStreamWriter& xmlWriter){
    // CONFIG_TAG
    // Control Flag
    // WARNING: Keep class abstract, this method is called only by subclass to add attribute to existing token (Keep that in mind)
    if(ProductInterface::controlFlag() != UserProcedureRule::defaultControlFlag()){
        QString controlFlagStr = UserProcedureRule::toStr(ProductInterface::controlFlag());
        Q_ASSERT_X(controlFlagStr.length() != 0, "RawRule::toXmlContent", "Internal error: Unknown ControlFlag");
        xmlWriter.writeAttribute("userInteraction", controlFlagStr);
    }
}



void Tcl2CaplControllerConfig::QuickRule::toXmlContent(QXmlStreamWriter& xmlWriter){
    // CONFIG_TAG
    // Create element , then add atributes for QuickRule and RawRule(ProductInterface) (call RawRule::toXmlContent)
    xmlWriter.writeStartElement("quickRule");
    ProductInterface::toXmlContent(xmlWriter);

    // Numb of Arguments
    if(numbOfArguments != -1){  // add attribute about numb of Arguments
        xmlWriter.writeAttribute("numbOfArguments", QString::number(numbOfArguments));
    }

    // Expected Arguments
    for(ExpectedArgumentsByIndex::Iterator expectedArgument = expectedArgumentsByIndex.begin(); expectedArgument != expectedArgumentsByIndex.end(); expectedArgument++){
        xmlWriter.writeStartElement("expectedArguments");
        xmlWriter.writeAttribute("index", QString::number(expectedArgument.key()));
        for(ExpectedArgumentsByIndex::mapped_type::Iterator argument = expectedArgument.value().begin(); argument != expectedArgument.value().end(); argument++){
            xmlWriter.writeTextElement("expectedArgument", (*argument));
        }
        xmlWriter.writeEndElement();
    }
    // Expected Arguments Without index
    if(not expectedArguments.isEmpty()){
        xmlWriter.writeStartElement("expectedArguments");
        for(ExpectedArgumentsWithoutIndex::Iterator argument = expectedArguments.begin(); argument != expectedArguments.end(); argument++){
            xmlWriter.writeTextElement("expectedArgument", (*argument));
        }
        xmlWriter.writeEndElement();
    }
    // Write Actions
    if(not writeActions_.isEmpty()){
        for(ExpectedArgumentsWithoutIndex::Iterator argument = expectedArguments.begin(); argument != expectedArguments.end(); argument++){
            xmlWriter.writeStartElement("writeAction");
            xmlWriter.writeTextElement("expectedArgument", (*argument));
            xmlWriter.writeEndElement();
        }
    }

    xmlWriter.writeEndElement();
}

void Tcl2CaplControllerConfig::AdvancedRule::toXmlContent(QXmlStreamWriter& xmlWriter){
    // CONFIG_TAG
    // Create element , then add atributes for QuickRule and RawRule(ProductInterface) (call RawRule::toXmlContent)
    xmlWriter.writeStartElement("advancedRule");
    ProductInterface::toXmlContent(xmlWriter);

    // Conditionals
    using Condition = ConditionalActions::Iterator;
    for(Condition condition = conditions().begin(); condition < conditions().end(); condition++){
        (*condition)->toXmlContent(xmlWriter);
    }

    // Executables
    using Executable = ExecutableActions::Iterator;
    for(Executable executable = executables().begin(); executable < executables().end(); executable++){
        (*executable)->toXmlContent(xmlWriter);
    }

    xmlWriter.writeEndElement();
}

void Tcl2CaplControllerConfig::RulesForArgument::toXmlContent(QXmlStreamWriter& xmlWriter){
    // CONFIG_TAG

    xmlWriter.writeStartElement("rulesForArgument");
    ProductInterface::toXmlContent(xmlWriter);

    if(index() != INDEX_NO_VALUE)
        xmlWriter.writeAttribute("index", QString::number(index_));


    // Dynamic Rules
    using DynamicRule = AdvancedRules::Iterator;
    for(DynamicRule dynamicRule = dynamicRules().begin(); dynamicRule < dynamicRules().end(); dynamicRule++){
        dynamicRule->toXmlContent(xmlWriter);
    }

    // Rules on Move
    using RuleOnMove = AdvancedRules::Iterator;
    for(RuleOnMove ruleOnMove = rulesOnMove().begin(); ruleOnMove < rulesOnMove().end(); ruleOnMove++){
        ruleOnMove->toXmlContent(xmlWriter);
    }

    xmlWriter.writeEndElement();
}

*/
