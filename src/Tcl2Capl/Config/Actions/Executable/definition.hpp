#ifndef DEFINITION_EXECUTABLE_HPP
#define DEFINITION_EXECUTABLE_HPP

#include"TclInterpreter/tclToCAPL.hpp"
#include"External/Factory/factory.hpp"

using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
using Settings = Tcl::Interpreter::Command::Settings;
using UserProcedure = Tcl::Interpreter::Command::Definition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;


namespace ExecutableActions {
    using Type = UserProcedure::Action::Executable;
}

using ExecutablesTypes = ExecutableActions::Type;
using ExecutablesProductDefinition = ProductDefinition<ExecutablesTypes>;


template <>
struct ExecutablesProductDefinition::ImplementationData::Properties{

};

template <>
class ExecutablesProductDefinition::ImplementationData::Methods{

};

template <>
class ExecutablesProductDefinition::InterfaceData::Methods{
public:
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter) = 0;
    virtual void toAction(UserProcedureRule::ExecutableActions::Type&) = 0;
    //inline virtual RawFormatType rawFormatType() const = 0;
};

using ExecutablesProducts = ProductsConfiguration<ExecutablesTypes>;

#endif // DEFINITION_EXECUTABLE_HPP
