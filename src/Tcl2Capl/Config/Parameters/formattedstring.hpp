#ifndef FORMATTEDSTRING_HPP
#define FORMATTEDSTRING_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersFactory::ImplementationData<ParametersType::FormattedString>::Properties{
protected:
   // FormatParametersFactory::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class ParametersFactory::ImplementationData<ParametersType::FormattedString>::Methods
: protected ParametersFactory::ImplementationData<ParametersType::FormattedString>::Properties
{
protected:
};

template<>
template<>
class ParametersFactory::InterfaceData<ParametersType::FormattedString>::Methods
: public ParametersFactory::Implementation<ParametersType::FormattedString>
{
public:
    void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // FORMATTEDSTRING_HPP
