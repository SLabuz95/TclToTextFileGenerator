#ifndef ARGSAFTERINDEX_HPP
#define ARGSAFTERINDEX_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::ArgumentsFromItem>::Properties
: public FormatParametersProductDefinition::Definition
{
protected:
    static constexpr int INVALID_INDEX = INT_MIN;
    int index_ = INT_MIN;
    QString separator_;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::ArgumentsFromItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::ArgumentsFromItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::ArgumentsFromItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::ArgumentsFromItem>
{
public:
    void setIndex(int index){index_ = index;}
    void setSeparator(QString str){separator_ = str;}
    int index(){return (index_ == INVALID_INDEX)? 0 : index_;}
    const QString& separator(){return separator_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = Tcl::Interpreter::Command::Definition::Format;
        if(not separator().isEmpty())
            Format::addFormatRule(parameters, Format::Rule::SEPARATOR, separator_);
        Format::addFormatRule(parameters, Format::Rule::ARGS_AFTER_INDEX, QString::number(index()));
    }
    void toXmlContent(QXmlStreamWriter& xmlWriter) override{
        if(separator().length() == 0){
            xmlWriter.writeEmptyElement("formatRule"); // String param?
            xmlWriter.writeAttribute("type", FormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", QString::number(index()));
        }else{
            xmlWriter.writeStartElement("formatRule"); // String param?
            xmlWriter.writeAttribute("type", FormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", QString::number(index()));
            xmlWriter.writeCharacters(separator());
            xmlWriter.writeEndElement(); // List? End
        }
    }
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::ArgumentsFromItem;}

};


#endif // ARGSAFTERINDEX_HPP
