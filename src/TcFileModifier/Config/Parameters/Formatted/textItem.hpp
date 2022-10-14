#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_TEXTITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_TEXTITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TextItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
protected:
    QString text_;

};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TextItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::TextItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::TextItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::TextItem>
{
public:
    void setText(QString str){text_ = str;}
    const QString& text()const{return text_;}
    void toActionParameters(QStringList& parameters)override{
        if(not text().isEmpty()){
            parameters << text();
        }
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("modifierFormatRule"); // String param?
        xmlWriter.writeAttribute("type", ModifierFormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
        xmlWriter.writeCharacters(text());
        xmlWriter.writeEndElement();
    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_TEXTITEM_HPP
