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
        using Actions = ModifierActions;
        xmlWriter.writeStartElement("rawModifierRule");
        xmlWriter.writeAttribute("controlFlag", FCT::ModifierRules::ModifierControlFlagInfo::toStr(controlFlag()));
        Actions::Iterator condition = conditions().begin();
        Actions::Iterator executable = executables().begin();
        xmlWriter.writeStartElement("modifierConditionalActions");
        for( ; condition < conditions().end(); condition++){
            (*condition)->toXmlContent(xmlWriter);
        }
        xmlWriter.writeEndElement(); // End of Container
        xmlWriter.writeStartElement("modifierExecutableActions");
        for( ; executable < executables().end(); executable++){
            (*executable)->toXmlContent(xmlWriter);
        }
        xmlWriter.writeEndElement(); // End of Container
        xmlWriter.writeEndElement();
    }
    void toActions(ModifierRule::Actions& actions) override{
        using Actions = ModifierRule::Actions;
        actions.reserve(executables().size());
        actions.resize(executables().size());
        Actions::Iterator actionsIter_ = actions.begin();
        ModifierActions::Iterator action = executables().begin();
        for( ; actionsIter_ < actions.end(); actionsIter_++, action++){
            (*action)->toAction(*actionsIter_);
        }

    } // For actions only (Rule is only container)


    virtual void actionsToXmlContent(QXmlStreamWriter& xmlWriter)override{
        using Actions = ModifierActions;
        Actions::Iterator executable = executables().begin();
        for( ; executable < executables().end(); executable++){
            (*executable)->toXmlContent(xmlWriter);
        }
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
