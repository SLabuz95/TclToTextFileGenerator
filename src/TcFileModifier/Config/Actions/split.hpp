#ifndef FILE_MODIFIER_ACTION_SPLIT_HPP
#define FILE_MODIFIER_ACTION_SPLIT_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::Split>::Properties
: public ModifierActionProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::Split>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::Split>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::Split>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::Split>
{
public:
    void toAction(ModifierAction& action)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};


#endif // FILE_MODIFIER_ACTION_SPLIT_HPP
