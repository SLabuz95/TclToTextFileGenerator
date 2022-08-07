#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_TEXTITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_TEXTITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TextItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TextItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TextItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::TextItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::TextItem>
{
public:
    void toActionParameters(QStringList& parameters)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_TEXTITEM_HPP
