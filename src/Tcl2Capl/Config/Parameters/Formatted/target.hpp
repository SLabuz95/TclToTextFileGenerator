#ifndef TARGET_HPP
#define TARGET_HPP


#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersFactory::ImplementationData<FormatParametersType::TARGET>::Properties{
protected:
   // FormatParametersFactory::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersFactory::ImplementationData<FormatParametersType::TARGET>::Methods
: protected FormatParametersFactory::ImplementationData<FormatParametersType::TARGET>::Properties
{

};

template<>
template<>
class FormatParametersFactory::InterfaceData<FormatParametersType::TARGET>::Methods
: public FormatParametersFactory::Implementation<FormatParametersType::TARGET>
{
public:
   // void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::TARGET;}

};
/*
using FormatParam = ParametersFactory::Product<ParametersType::FormattedString>;
using IntegerParam = ParametersFactory::Product<ParametersType::Integer>;
using ListParam = ParametersFactory::Product<ParametersType::List>;
*/

#endif // TARGET_HPP
