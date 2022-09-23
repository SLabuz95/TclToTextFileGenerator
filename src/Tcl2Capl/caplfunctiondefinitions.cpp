#include"caplFunctionDefiniitions.hpp"
#include<QDebug>
#include<QFile>

using namespace Tcl::Analysis;

void FunctionDefinitions::addDefinitionNoRules(Call& procedureCall)
{
    Definitions::Iterator caplDefinition;
    if((caplDefinition = definitionsOnNoRules.find(procedureCall._name())) != definitionsOnNoRules.end()){
        DefinitionInfo::Iterator caplDefinitionInfo;
        if((caplDefinitionInfo = caplDefinition->find(procedureCall.parametersLength())) != caplDefinition->end()){
            // Search parameters
            ProcedureInfos::Iterator procedureInfo;
            Parameters parameters = procedureCall.parametersStats();
            if((procedureInfo = caplDefinitionInfo->find(parameters)) != caplDefinitionInfo->end()){
                Example example = procedureCall.generateFunctionDefinitionExample();
                if(not procedureInfo.value().contains(example)){
                    procedureInfo.value().append(example);
                }
            }else{
                Examples examples{procedureCall.generateFunctionDefinitionExample()};
                caplDefinitionInfo->insert(parameters, examples);
            }
        }else{
            ProcedureInfos procedureInfos;
            Examples examples{procedureCall.generateFunctionDefinitionExample()};
            procedureInfos.insert(procedureCall.parametersStats(), examples);
            caplDefinition->insert(procedureCall.parametersLength(), procedureInfos);
        }
    }else{
        DefinitionInfo info;
        ProcedureInfos procedureInfos;
        Examples examples{procedureCall.generateFunctionDefinitionExample()};
        procedureInfos.insert(procedureCall.parametersStats(), examples);
        info.insert(procedureCall.parametersLength(), procedureInfos);
        definitionsOnNoRules.insert(procedureCall._name(), info);
    }
}

void FunctionDefinitions::addDefinitionNotSatisfiedRules(Call& procedureCall)
{
    Definitions::Iterator caplDefinition;
    if((caplDefinition = definitionsOnNotSatisfiedRules.find(procedureCall._name())) != definitionsOnNotSatisfiedRules.end()){
        DefinitionInfo::Iterator caplDefinitionInfo;
        if((caplDefinitionInfo = caplDefinition->find(procedureCall.parametersLength())) != caplDefinition->end()){
            // Search parameters
            ProcedureInfos::Iterator procedureInfo;
            Parameters parameters = procedureCall.parametersStats();
            if((procedureInfo = caplDefinitionInfo->find(parameters)) != caplDefinitionInfo->end()){
                Example example = procedureCall.generateFunctionDefinitionExample();
                if(not procedureInfo.value().contains(example)){
                    procedureInfo.value().append(example);
                }
            }else{
                Examples examples{procedureCall.generateFunctionDefinitionExample()};
                caplDefinitionInfo->insert(parameters, examples);
            }
        }else{
            ProcedureInfos procedureInfos;
            Examples examples{procedureCall.generateFunctionDefinitionExample()};
            procedureInfos.insert(procedureCall.parametersStats(), examples);
            caplDefinition->insert(procedureCall.parametersLength(), procedureInfos);
        }
    }else{
        DefinitionInfo info;
        ProcedureInfos procedureInfos;
        Examples examples{procedureCall.generateFunctionDefinitionExample()};
        procedureInfos.insert(procedureCall.parametersStats(), examples);
        info.insert(procedureCall.parametersLength(), procedureInfos);
        definitionsOnNotSatisfiedRules.insert(procedureCall._name(), info);
    }
}

void FunctionDefinitions::writeCaplFunctionDefinitions(QFile &file){
    if(not definitionsOnNoRules.isEmpty()) {
        const QString newFunction = "// _NEW_FUNC_\n";
        const QString startOfExamples = "// EXAMPLES\n/*\n";
        const QString endOfExamples = "\n*/\n// END OF EXAMPLES\n";
        if(not file.isOpen()){
            if(not file.open(QIODevice::Text | QIODevice::Append))
                return;
            file.write("includes{\n}\n\nvariables{\n}\n\n");
        }
        for(Definitions::Iterator caplDefinition = definitionsOnNoRules.begin();
            caplDefinition != definitionsOnNoRules.end();
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
                ProcedureInfos& procedureInfos = caplDefinitionInfo.value();
                for(ProcedureInfos::Iterator procedureInfo = procedureInfos.begin();
                    procedureInfo != procedureInfos.end();
                    procedureInfo++)
                {
                    file.write(procedureInfo.value().join("\n").toUtf8());
                }
                file.write(endOfExamples.toUtf8());
            }
        }
        file.close();
    }
}
