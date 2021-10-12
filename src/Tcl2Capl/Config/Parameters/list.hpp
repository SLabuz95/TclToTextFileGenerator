#ifndef LIST_HPP
#define LIST_HPP


#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersProducts::ImplementationData<ParametersType::List>::Properties
: protected ParametersProductDefinition::Definition
{
protected:
   // ListOfBases params;

};

template<>
template<>
class ParametersProducts::ImplementationData<ParametersType::List>::Methods
: protected ParametersProducts::ImplementationData<ParametersType::List>::Properties
{
protected:

};

template<>
template<>
class ParametersProducts::InterfaceData<ParametersType::List>::Methods
: public ParametersProducts::Implementation<ParametersType::List>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // LIST_HPP
