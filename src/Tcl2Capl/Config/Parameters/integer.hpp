#ifndef INTEGER_HPP
#define INTEGER_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/definition.hpp"

template<>
template<>
struct ParametersProducts::ImplementationData<ParametersType::Integer>::Properties
: public ParametersProductDefinition::Definition
{
public:
    inline static constexpr int NO_VALUE = INT32_MIN;
protected:
   // qint32 value = NO_VALUE;
};

template<>
template<>
class ParametersProducts::ImplementationData<ParametersType::Integer>::Methods
: public ParametersProducts::ImplementationData<ParametersType::Integer>::Properties
{
protected:

};

template<>
template<>
class ParametersProducts::InterfaceData<ParametersType::Integer>::Methods
: public ParametersProducts::Implementation<ParametersType::Integer>
{
public:
    //void toXmlContent(QXmlStreamWriter& xmlWriter) override;

};

#endif // INTEGER_HPP
