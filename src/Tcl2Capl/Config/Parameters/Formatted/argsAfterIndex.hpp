#ifndef ARGSAFTERINDEX_HPP
#define ARGSAFTERINDEX_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersFactory::ImplementationData<FormatParametersType::ARGS_AFTER_INDEX>::Properties{
protected:
    //FormatParametersFactory::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersFactory::ImplementationData<FormatParametersType::ARGS_AFTER_INDEX>::Methods
: protected FormatParametersFactory::ImplementationData<FormatParametersType::ARGS_AFTER_INDEX>::Properties
{

};

template<>
template<>
class FormatParametersFactory::InterfaceData<FormatParametersType::ARGS_AFTER_INDEX>::Methods
: public FormatParametersFactory::Implementation<FormatParametersType::ARGS_AFTER_INDEX>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::ARGS_AFTER_INDEX;}

};


#endif // ARGSAFTERINDEX_HPP
