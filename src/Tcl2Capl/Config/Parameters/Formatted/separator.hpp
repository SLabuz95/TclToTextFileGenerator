#ifndef SEPARATOR_HPP
#define SEPARATOR_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersFactory::ImplementationData<FormatParametersType::SEPARATOR>::Properties{
protected:
    //FormatParametersFactory::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersFactory::ImplementationData<FormatParametersType::SEPARATOR>::Methods
: protected FormatParametersFactory::ImplementationData<FormatParametersType::SEPARATOR>::Properties
{

};

template<>
template<>
class FormatParametersFactory::InterfaceData<FormatParametersType::SEPARATOR>::Methods
: public FormatParametersFactory::Implementation<FormatParametersType::SEPARATOR>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::SEPARATOR;}

};


#endif // SEPARATOR_HPP
