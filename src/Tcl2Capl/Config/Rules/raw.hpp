#ifndef RAW_HPP
#define RAW_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Rules/definition.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct RulesProducts::ImplementationData<RulesTypes::RawRule>::Properties
: protected RulesProductDefinition::Definition
{
public:
    //using ConditionalActions = ConditionalsFactory::ListOfBases;
    //using ExecutableActions = ExecutablesFactory::ListOfBases;

protected:
    //ConditionalActions conditions_;
    //ExecutableActions executables_;

};

template<>
template<>
class RulesProducts::ImplementationData<RulesTypes::RawRule>::Methods : public RulesProducts::ImplementationData<RulesTypes::RawRule>::Properties{

};

template<>
template<>
class RulesProducts::InterfaceData<RulesTypes::RawRule>::Methods : public RulesProducts::Implementation<RulesTypes::RawRule>{
    //inline ConditionalActions& conditions(){return conditions_;}
    //inline ExecutableActions& executables(){return executables_;}
   //void toRule(UserProcedureRule&) override;
   //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
};

#endif // RAW_HPP
