#include"caplFunctionDefiniitions.hpp"

void CAPLFunctionDefinitions::addDefinitionNoRules(TCLInterpreter::TCLProceduresInterpreter::ProcedureCall& procedureCall)
{
    Definitions::Iterator caplDefinition;
    if((caplDefinition = definitionsOnNoRules.find(procedureCall.name())) != definitionsOnNoRules.end()){
        DefinitionInfo::Iterator caplDefinitionInfo;
        if((caplDefinitionInfo = caplDefinition->find(procedureCall.parametersLength())) != caplDefinition->end()){
            // Search parameters
            if(procedureCall.name() == "test_result" and procedureCall.parametersLength() == 2)
                qDebug() << "Test";
            ProcedureInfos::Iterator procedureInfo;
            Parameters parameters = procedureCall.parametersStats();
            if((procedureInfo = caplDefinitionInfo->find(parameters)) != caplDefinitionInfo->end()){
                Example example = procedureCall.generateCAPLFunctionDefinitionExample();
                if(not procedureInfo.value().contains(example)){
                    procedureInfo.value().append(example);
                }
            }else{
                Examples examples{procedureCall.generateCAPLFunctionDefinitionExample()};
                caplDefinitionInfo->insert(parameters, examples);
            }
        }else{
            ProcedureInfos procedureInfos;
            Examples examples{procedureCall.generateCAPLFunctionDefinitionExample()};
            procedureInfos.insert(procedureCall.parametersStats(), examples);
            caplDefinition->insert(procedureCall.parametersLength(), procedureInfos);
        }
    }else{
        DefinitionInfo info;
        ProcedureInfos procedureInfos;
        Examples examples{procedureCall.generateCAPLFunctionDefinitionExample()};
        procedureInfos.insert(procedureCall.parametersStats(), examples);
        info.insert(procedureCall.parametersLength(), procedureInfos);
        definitionsOnNoRules.insert(procedureCall.name(), info);
    }
}

void CAPLFunctionDefinitions::addDefinitionNotSatisfiedRules(TCLInterpreter::TCLProceduresInterpreter::ProcedureCall& procedureCall)
{
    Definitions::Iterator caplDefinition;
    if((caplDefinition = definitionsOnNotSatisfiedRules.find(procedureCall.name())) != definitionsOnNotSatisfiedRules.end()){
        DefinitionInfo::Iterator caplDefinitionInfo;
        if((caplDefinitionInfo = caplDefinition->find(procedureCall.parametersLength())) != caplDefinition->end()){
            if(procedureCall.name() == "test_result" and procedureCall.parametersLength() == 2)
                qDebug() << "Test";
            // Search parameters
            ProcedureInfos::Iterator procedureInfo;
            Parameters parameters = procedureCall.parametersStats();
            if((procedureInfo = caplDefinitionInfo->find(parameters)) != caplDefinitionInfo->end()){
                Example example = procedureCall.generateCAPLFunctionDefinitionExample();
                if(not procedureInfo.value().contains(example)){
                    procedureInfo.value().append(example);
                }
            }else{
                Examples examples{procedureCall.generateCAPLFunctionDefinitionExample()};
                caplDefinitionInfo->insert(parameters, examples);
            }
        }else{
            ProcedureInfos procedureInfos;
            Examples examples{procedureCall.generateCAPLFunctionDefinitionExample()};
            procedureInfos.insert(procedureCall.parametersStats(), examples);
            caplDefinition->insert(procedureCall.parametersLength(), procedureInfos);
        }
    }else{
        DefinitionInfo info;
        ProcedureInfos procedureInfos;
        Examples examples{procedureCall.generateCAPLFunctionDefinitionExample()};
        procedureInfos.insert(procedureCall.parametersStats(), examples);
        info.insert(procedureCall.parametersLength(), procedureInfos);
        definitionsOnNotSatisfiedRules.insert(procedureCall.name(), info);
    }
}

void CAPLFunctionDefinitions::writeCaplFunctionDefinitions(QFile &file){
    /*if(not definitions.isEmpty()) {
        const QString newFunction = "// _NEW_FUNC_\n";
        const QString startOfExamples = "// EXAMPLES\n";
        const QString endOfExamples = "\n// END OF EXAMPLES\n";
        if(not file.isOpen()){
            if(not file.open(QIODevice::Text | QIODevice::Append))
                return;
            file.write("includes{\n}\n\nvariables{\n}\n\n");
        }
        for(Definitions::Iterator caplDefinition = definitions.begin();
            caplDefinition != definitions.end();
            caplDefinition++)
        {
            QString name = caplDefinition.key();
            for(DefinitionInfo::Iterator caplDefinitionInfo = caplDefinition->begin();
                caplDefinitionInfo != caplDefinition->end();
                caplDefinitionInfo++)
            {
                DefinitionInfo::key_type numbOfParameters = caplDefinitionInfo.key();
                QString definitionStr = QString("/// <") +  name + ">\n" + name + "(";
                for(DefinitionInfo::key_type paramNumb = 0; paramNumb < numbOfParameters; paramNumb++)
                    definitionStr += QString("int arg") + QString::number(paramNumb+1) + QString(", ");
                if(numbOfParameters > 0)
                    definitionStr.chop(QStringLiteral(", ").length());
                definitionStr += "){} " + newFunction;

                // Write Definition
                file.write(definitionStr.toUtf8());

                // Examples
                file.write(startOfExamples.toUtf8());
                file.write(caplDefinitionInfo->values().join("\n").toUtf8());
                file.write(endOfExamples.toUtf8());
            }
        }
        file.close();
    }*/
}
