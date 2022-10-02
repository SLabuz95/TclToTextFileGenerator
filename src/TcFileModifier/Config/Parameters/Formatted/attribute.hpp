#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_ATTRIBUTE_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_ATTRIBUTE_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::AttributeItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
protected:
    QString name_;
};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::AttributeItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::AttributeItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::AttributeItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::AttributeItem>
{
public:
    void setName(QString name){name_ = name;}
    const QString& name(){return name_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = ModifierFormatParameters::Format;
        Format::addFormatRule(parameters, Format::Rule::ATTRIBUTE, name_);
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeEmptyElement("modifierFormatRule"); // String param?
        xmlWriter.writeAttribute("type", ModifierFormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
        xmlWriter.writeAttribute("value", name());

    }
};

#endif // FILE_MODIFIER_FORMATTED_PARAMETER_ATTRIBUTE_HPP
