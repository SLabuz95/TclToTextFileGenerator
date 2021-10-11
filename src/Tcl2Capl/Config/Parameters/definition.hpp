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
using ParametersFactory = Factory<ParametersType>;


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


#endif // DEFINITION_HPP
