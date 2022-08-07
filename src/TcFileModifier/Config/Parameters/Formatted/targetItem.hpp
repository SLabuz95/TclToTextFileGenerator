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

protected:

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
    void toActionParameters(QStringList& parameters)override{

    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_TARGETITEM_HPP
