#ifndef TEXT_HPP
#define TEXT_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
struct FormatParametersProducts::ImplementationData<FormatParametersType::TextItem>::Properties
: public FormatParametersProductDefinition::Definition
{
protected:
    QString text_;

};

template<>
template<>
class FormatParametersProducts::ImplementationData<FormatParametersType::TextItem>::Methods
: public FormatParametersProducts::ImplementationData<FormatParametersType::TextItem>::Properties
{

};

template<>
template<>
class FormatParametersProducts::InterfaceData<FormatParametersType::TextItem>::Methods
: public FormatParametersProducts::Implementation<FormatParametersType::TextItem>
{
public:
    void setText(QString str){text_ = str;}
    const QString& text()const{return text_;}
    void toActionParameters(QStringList& parameters)override{
        if(not text().isEmpty()){
            parameters << text();
        }
    }
    void toXmlContent(QXmlStreamWriter& xmlWriter) override{
        xmlWriter.writeStartElement("formatRule"); // String param?
        xmlWriter.writeAttribute("type", FormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
        xmlWriter.writeCharacters(text());
        xmlWriter.writeEndElement();
    }
   // inline RawFormatType rawFormatType()const override final{return RawFormatType::ArgumentsFromItem;}

};

#endif // TEXT_HPP
