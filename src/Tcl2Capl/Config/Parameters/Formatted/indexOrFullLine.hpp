#ifndef INDEXORFULLLINE_HPP
#define INDEXORFULLLINE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::IndexItem>::Properties
: public FormatParametersProductDefinition::Definition
{

protected:
   static constexpr int INVALID_INDEX = INT_MIN;
   int index_ = INT_MIN;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::IndexItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::IndexItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::IndexItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::IndexItem>
{
public:
    void setIndex(int index){index_ = index;}
    int index(){return (index_ == INVALID_INDEX)? 0 : index_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = Tcl::Interpreter::Command::Definition::Format;
        Format::addFormatRule(parameters, Format::Rule::INDEX_OR_FULL_LINE, QString::number(index()));
    }
    void toXmlContent(QXmlStreamWriter& xmlWriter) override{
            xmlWriter.writeEmptyElement("formatRule"); // String param?
            xmlWriter.writeAttribute("type", QString::number(type())); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", QString::number(index()));
    }
    //inline RawFormatType rawFormatType()const override final{return RawFormatType::IndexItem;}

};


#endif // INDEXORFULLLINE_HPP
