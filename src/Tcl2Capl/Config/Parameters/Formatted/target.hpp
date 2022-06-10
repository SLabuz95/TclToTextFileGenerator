#ifndef NameItem_HPP
#define NameItem_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::NameItem>::Properties
: protected FormatParametersProductDefinition::Definition
{
protected:
   // FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::NameItem>::Methods
: protected FormatParametersProducts::ImplementationData<FormatParametersType::NameItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::NameItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::NameItem>
{
public:
   // void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::NameItem;}

};
/*
using FormatParam = ParametersFactory::Product<ParametersType::FormattedString>;
using IntegerParam = ParametersFactory::Product<ParametersType::Integer>;
using ListParam = ParametersFactory::Product<ParametersType::List>;
*/

#endif // NameItem_HPP
