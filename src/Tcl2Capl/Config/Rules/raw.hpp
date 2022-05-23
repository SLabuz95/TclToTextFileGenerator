#ifndef RAW_HPP
#define RAW_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Rules/definition.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"


template<>
template<>
struct RulesProducts::ImplementationData<RulesTypes::RawRule>::Properties
: protected RulesProductDefinition::Definition
{
public:
    using ConditionalActions = ConditionalsFactory::ListOfBases;
    using ExecutableActions = ExecutablesFactory::ListOfBases;

protected:
    ConditionalActions conditions_;
    ExecutableActions executables_;

};

template<>
template<>
class RulesProducts::ImplementationData<RulesTypes::RawRule>::Methods : public RulesProducts::ImplementationData<RulesTypes::RawRule>::Properties{

};

template<>
template<>
class RulesProducts::InterfaceData<RulesTypes::RawRule>::Methods : public RulesProducts::Implementation<RulesTypes::RawRule>{
    inline ConditionalActions& conditions(){return conditions_;}
    inline ExecutableActions& executables(){return executables_;}

    template<ConditionalsFactory::ProductTypeEnum productType, class ...Args>
    ConditionalsFactory::Product<productType>& addCondition(Args ...arg){
       conditions_.append(ConditionalsFactory::create<productType>(arg...));
       return conditions_.last();

    }
    template<ExecutablesFactory::ProductTypeEnum productType, class ...Args>
    ExecutablesFactory::Product<productType>& addExecutable(Args ...arg){
        executables_.append(ExecutablesFactory::create<productType>(arg...));
        return executables_.last();
    }

    template<class ...Args>
    ConditionalActions::Type addCondition(Args ...arg){
       //conditions_.append(ConditionalsFactory::create(arg...));
       //return conditions_.last();

    }
    template<class ...Args>
    ExecutableActions::Type addExecutable(Args ...arg){
        //executables_.append(ExecutablesFactory::create(arg...));
        //return executables_.last();
    }

   //void toRule(UserProcedureRule&) override;
   //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
};

#endif // RAW_HPP
