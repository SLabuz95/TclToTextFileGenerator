#include"FCT_products.hpp"

using Type = ModifierFormatParametersFactory::ProductTypeEnum;

const QList<QString> ModifierFormatParameters::TypeInfo::typeMap =
{
    "Text",
    "FullLine",
    "Index",
    "ArgumentsFrom",
    "Target",
    "Attribute"
};

template<>
ModifierFormatParametersFactory::CreateFunctionTable ModifierFormatParametersFactory::createFunctionTable =
{
    &ModifierFormatParametersFactory::create<Type::TextItem>,
    &ModifierFormatParametersFactory::create<Type::FullLineItem>,
    &ModifierFormatParametersFactory::create<Type::IndexItem>,
    &ModifierFormatParametersFactory::create<Type::ArgumentsFromItem>,
    &ModifierFormatParametersFactory::create<Type::TargetItem>,
    &ModifierFormatParametersFactory::create<Type::AttributeItem>,
};
