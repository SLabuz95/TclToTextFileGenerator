#ifndef STRING_HPP
#define STRING_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersProducts::ImplementationData<ParametersType::String>::Properties
: protected ParametersProductDefinition::Definition
{
protected:
    QString content;

};

template<>
template<>
class ParametersProducts::ImplementationData<ParametersType::String>::Methods
: protected ParametersProducts::ImplementationData<ParametersType::String>::Properties
{

};

template<>
template<>
class ParametersProducts::InterfaceData<ParametersType::String>::Methods
: public ParametersProducts::Implementation<ParametersType::String>
{
public:
   // void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // STRING_HPP
