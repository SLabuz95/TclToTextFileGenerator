#ifndef FILE_MODIFIER_RULES_DEFINITION_HPP
#define FILE_MODIFIER_RULES_DEFINITION_HPP

#include"TcFileModifier/tcfilemodifierconfigbase.hpp"
#include"External/Factory/productdef.hpp"
//#include"External/Factory/factory.hpp"

using ModifierControlFlag = TcFileModifierConfigBase::ModifierRuleControl;
using ModifierRule = TcFileModifierConfigBase::ModifierRule;
class QXmlStreamWriter;
namespace ModifierRules {
    enum class Type : uint{
        FCT_Begin,

        //QuickRule = FCT_Begin, // To generate raw rule
        RawRule  = FCT_Begin,   // Like in Interpreter
        //RulesForArgument, // Raw Rule but for specific argument index only

        FCT_End,

    };
}

using ModifierRulesTypes = ModifierRules::Type;
using ModifierRulesProductDefinition = ProductDefinition<ModifierRulesTypes>;
//using RulesFactory = Factory<RulesTypes>;


template <>
struct ModifierRulesProductDefinition::ImplementationData::Properties{
protected:
    //inline Properties(ControlFlag cf) : controlFlag_(cf){}
    ModifierControlFlag controlFlag_;
};

template <>
class ModifierRulesProductDefinition::ImplementationData::Methods
        : public ModifierRulesProductDefinition::ImplementationData::Properties
{
public:
    //Methods(ControlFlag flag) : Properties(flag){}

};

template <>
class ModifierRulesProductDefinition::InterfaceData::Methods
    : public ModifierRulesProductDefinition::Implementation
{

public:
    inline void setControlFlag(ModifierControlFlag cf){controlFlag_ = cf;}
    inline ModifierControlFlag controlFlag()const{return controlFlag_;}

    virtual void toRule(ModifierRule&) = 0;
    virtual void toActions(ModifierRule::Actions&) = 0; // For actions only (Rule is only container)
    virtual void toXmlContent(QXmlStreamWriter& xmlWriter) = 0;
};

using ModifierRulesProducts = ProductsConfiguration<ModifierRulesTypes>;

#endif // FILE_MODIFIER_RULES_DEFINITION_HPP
