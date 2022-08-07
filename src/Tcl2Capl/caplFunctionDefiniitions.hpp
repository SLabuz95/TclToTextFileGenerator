#ifndef CAPLFUNCTIONDEFINIITIONS_HPP
#define CAPLFUNCTIONDEFINIITIONS_HPP
#include"TclInterpreter/CommandsCont/commandCall.hpp"
#include<QMap>

class QFile;
namespace Tcl::Analysis{
    using namespace Tcl::Interpreter;
    using namespace Tcl::Interpreter::Command;
    class FunctionDefinitions{
    public:
        using Parameters = QList<Stat>;
        using Example = QString;
        using Examples = QList<Example>;
        using ProcedureInfos = QMap<Parameters, Examples>;
        using DefinitionInfo = QMap<Call::Parameters::size_type, ProcedureInfos>; // <Numb of Parameters, Examples/ Occurencies>
        using Definitions = QMap<QString, DefinitionInfo>;    // <Name, Info>

    protected:
        Definitions definitionsOnNoRules;
        Definitions definitionsOnNotSatisfiedRules;

    public:

        void addDefinitionNoRules(Call& procedureCall);
        void addDefinitionNotSatisfiedRules(Call& procedureCall);

        void writeCaplFunctionDefinitions(QFile &file);

         Definitions& definitionsOnNoRulesView(){return definitionsOnNoRules;}
         Definitions& definitionsOnNotSatisfiedRulesView(){return definitionsOnNotSatisfiedRules;}

    };

    using FunctionDefinitionsRef = FunctionDefinitions&;

};

#endif // CAPLFUNCTIONDEFINIITIONS_HPP
