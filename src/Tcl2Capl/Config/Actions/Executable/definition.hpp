#ifndef DEFINITION_HPP
#define DEFINITION_HPP

#include"tclToCAPL.hpp"
#include"External/Factory/factory.hpp"

using TclProcedureInterpreter = TCLInterpreter::TCLProceduresInterpreter;
using Settings = TclProcedureInterpreter::ProdecuresSettings;
using UserProcedure = TclProcedureInterpreter::ProcedureDefinition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;


namespace ExecutableActions {
    using Type = UserProcedure::Action::Executable;
}

using ExecutablesTypes = ExecutableActions::Type;
using ExecutablesProductDefinition = ProductDefinition<ExecutablesTypes>;
using ExecutablesFactory = Factory<ExecutablesTypes>;


template <>
struct ExecutablesProductDefinition::ImplementationData::Properties{

};

template <>
class ExecutablesProductDefinition::ImplementationData::Methods{

};

template <>
class ExecutablesProductDefinition::InterfaceData::Methods{

    virtual void toXmlContent(QXmlStreamWriter& xmlWriter){};
    //inline virtual RawFormatType rawFormatType() const = 0;
};

#endif // DEFINITION_HPP
