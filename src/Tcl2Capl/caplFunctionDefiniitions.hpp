#ifndef CAPLFUNCTIONDEFINIITIONS_HPP
#define CAPLFUNCTIONDEFINIITIONS_HPP
#include"tclToCAPL.hpp"

class CAPLFunctionDefinitions{
    using DefinitionInfo = QMap<TCLInterpreter::TCLProceduresInterpreter::ProcedureCall::Parameters::size_type, QSet<QString>>; // <Numb of Parameters, Examples/ Occurencies>
    using Definitions = QMap<QString, DefinitionInfo>;    // <Name, Info>

protected:
    Definitions definitions;

public:
    void addDefinition(TCLInterpreter::TCLProceduresInterpreter::ProcedureCall procedureCall);

    void writeCaplFunctionDefinitions(QFile &file);
};
using CAPLFunctionDefinitionsRef = CAPLFunctionDefinitions&;

#endif // CAPLFUNCTIONDEFINIITIONS_HPP
