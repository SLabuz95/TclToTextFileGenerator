#include"FCT_products.hpp"

using ActionType = ModifierActionsFactory::ProductTypeEnum;


const QList<QString> ModifierActions::TypeInfo::typeMap =
{
    "Split",
    "StartsWith",
    "Compare",
    "Write",
    "ChangePhase",
    "Interpret",
};

template<>
ModifierActionsFactory::CreateFunctionTable ModifierActionsFactory::createFunctionTable =
{
    &ModifierActionsFactory::create<ActionType::Split>,
    &ModifierActionsFactory::create<ActionType::StartsWith>,
    &ModifierActionsFactory::create<ActionType::Compare>,
    &ModifierActionsFactory::create<ActionType::Write>,
    &ModifierActionsFactory::create<ActionType::ChangePhase>,
    &ModifierActionsFactory::create<ActionType::Interpret>,
};
