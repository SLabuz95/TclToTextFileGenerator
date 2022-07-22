#include"controllerconfiginfo.hpp"
#include<QXmlStreamWriter>

void ControllerConfigInfo::writeProceduresToXML(QXmlStreamWriter& xmlWriter){
    // Prepare Token

    using Config = decltype(newProceduresMap);
    using ConfigKeyIter = Config::Iterator;
    using ProcedureName = UserProcedure::ProcedureName;
    using Rules = UserProcedureRules;

    xmlWriter.writeStartElement("procedures");

    QString name;
    qsizetype numbOfRules;
    NewRules::Iterator newRule;
    NewRules::Iterator endRule;
    qsizetype numbOfIndexes;
    // Prepare containers
    // On end of Call
    ConfigKeyIter configProcedureStartIter = newProceduresMap.begin();
    ConfigKeyIter configMapIter = configProcedureStartIter;

    while(configMapIter != newProceduresMap.end()){
        configProcedureStartIter = configMapIter;
        name = configMapIter.key().first;
        if(configMapIter.value() >= 0){ // Check if Exists
            // Write Procedure element
            xmlWriter.writeStartElement("procedure");
            xmlWriter.writeAttribute("name", name);

            // Write OnEndOfCallRules
            xmlWriter.writeStartElement("rulesOnEndOfCall");

            newRule = newRules.begin() + configMapIter.value();
            configMapIter++;
            endRule = newRules.begin() + configMapIter.value(); // Can be read cause Undefined Argument cant be negative

            for(; newRule < endRule; newRule++)
            {
                (*newRule)->toXmlContent(xmlWriter);
            }

            xmlWriter.writeEndElement(); // -----------------

            // Undefined arguments
            xmlWriter.writeStartElement("rulesForUndefinedArgument");

            newRule = newRules.begin() + configMapIter.value();
            configMapIter++;
            if(configMapIter != newProceduresMap.end()){
                endRule = newRules.begin() + abs(configMapIter.value());
            }else{
                endRule = newRules.begin() + newRules.size();
            }
            numbOfRules = endRule - newRule;
            for(; newRule < endRule; newRule++)
            {
                (*newRule)->toXmlContent(xmlWriter);
            }

            xmlWriter.writeEndElement(); // -----------------
            // Rules for arguments
            numbOfIndexes = 0;
            while(configMapIter != newProceduresMap.end() and configMapIter.key().second != RulesCategories::OnEndOfCall){
                if(configMapIter.value() >= 0) // Index exists
                    numbOfIndexes++;
                configMapIter++;
            }

            configMapIter = configProcedureStartIter; // On End
            configMapIter++; // Undefined
            configMapIter++; // First Index
            while(configMapIter != newProceduresMap.end() and configMapIter.key().second != RulesCategories::OnEndOfCall){
                if(configMapIter.value() >= 0) // Index exists
                {
                    // Set Index
                    xmlWriter.writeStartElement("rulesForArgument");
                    xmlWriter.writeAttribute("index", QString::number(configMapIter.key().second));
                    xmlWriter.writeStartElement("rulesOnMove");

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

                    for(; newRule < endRule; newRule++)
                    {
                        (*newRule)->toXmlContent(xmlWriter);
                    }
                    xmlWriter.writeEndElement(); // --- rulesOnMove --------------
                    xmlWriter.writeEndElement(); // --- rulesForArgument --------------
                }else{
                    configMapIter++;
                }
            }
            xmlWriter.writeEndElement(); // --- procedure --------------

        }else{ // Not exist - move to next procedure
            while(configMapIter != newProceduresMap.end() and configMapIter.key().second != RulesCategories::OnEndOfCall){
                configMapIter++;
            }
            // Do not increment again
        }
    }
    xmlWriter.writeEndElement();
}

void ControllerConfigInfo::writeDefaultProcedureToXML(QXmlStreamWriter& xmlWriter){
    // Prepare Token
    xmlWriter.writeStartElement("defaultProcedure");

    // Prepare data
    using Config = decltype(newDefaultProcedureMap);
    using ConfigKeyIter = Config::Iterator;
    using ProcedureName = UserProcedure::ProcedureName;

    using Rules = UserProcedureRules;
    qsizetype numbOfRules = 0;

    // Prepare containers
    // On end of Call
    ConfigKeyIter configMapIter = newDefaultProcedureMap.begin();
    configMapIter++;
    numbOfRules = configMapIter.value();
    NewRules::Iterator newRule = newDefaultProcedureRules.begin();
    NewRules::Iterator endRule = newRule + numbOfRules;
    xmlWriter.writeStartElement("rulesOnEndOfCall");

    for(; newRule < endRule; newRule++)
    {
        (*newRule)->toXmlContent(xmlWriter);
    }
    xmlWriter.writeEndElement(); // --- rulesOnEndOfCall --------------
    // Undefined Argument
    configMapIter++;
    numbOfRules = (configMapIter != newDefaultProcedureMap.end())?
                abs(configMapIter.value()) - numbOfRules :
                newDefaultProcedureRules.size() - numbOfRules;

    newRule = endRule;
    endRule = newRule + numbOfRules;
    xmlWriter.writeStartElement("rulesForUndefinedArgument");

    for(; newRule < endRule; newRule++)
    {
        (*newRule)->toXmlContent(xmlWriter);
    }

    xmlWriter.writeEndElement(); // --- rulesForUndefinedArgument --------------
    // Rules for arguments
    while(configMapIter != newDefaultProcedureMap.end()){
        if(configMapIter.value() >= 0){
            // Set Index
            xmlWriter.writeStartElement("rulesForArgument");
            xmlWriter.writeAttribute("index", QString::number(configMapIter.key()));
            xmlWriter.writeStartElement("rulesOnMove");

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

            for(; newRule < endRule; newRule++)
            {
                (*newRule)->toXmlContent(xmlWriter);
            }
            xmlWriter.writeEndElement(); // --- rulesOnMove --------------
            xmlWriter.writeEndElement(); // --- rulesForArgument --------------
        }else{
            configMapIter++;
        }
    }

    // Close Attribute
    xmlWriter.writeEndElement(); // --- defaultProcedure --------------

}

void ControllerConfigInfo::writeSettingsToXML(QXmlStreamWriter& xmlWriter){
      settings().toXmlContent(xmlWriter);
}

void ControllerConfigInfo::toXmlContent(QXmlStreamWriter& xmlWriter){
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("userConfig");

    writeSettingsToXML(xmlWriter);
    writeProceduresToXML(xmlWriter);
    writeDefaultProcedureToXML(xmlWriter);

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
}
