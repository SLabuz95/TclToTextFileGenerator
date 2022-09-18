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
    ModifierActions conditionals_;
    ModifierActions executables_;

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
    inline ModifierActions& conditions(){return conditionals_;}
    inline ModifierActions& executables(){return executables_;}

    void toRule(ModifierRule& rule)override{
        using Actions = ModifierRule::Actions;
        Actions conditionals_(conditions().size());
        Actions executables_(executables().size());
        Actions::Iterator conditionalsIter_ = conditionals_.begin();
        Actions::Iterator executablesIter_ = executables_.begin();
        ModifierActions::Iterator condition = conditions().begin();
        ModifierActions::Iterator executable = executables().begin();
        for( ; conditionalsIter_ < conditionals_.end(); conditionalsIter_++, condition++){
            (*condition)->toAction(*conditionalsIter_);
        }
        for( ; executablesIter_ < executables_.end(); executablesIter_++, executable++){
            (*executable)->toAction(*executablesIter_);
        }
        rule = ModifierRule{conditionals_, executables_, controlFlag()};
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
    void toActions(ModifierRule::Actions& actions) override{
        using Actions = ModifierRule::Actions;
        actions.reserve(conditions().size());
        actions.resize(conditions().size());
        Actions::Iterator conditionalsIter_ = actions.begin();
        ModifierActions::Iterator condition = conditions().begin();
        for( ; conditionalsIter_ < actions.end(); conditionalsIter_++, condition++){
            (*condition)->toAction(*conditionalsIter_);
        }

    } // For actions only (Rule is only container)
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
