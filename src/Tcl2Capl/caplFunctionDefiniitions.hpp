#ifndef CAPLFUNCTIONDEFINIITIONS_HPP
#define CAPLFUNCTIONDEFINIITIONS_HPP
#include"TclInterpreter/tclToCAPL.hpp"


class FunctionDefinitions{
public:
    using Parameters = QList<Stat>;
    using Example = QString;
    using Examples = QList<Example>;
    using ProcedureInfos = QMap<Parameters, Examples>;
    using DefinitionInfo = QMap<TCLCommandsController::ProcedureCall::Parameters::size_type, ProcedureInfos>; // <Numb of Parameters, Examples/ Occurencies>
    using Definitions = QMap<QString, DefinitionInfo>;    // <Name, Info>

protected:
    Definitions definitionsOnNoRules;
    Definitions definitionsOnNotSatisfiedRules;

public:

    void addDefinitionNoRules(TCLCommandsController::ProcedureCall& procedureCall);
    void addDefinitionNotSatisfiedRules(TCLCommandsController::ProcedureCall& procedureCall);

    void writeCaplFunctionDefinitions(QFile &file);

     Definitions& definitionsOnNoRulesView(){return definitionsOnNoRules;}
     Definitions& definitionsOnNotSatisfiedRulesView(){return definitionsOnNotSatisfiedRules;}

};



#endif // CAPLFUNCTIONDEFINIITIONS_HPP
