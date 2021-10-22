#ifndef ARGSAFTERINDEX_HPP
#define ARGSAFTERINDEX_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::ARGS_AFTER_INDEX>::Properties
: protected FormatParametersProductDefinition::Definition
{
protected:
    //FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::ARGS_AFTER_INDEX>::Methods
: protected FormatParametersProducts::ImplementationData<FormatParametersType::ARGS_AFTER_INDEX>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::ARGS_AFTER_INDEX>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::ARGS_AFTER_INDEX>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::ARGS_AFTER_INDEX;}

};


#endif // ARGSAFTERINDEX_HPP
