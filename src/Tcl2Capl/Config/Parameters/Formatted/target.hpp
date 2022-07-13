#ifndef TARGET_HPP
#define TARGET_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Properties
: public FormatParametersProductDefinition::Definition
{
    using UserProcedure = Tcl::Interpreter::Command::Definition;
public:
    using Format = UserProcedure::Format;
    using Target = Format::Target;
protected:
    Target target_ = Target();

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::FormatItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::FormatItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::FormatItem>
{
public:
    void setTarget(Target target){target_ = target;}
    Target target()const{return target_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = Tcl::Interpreter::Command::Definition::Format;
        Format::addFormatRule(parameters, Format::Rule::TARGET, Format::cast_target_str(target()));
    }
   // void toXmlContent(QXmlStreamWriter& xmlWriter) override;
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::NameItem;}

};
/*
using FormatParam = ParametersFactory::Product<ParametersType::FormattedString>;
using IntegerParam = ParametersFactory::Product<ParametersType::Integer>;
using ListParam = ParametersFactory::Product<ParametersType::List>;
*/

#endif // TARGET_HPP
