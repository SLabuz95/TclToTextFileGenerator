#ifndef DEFINITION_PARAMETERS_FORMATTED_HPP
#define DEFINITION_PARAMETERS_FORMATTED_HPP

#include"TclInterpreter/tclToCAPL.hpp"
#include"External/Factory/factory.hpp"

using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
using Settings = Tcl::Interpreter::Command::Settings;
using UserProcedure = Tcl::Interpreter::Command::Definition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;


namespace FormatParameters {
    enum Type : uint{
        FCT_Begin,

        INDEX_OR_FULL_LINE = FCT_Begin,
        ARGS_AFTER_INDEX,
        SEPARATOR,
        TARGET,

        FCT_End,

    };

}

using FormatParametersType = FormatParameters::Type;
using FormatParametersProductDefinition = ProductDefinition<FormatParametersType>;

template <>
struct FormatParametersProductDefinition::ImplementationData::Properties{

};

template <>
class FormatParametersProductDefinition::ImplementationData::Methods{

};

template <>
class FormatParametersProductDefinition::InterfaceData::Methods{

    virtual void toXmlContent(QXmlStreamWriter& xmlWriter){};
    //inline virtual RawFormatType rawFormatType() const = 0;
};

using FormatParametersProducts = ProductsConfiguration<FormatParametersType>;


#endif // DEFINITION_PARAMETERS_FORMATTED_HPP
