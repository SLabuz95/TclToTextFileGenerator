#ifndef FILE_MODIFIER_ACTION_STARTSWITH_HPP
#define FILE_MODIFIER_ACTION_STARTSWITH_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::StartsWith>::Properties
: public ModifierActionProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::StartsWith>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::StartsWith>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::StartsWith>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::StartsWith>
{
public:
    void toAction(ModifierAction& action)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_ACTION_STARTSWITH_HPP
