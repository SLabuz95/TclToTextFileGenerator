#ifndef FILE_MODIFIER_ACTION_COMPARE_HPP
#define FILE_MODIFIER_ACTION_COMPARE_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::Compare>::Properties
: public ModifierActionProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::Compare>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::Compare>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::Compare>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::Compare>
{
public:
    void toAction(ModifierAction& action)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};

#endif // FILE_MODIFIER_ACTION_COMPARE_HPP
