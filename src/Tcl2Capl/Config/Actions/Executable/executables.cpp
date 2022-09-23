#include"executables.hpp"

using Executable = ExecutablesFactory::ProductTypeEnum;

template<>
ExecutablesFactory::CreateFunctionTable ExecutablesFactory::createFunctionTable =
{
    &ExecutablesFactory::create<Executable::Write>,
    &ExecutablesFactory::create<Executable::TclParse>,
    &ExecutablesFactory::create<Executable::Error>,
    &ExecutablesFactory::create<Executable::AddPreExpression>,
    &ExecutablesFactory::create<Executable::AddUserInteraction>,
    &ExecutablesFactory::create<Executable::AddPredefinition>,
    &ExecutablesFactory::create<Executable::WriteAttribute>,
};
