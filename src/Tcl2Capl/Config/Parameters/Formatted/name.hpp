#ifndef NAMEITEM_HPP
#define NAMEITEM_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::NameItem>::Properties
: public FormatParametersProductDefinition::Definition
{
public:
    //FormatParametersProducts::FactoryCommonInterface* data = nullptr;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::NameItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::NameItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::NameItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::NameItem>
{
public:
    void toActionParameters(QStringList& parameters)override{
        using Format = Tcl::Interpreter::Command::Definition::Format;
        Format::addFormatRule(parameters, Format::Rule::INDEX_OR_FULL_LINE, QString());
    }
    void toXmlContent(QXmlStreamWriter& xmlWriter) override{
        xmlWriter.writeEmptyElement("formatRule"); // String param?
        xmlWriter.writeAttribute("type", QString::number(type())); // For compatibility with future implementation
    }
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::FormatItem;}

};


#endif // NAMEITEM_HPP
