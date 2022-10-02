#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_TARGETITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_TARGETITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TargetItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{

public:
    using Format = ModifierFormatParameters::Format;
    using Target = Format::Target;
protected:
    Target target_ = Target();

};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TargetItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TargetItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::TargetItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::TargetItem>
{
public:
    void setTarget(Target target){target_ = target;}
    Target target()const{return target_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = ModifierFormatParameters::Format;
        Format::addFormatRule(parameters, Format::Rule::TARGET, Format::cast_target_str(target()));
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeEmptyElement("modifierFormatRule"); // String param?
        xmlWriter.writeAttribute("type", ModifierFormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
        xmlWriter.writeAttribute("value", Format::toStr_target(target()));

    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_TARGETITEM_HPP
