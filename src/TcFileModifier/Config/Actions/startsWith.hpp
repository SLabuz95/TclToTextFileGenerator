#ifndef FILE_MODIFIER_ACTION_STARTSWITH_HPP
#define FILE_MODIFIER_ACTION_STARTSWITH_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/FCT_products.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::StartsWith>::Properties
: public ModifierActionProductDefinition::Definition
{
public:
    using FormattedString = ModifierFormatParametersFactory::ListOfBases;
protected:
     //FormattedString inputFormattedString_;
     QStringList stringsToCompare_;


};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::StartsWith>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::StartsWith>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::StartsWith>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::StartsWith>
{
public:
    QStringList& stringsToCompare(){return stringsToCompare_;}
    //FormattedString& inputFormattedString(){return inputFormattedString_;}
    void toAction(ModifierAction& action)override{
        using Parameters = QStringList;
        //Parameters formattedStringParameters;
        // Temp for no parameter definitions for ModifierRules
        //inputFormattedString().toActionParameters(formattedStringParameters);
        //FormattedString::Iterator parameter = inputFormattedString().begin();
        //for( ; parameter < inputFormattedString().end(); parameter++)
       //     (*parameter)->toActionParameters(formattedStringParameters);

        action = ModifierAction{static_cast<RawModifierActionType>(type()),
                        Parameters({stringsToCompare_})

                            //<< QStringList{QString::number(formattedStringParameters.size())}
                            //<< formattedStringParameters
                                           };
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("modifierAction");
        xmlWriter.writeAttribute("type", ModifierActions::TypeInfo::toStr(type()));
        // stringsToCompare_
        xmlWriter.writeStartElement("param"); // List?
        xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
        for(QStringList::Iterator string = stringsToCompare_.begin(); string < stringsToCompare_.end(); string++){
            xmlWriter.writeStartElement("param"); // String param?
            xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
            xmlWriter.writeCharacters(*string);
            xmlWriter.writeEndElement(); // List? End
        }
        xmlWriter.writeEndElement(); // List? End

        xmlWriter.writeEndElement();
    }
};
#endif // FILE_MODIFIER_ACTION_STARTSWITH_HPP
