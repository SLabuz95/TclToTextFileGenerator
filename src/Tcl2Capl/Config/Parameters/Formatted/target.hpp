#ifndef TARGET_HPP
#define TARGET_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Properties
: public FormatParametersProductDefinition::Definition
{
protected:
   // FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::FormatItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::FormatItem>
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

#endif // TARGET_HPP
