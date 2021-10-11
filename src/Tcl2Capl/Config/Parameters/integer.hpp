#ifndef INTEGER_HPP
#define INTEGER_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersFactory::ImplementationData<ParametersType::Integer>::Properties{
public:
    inline static constexpr int NO_VALUE = INT32_MIN;
protected:
   // qint32 value = NO_VALUE;
};

template<>
template<>
class ParametersFactory::ImplementationData<ParametersType::Integer>::Methods
: protected ParametersFactory::ImplementationData<ParametersType::Integer>::Properties
{
protected:

};

template<>
template<>
class ParametersFactory::InterfaceData<ParametersType::Integer>::Methods
: public ParametersFactory::Implementation<ParametersType::Integer>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // INTEGER_HPP
