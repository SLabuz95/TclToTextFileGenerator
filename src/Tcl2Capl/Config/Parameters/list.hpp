#ifndef LIST_HPP
#define LIST_HPP


#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersFactory::ImplementationData<ParametersType::List>::Properties{
protected:
   // ListOfBases params;

};

template<>
template<>
class ParametersFactory::ImplementationData<ParametersType::List>::Methods
: protected ParametersFactory::ImplementationData<ParametersType::List>::Properties
{
protected:

};

template<>
template<>
class ParametersFactory::InterfaceData<ParametersType::List>::Methods
: public ParametersFactory::Implementation<ParametersType::List>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // LIST_HPP
