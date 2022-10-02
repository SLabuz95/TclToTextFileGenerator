#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_FULLLINEITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_FULLLINEITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::FullLineItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
public:

protected:

};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::FullLineItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::FullLineItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::FullLineItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::FullLineItem>
{
public:
    void toActionParameters(QStringList& parameters)override{
        using Format = ModifierFormatParameters::Format;
        Format::addFormatRule(parameters, Format::Rule::INDEX_OR_FULL_LINE, QString());
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeEmptyElement("modifierFormatRule"); // String param?
        xmlWriter.writeAttribute("type", ModifierFormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_FULLLINEITEM_HPP
