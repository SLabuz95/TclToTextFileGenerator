#ifndef FILE_MODIFIER_ACTION_TCLINTERPRET_HPP
#define FILE_MODIFIER_ACTION_TCLINTERPRET_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::Interpret>::Properties
: public ModifierActionProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::Interpret>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::Interpret>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::Interpret>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::Interpret>
{
public:
    void toAction(ModifierAction& action)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_ACTION_TCLINTERPRET_HPP
