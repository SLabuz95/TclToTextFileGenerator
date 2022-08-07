#ifndef FILE_MODIFIER_ACTION_WRITE_HPP
#define FILE_MODIFIER_ACTION_WRITE_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::Write>::Properties
: public ModifierActionProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::Write>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::Write>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::Write>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::Write>
{
public:
    void toAction(ModifierAction& action)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};

#endif // FILE_MODIFIER_ACTION_WRITE_HPP
