#ifndef FILE_MODIFIER_RULES_RAW_HPP
#define FILE_MODIFIER_RULES_RAW_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Rules/definition.hpp"
#include"TcFileModifier/Config/Actions/FCT_products.hpp"


template<>
template<>
struct ModifierRulesProducts::ImplementationData<ModifierRulesTypes::RawRule>::Properties
: public ModifierRulesProductDefinition::Definition
{
public:
    using ModifierActions = ModifierActionsFactory::ListOfBases;

protected:
    ModifierActions actions_;

};

template<>
template<>
class ModifierRulesProducts::ImplementationData<ModifierRulesTypes::RawRule>::Methods
: public ModifierRulesProducts::ImplementationData<ModifierRulesTypes::RawRule>::Properties
{

};

template<>
template<>
class ModifierRulesProducts::InterfaceData<ModifierRulesTypes::RawRule>::Methods
: public ModifierRulesProducts::Implementation<ModifierRulesTypes::RawRule>
{
public:
    inline ModifierActions& actions(){return actions_;}
    //inline ModifierExecutableActions& executables(){return executables_;}

    void toRule(ModifierRule& rule)override{
        /*using Conditionals = UserProcedureRule::ModifierConditionalActions;
        using Executables = UserProcedureRule::ModifierExecutableActions;
        Conditionals conditionals_(conditions().size());
        Executables executables_(executables().size());
        Conditionals::Iterator conditionalsIter_ = conditionals_.begin();
        Executables::Iterator executablesIter_ = executables_.begin();
        ModifierConditionalActions::Iterator condition = conditions().begin();
        ModifierExecutableActions::Iterator executable = executables().begin();
        for( ; conditionalsIter_ < conditionals_.end(); conditionalsIter_++, condition++){
            (*condition)->toAction(*conditionalsIter_);
        }
        for( ; executablesIter_ < executables_.end(); executablesIter_++, executable++){
            (*executable)->toAction(*executablesIter_);
        }
        rule = UserProcedureRule(conditionals_, executables_, controlFlag());*/
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        /*xmlWriter.writeStartElement("rawRule");
        xmlWriter.writeAttribute("controlFlag", UserProcedureRule::toStr(controlFlag()));
        using Conditionals = UserProcedureRule::ModifierConditionalActions;
        using Executables = UserProcedureRule::ModifierExecutableActions;
        ModifierConditionalActions::Iterator condition = conditions().begin();
        ModifierExecutableActions::Iterator executable = executables().begin();
        for( ; condition < conditions().end(); condition++){
            (*condition)->toXmlContent(xmlWriter);
        }
        for( ; executable < executables().end(); executable++){
            (*executable)->toXmlContent(xmlWriter);
        }
        xmlWriter.writeEndElement();*/
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
    ModifierConditionalActions::Type addCondition(Args ...arg){
       //conditions_.append(ConditionalsFactory::create(arg...));
       //return conditions_.last();

    }
    template<class ...Args>
    ModifierExecutableActions::Type addExecutable(Args ...arg){
        //executables_.append(ExecutablesFactory::create(arg...));
        //return executables_.last();
    }
*/
   //void toRule(UserProcedureRule&) override;
   //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
};

#endif // FILE_MODIFIER_RULES_RAW_HPP
