#ifndef STRING_HPP
#define STRING_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersFactory::ImplementationData<ParametersType::String>::Properties{
protected:
    QString content;

};

template<>
template<>
class ParametersFactory::ImplementationData<ParametersType::String>::Methods
: protected ParametersFactory::ImplementationData<ParametersType::String>::Properties
{

};

template<>
template<>
class ParametersFactory::InterfaceData<ParametersType::String>::Methods
: public ParametersFactory::Implementation<ParametersType::String>
{
public:
   // void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // STRING_HPP
