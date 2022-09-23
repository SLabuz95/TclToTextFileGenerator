#include"FCT_products.hpp"

using RuleType = ModifierRulesFactory::ProductTypeEnum;

const QList<QString> FCT::ModifierRules::ModifierControlFlagInfo::modifierControlFlagMap =
{
    "BreakRuleCheck",
    "NoBreakRuleCheck",
    "BreakRuleCheckIgnoreOnEndActions",
};


template<>
ModifierRulesFactory::CreateFunctionTable ModifierRulesFactory::createFunctionTable =
{
    &ModifierRulesFactory::create<RuleType::RawRule>,
   // &RulesFactory::create<RuleType::RulesForArgument>
};
