#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::AttributeItem>::Properties
: public FormatParametersProductDefinition::Definition
{
protected:
    QString name_;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::AttributeItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::AttributeItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::AttributeItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::AttributeItem>
{
public:
    void setName(QString name){name_ = name;}
    const QString& name(){return name_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = Tcl::Interpreter::Command::Definition::Format;
        Format::addFormatRule(parameters, Format::Rule::ATTRIBUTE, name_);
    }
    void toXmlContent(QXmlStreamWriter& xmlWriter) override{

    }
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::AttributeItem;}

};

#endif // ARGSAFTERINDEX_HPP

