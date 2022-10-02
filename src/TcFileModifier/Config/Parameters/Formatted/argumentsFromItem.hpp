#ifndef FILE_MODIFIER_FORMATTED_PARAMETER_ARGUMENTSFROMITEM_HPP
#define FILE_MODIFIER_FORMATTED_PARAMETER_ARGUMENTSFROMITEM_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/definition.hpp"

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::ArgumentsFromItem>::Properties
: public ModifierFormatParametersProductDefinition::Definition
{
protected:
    static constexpr int INVALID_INDEX = INT_MIN;
    int index_ = INT_MIN;
    QString separator_;
};

template<>
template<>
class ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::ArgumentsFromItem>::Methods
: public ModifierFormatParametersProducts::ImplementationData<ModifierFormatParametersType::ArgumentsFromItem>::Properties
{
};

template<>
template<>
class ModifierFormatParametersProducts::InterfaceData<ModifierFormatParametersType::ArgumentsFromItem>::Methods
: public ModifierFormatParametersProducts::Implementation<ModifierFormatParametersType::ArgumentsFromItem>
{
public:
    void setIndex(int index){index_ = index;}
    void setSeparator(QString str){separator_ = str;}
    int index(){return (index_ == INVALID_INDEX)? 0 : index_;}
    const QString& separator(){return separator_;}
    void toActionParameters(QStringList& parameters)override{
        using Format = ModifierFormatParameters::Format;
        if(not separator().isEmpty())
            Format::addFormatRule(parameters, Format::Rule::SEPARATOR, separator_);
        Format::addFormatRule(parameters, Format::Rule::ARGS_AFTER_INDEX, QString::number(index()));
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        if(separator().length() == 0){
            xmlWriter.writeEmptyElement("modifierFormatRule"); // String param?
            xmlWriter.writeAttribute("type", ModifierFormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", QString::number(index()));
        }else{
            xmlWriter.writeStartElement("modifierFormatRule"); // String param?
            xmlWriter.writeAttribute("type", ModifierFormatParameters::TypeInfo::toStr(type())); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", QString::number(index()));
            xmlWriter.writeCharacters(separator());
            xmlWriter.writeEndElement(); // List? End
        }
    }
};
#endif // FILE_MODIFIER_FORMATTED_PARAMETER_ARGUMENTSFROMITEM_HPP
