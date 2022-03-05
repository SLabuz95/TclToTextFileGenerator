#ifndef CAPLFUNCTIONDEFINIITIONS_HPP
#define CAPLFUNCTIONDEFINIITIONS_HPP
#include"tclToCAPL.hpp"

class FunctionDefinitions{
public:
    using Parameters = QList<TCLInterpreter::Stat>;
    using Example = QString;
    using Examples = QList<Example>;
    using ProcedureInfos = QMap<Parameters, Examples>;
    using DefinitionInfo = QMap<TCLInterpreter::TCLProceduresInterpreter::ProcedureCall::Parameters::size_type, ProcedureInfos>; // <Numb of Parameters, Examples/ Occurencies>
    using Definitions = QMap<QString, DefinitionInfo>;    // <Name, Info>

protected:
    Definitions definitionsOnNoRules;
    Definitions definitionsOnNotSatisfiedRules;

public:

    void addDefinitionNoRules(TCLInterpreter::TCLProceduresInterpreter::ProcedureCall& procedureCall);
    void addDefinitionNotSatisfiedRules(TCLInterpreter::TCLProceduresInterpreter::ProcedureCall& procedureCall);

    void writeCaplFunctionDefinitions(QFile &file);

     Definitions& definitionsOnNoRulesView(){return definitionsOnNoRules;}
     Definitions& definitionsOnNotSatisfiedRulesView(){return definitionsOnNotSatisfiedRules;}

};



#endif // CAPLFUNCTIONDEFINIITIONS_HPP
