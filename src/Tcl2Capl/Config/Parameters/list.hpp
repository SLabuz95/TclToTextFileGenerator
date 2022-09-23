#ifndef LIST_HPP
#define LIST_HPP


#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersProducts::ImplementationData<ParametersType::List>::Properties
: public ParametersProductDefinition::Definition
{
protected:
    using ParametersList = ListOfBases;
    ListOfBases parameters_;

};

template<>
template<>
class ParametersProducts::ImplementationData<ParametersType::List>::Methods
: public ParametersProducts::ImplementationData<ParametersType::List>::Properties
{
protected:

};

template<>
template<>
class ParametersProducts::InterfaceData<ParametersType::List>::Methods
: public ParametersProducts::Implementation<ParametersType::List>
{
public:
    ListOfBases& parameters(){return parameters_;}
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // LIST_HPP
