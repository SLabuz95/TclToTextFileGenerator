#ifndef FormatItem_HPP
#define FormatItem_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Properties
: protected FormatParametersProductDefinition::Definition
{
protected:
    //FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Methods
: protected FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::FormatItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::FormatItem>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::FormatItem;}

};


#endif // FormatItem_HPP
