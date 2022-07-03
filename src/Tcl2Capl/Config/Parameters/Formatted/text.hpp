#ifndef TEXT_HPP
#define TEXT_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::TextItem>::Properties
: public FormatParametersProductDefinition::Definition
{
protected:
    //FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::TextItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::TextItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::TextItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::TextItem>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::ArgumentsFromItem;}

};

#endif // TEXT_HPP
