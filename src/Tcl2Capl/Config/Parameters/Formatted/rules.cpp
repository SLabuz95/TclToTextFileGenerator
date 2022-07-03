#include"FCT_products.hpp"

using Type = FormatParametersFactory::ProductTypeEnum;

template<>
FormatParametersFactory::CreateFunctionTable FormatParametersFactory::createFunctionTable =
{
    &FormatParametersFactory::create<Type::TextItem>,
    &FormatParametersFactory::create<Type::NameItem>,
    &FormatParametersFactory::create<Type::IndexItem>,
    &FormatParametersFactory::create<Type::ArgumentsFromItem>,
    &FormatParametersFactory::create<Type::FormatItem>,
};
