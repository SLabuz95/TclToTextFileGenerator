#ifndef INDEXORFULLLINE_HPP
#define INDEXORFULLLINE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::INDEX_OR_FULL_LINE>::Properties
: protected FormatParametersProductDefinition::Definition
{

protected:
   // FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::INDEX_OR_FULL_LINE>::Methods
: protected FormatParametersProducts::ImplementationData<FormatParametersType::INDEX_OR_FULL_LINE>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::INDEX_OR_FULL_LINE>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::INDEX_OR_FULL_LINE>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::INDEX_OR_FULL_LINE;}

};


#endif // INDEXORFULLLINE_HPP
