#include"FCT_products.hpp"

using Type = ParametersFactory::ProductTypeEnum;

template<>
ParametersFactory::CreateFunctionTable ParametersFactory::createFunctionTable =
{
    &ParametersFactory::create<Type::String>,
    &ParametersFactory::create<Type::FormattedString>,
    &ParametersFactory::create<Type::Integer>,
    &ParametersFactory::create<Type::List>,
};
