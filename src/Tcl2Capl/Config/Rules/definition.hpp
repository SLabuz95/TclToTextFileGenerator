#ifndef DEFINITION_RULES_HPP
#define DEFINITION_RULES_HPP

#include"TclInterpreter/tclToCAPL.hpp"
#include"External/Factory/productdef.hpp"
//#include"External/Factory/factory.hpp"

using TclProcedureInterpreter = Tcl::Interpreter::Command::Controller;
using Settings = Tcl::Interpreter::Command::Settings;
using UserProcedure = Tcl::Interpreter::Command::Definition;
using UserProcedureRule = UserProcedure::Rule;
using UserProcedureRules = QList<UserProcedureRule>;
using Mode = Settings::InterpreterMode;
using ControlFlag = UserProcedureRule::Control;


namespace Rules {
    enum class Type : uint{
        FCT_Begin,

        //QuickRule = FCT_Begin, // To generate raw rule
        RawRule  = FCT_Begin,   // Like in Interpreter
        RulesForArgument, // Raw Rule but for specific argument index only

        FCT_End,

    };
}

using RulesTypes = Rules::Type;
using RulesProductDefinition = ProductDefinition<RulesTypes>;
//using RulesFactory = Factory<RulesTypes>;


template <>
struct RulesProductDefinition::ImplementationData::Properties{
protected:
    //inline Properties(ControlFlag cf) : controlFlag_(cf){}
    ControlFlag controlFlag_;
};

template <>
class RulesProductDefinition::ImplementationData::Methods
        : public RulesProductDefinition::ImplementationData::Properties
{
public:
    //Methods(ControlFlag flag) : Properties(flag){}

};

template <>
class RulesProductDefinition::InterfaceData::Methods
    : public RulesProductDefinition::Implementation
{

public:
    inline void setControlFlag(ControlFlag cf){controlFlag_ = cf;}
    inline ControlFlag controlFlag()const{return controlFlag_;}

    virtual void toRule(UserProcedureRule&) = 0;
    //virtual void toXmlContent(QXmlStreamWriter& xmlWriter);
};

using RulesProducts = ProductsConfiguration<RulesTypes>;

#endif // DEFINITION_RULES_HPP
