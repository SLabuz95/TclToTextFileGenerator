#ifndef INDEXORFULLLINE_HPP
#define INDEXORFULLLINE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::IndexItem>::Properties
: public FormatParametersProductDefinition::Definition
{

protected:
   // FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::IndexItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::IndexItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::IndexItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::IndexItem>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::IndexItem;}

};


#endif // INDEXORFULLLINE_HPP
