#ifndef INDEXORFULLLINE_HPP
#define INDEXORFULLLINE_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersFactory::ImplementationData<FormatParametersType::INDEX_OR_FULL_LINE>::Properties{

protected:
   // FormatParametersFactory::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersFactory::ImplementationData<FormatParametersType::INDEX_OR_FULL_LINE>::Methods
: protected FormatParametersFactory::ImplementationData<FormatParametersType::INDEX_OR_FULL_LINE>::Properties
{

};

template<>
template<>
class FormatParametersFactory::InterfaceData<FormatParametersType::INDEX_OR_FULL_LINE>::Methods
: public FormatParametersFactory::Implementation<FormatParametersType::INDEX_OR_FULL_LINE>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::INDEX_OR_FULL_LINE;}

};


#endif // INDEXORFULLLINE_HPP
