#ifndef DEFINITION_PARAMETERS_HPP
#define DEFINITION_PARAMETERS_HPP

#include"TclInterpreter/CommandsCont/commandsCont.hpp"
#include"External/Factory/factory.hpp"

using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
using Settings = Tcl::Interpreter::Command::Settings;
using UserProcedure = Tcl::Interpreter::Command::Definition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;

// Product Definition
namespace Parameters {
    enum Type : uint{
        FCT_Begin,

        String = FCT_Begin,
        FormattedString,
        Integer,
        List,   // List of Parameters

        FCT_End,

    };

}

using ParametersType = Parameters::Type;
using ParametersProductDefinition = ProductDefinition<ParametersType>;


template <>
struct ParametersProductDefinition::ImplementationData::Properties{

};

template <>
class ParametersProductDefinition::ImplementationData::Methods : protected ParametersProductDefinition::ImplementationData::Properties{

};

template <>
class ParametersProductDefinition::InterfaceData::Methods : public ParametersProductDefinition::Implementation{

    virtual void toXmlContent(QXmlStreamWriter& xmlWriter){};
    //inline virtual RawFormatType rawFormatType() const = 0;
};

using ParametersProducts = ProductsConfiguration<ParametersType>;

#endif // DEFINITION_PARAMETERS_HPP
