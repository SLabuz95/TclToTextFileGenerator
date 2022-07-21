#include"conditionals.hpp"

using Conditional = ConditionalsFactory::ProductTypeEnum;

template<>
ConditionalsFactory::CreateFunctionTable ConditionalsFactory::createFunctionTable =
{
    &ConditionalsFactory::create<Conditional::CompareNumbOfArguments>,
    &ConditionalsFactory::create<Conditional::Compare>,
    //&ConditionalsFactory::create<Conditional::IsLastSavedStat>,
    &ConditionalsFactory::create<Conditional::CompareAndWrite>,
};
