#ifndef RAW_HPP
#define RAW_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Rules/definition.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"


template<>
template<>
struct RulesProducts::ImplementationData<RulesTypes::RawRule>::Properties
: public RulesProductDefinition::Definition
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
class RulesProducts::ImplementationData<RulesTypes::RawRule>::Methods
: public RulesProducts::ImplementationData<RulesTypes::RawRule>::Properties
{

};

template<>
template<>
class RulesProducts::InterfaceData<RulesTypes::RawRule>::Methods
: public RulesProducts::Implementation<RulesTypes::RawRule>
{
public:
    inline ConditionalActions& conditions(){return conditions_;}
    inline ExecutableActions& executables(){return executables_;}

    void toRule(UserProcedureRule& rule)override{
        using Conditionals = UserProcedureRule::ConditionalActions;
        using Executables = UserProcedureRule::ExecutableActions;
        Conditionals conditionals_(conditions().size());
        Executables executables_(executables().size());
        Conditionals::Iterator conditionalsIter_ = conditionals_.begin();
        Executables::Iterator executablesIter_ = executables_.begin();
        ConditionalActions::Iterator condition = conditions().begin();
        ExecutableActions::Iterator executable = executables().begin();
        for( ; conditionalsIter_ < conditionals_.end(); conditionalsIter_++, condition++){
            (*condition)->toAction(*conditionalsIter_);
        }
        for( ; executablesIter_ < executables_.end(); executablesIter_++, executable++){
            (*executable)->toAction(*executablesIter_);
        }
        rule = UserProcedureRule(conditionals_, executables_, controlFlag());
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("rawRule");
        xmlWriter.writeAttribute("controlFlag", UserProcedureRule::toStr(controlFlag()));
        using Conditionals = UserProcedureRule::ConditionalActions;
        using Executables = UserProcedureRule::ExecutableActions;
        ConditionalActions::Iterator condition = conditions().begin();
        ExecutableActions::Iterator executable = executables().begin();
        for( ; condition < conditions().end(); condition++){
            (*condition)->toXmlContent(xmlWriter);
        }
        for( ; executable < executables().end(); executable++){
            (*executable)->toXmlContent(xmlWriter);
        }
        xmlWriter.writeEndElement();
    }
/*
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
*/
   //void toRule(UserProcedureRule&) override;
   //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
};

#endif // RAW_HPP
