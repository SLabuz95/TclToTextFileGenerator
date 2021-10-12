#ifndef SEPARATOR_HPP
#define SEPARATOR_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::SEPARATOR>::Properties
: protected FormatParametersProductDefinition::Definition
{
protected:
    //FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::SEPARATOR>::Methods
: protected FormatParametersProducts::ImplementationData<FormatParametersType::SEPARATOR>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::SEPARATOR>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::SEPARATOR>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::SEPARATOR;}

};


#endif // SEPARATOR_HPP
