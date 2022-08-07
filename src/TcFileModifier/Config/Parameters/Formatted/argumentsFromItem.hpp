#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_ARGUMENTSFROMITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_ARGUMENTSFROMITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::ArgumentsFromItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::ArgumentsFromItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::ArgumentsFromItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::ArgumentsFromItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::ArgumentsFromItem>
{
public:
    void toActionParameters(QStringList& parameters)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_ARGUMENTSFROMITEM_HPP
