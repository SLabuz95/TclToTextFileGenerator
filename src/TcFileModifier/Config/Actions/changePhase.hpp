#ifndef FILE_MODIFIER_ACTION_CHANGEPHASE_HPP
#define FILE_MODIFIER_ACTION_CHANGEPHASE_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::ChangePhase>::Properties
: public ModifierActionProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::ChangePhase>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::ChangePhase>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::ChangePhase>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::ChangePhase>
{
public:
    void toAction(ModifierAction& action)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_ACTION_CHANGEPHASE_HPP
