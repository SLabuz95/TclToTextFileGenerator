#ifndef FORMATTEDSTRING_HPP
#define FORMATTEDSTRING_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersProducts::ImplementationData<ParametersType::FormattedString>::Properties
: public ParametersProductDefinition::Definition
{
protected:
   // FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class ParametersProducts::ImplementationData<ParametersType::FormattedString>::Methods
: public ParametersProducts::ImplementationData<ParametersType::FormattedString>::Properties
{
protected:
};

template<>
template<>
class ParametersProducts::InterfaceData<ParametersType::FormattedString>::Methods
: public ParametersProducts::Implementation<ParametersType::FormattedString>
{
public:
    void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // FORMATTEDSTRING_HPP
