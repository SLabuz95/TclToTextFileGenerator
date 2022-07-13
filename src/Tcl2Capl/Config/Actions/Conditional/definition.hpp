#ifndef DEFINITION_CONDITIONAL_HPP
#define DEFINITION_CONDITIONAL_HPP

#include"TclInterpreter/tclToCAPL.hpp"
#include"External/Factory/productdef.hpp"

using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
using Settings = Tcl::Interpreter::Command::Settings;
using UserProcedure = Tcl::Interpreter::Command::Definition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;


namespace ConditionalActions {
    using Type = UserProcedure::Action::Conditional;
}

using ConditionalsTypes = ConditionalActions::Type;
using ConditionalsProductDefinition = ProductDefinition<ConditionalsTypes>;
//using ConditionalsFactory = Factory<ConditionalsTypes>;


template <>
struct ConditionalsProductDefinition::ImplementationData::Properties{

};

template <>
class ConditionalsProductDefinition::ImplementationData::Methods{

};

template <>
class ConditionalsProductDefinition::InterfaceData::Methods{
public:
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter){};
    virtual void toAction(UserProcedureRule::ConditionalActions::Type&) = 0;
    //inline virtual RawFormatType rawFormatType() const = 0;
};

using ConditionalsProducts = ProductsConfiguration<ConditionalsTypes>;

#endif // DEFINITION_CONDITIONAL_HPP
