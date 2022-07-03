#ifndef ARGSAFTERINDEX_HPP
#define ARGSAFTERINDEX_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::ArgumentsFromItem>::Properties
: public FormatParametersProductDefinition::Definition
{
protected:
    //FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::ArgumentsFromItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::ArgumentsFromItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::ArgumentsFromItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::ArgumentsFromItem>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::ArgumentsFromItem;}

};


#endif // ARGSAFTERINDEX_HPP
