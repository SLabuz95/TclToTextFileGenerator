#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_INDEXITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_INDEXITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::IndexItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
protected:
   static constexpr int INVALID_INDEX = INT_MIN;
   int index_ = INT_MIN;


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
    void setIndex(int index){index_ = index;}
    int index(){return (index_ == INVALID_INDEX)? 0 : index_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = ModifierFormatParameters::Format;
        Format::addFormatRule(parameters, Format::Rule::INDEX_OR_FULL_LINE, QString::number(index()));
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_INDEXITEM_HPP
