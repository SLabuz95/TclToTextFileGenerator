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


namespace ConditionalActions {
    using Type = UserProcedure::Action::Conditional;
}

using ConditionalsTypes = ConditionalActions::Type;
using ConditionalsProductDefinition = ProductDefinition<ConditionalsTypes>;
using ConditionalsFactory = Factory<ConditionalsTypes>;


template <>
struct ConditionalsProductDefinition::ImplementationData::Properties{

};

template <>
class ConditionalsProductDefinition::ImplementationData::Methods{

};

template <>
class ConditionalsProductDefinition::InterfaceData::Methods{

    virtual void toXmlContent(QXmlStreamWriter& xmlWriter){};
    //inline virtual RawFormatType rawFormatType() const = 0;
};


#endif // DEFINITION_HPP
