#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_INDEXITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_INDEXITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::IndexItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::IndexItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::IndexItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::IndexItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::IndexItem>
{
public:
    void toActionParameters(QStringList& parameters)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_INDEXITEM_HPP
