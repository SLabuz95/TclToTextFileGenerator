#ifndef FORMATTEDSTRING_HPP
#define FORMATTEDSTRING_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"
#include"Formatted/FCT_products.hpp"

template<>
template<>
struct ParametersProducts::ImplementationData<ParametersType::FormattedString>::Properties
: public ParametersProductDefinition::Definition
{
public:
    using Parameters = FormatParametersFactory::ListOfBases;
protected:
   Parameters parameters_;

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
    Parameters& parameters(){return parameters_;}    
    void toActionParameters(QStringList& actionParameters){
        Parameters::Iterator parameter = parameters().begin();
        for( ; parameter < parameters().end(); parameter++)
            (*parameter)->toActionParameters(actionParameters);
    }
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // FORMATTEDSTRING_HPP
