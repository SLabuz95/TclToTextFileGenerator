#ifndef FILE_MODIFIER_ACTION_SPLIT_HPP
#define FILE_MODIFIER_ACTION_SPLIT_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/FCT_products.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::Split>::Properties
: public ModifierActionProductDefinition::Definition
{
public:
    using FormattedString = ModifierFormatParametersFactory::ListOfBases;
protected:
     FormattedString inputFormattedString_;


};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::Split>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::Split>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::Split>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::Split>
{
public:
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    void toAction(ModifierAction& action)override{
        using Parameters = QStringList;
        Parameters formattedStringParameters;
        // Temp for no parameter definitions for ModifierRules
        //inputFormattedString().toActionParameters(formattedStringParameters);
        FormattedString::Iterator parameter = inputFormattedString().begin();
        for( ; parameter < inputFormattedString().end(); parameter++)
            (*parameter)->toActionParameters(formattedStringParameters);

        action = ModifierAction{static_cast<RawModifierActionType>(type()),
                            Parameters()
                            << formattedStringParameters
                                           };
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("modifierAction");
        xmlWriter.writeAttribute("type", ModifierActions::TypeInfo::toStr(type()));
        // inputFormattedString_
        xmlWriter.writeStartElement("param"); // List?
        for(decltype(inputFormattedString_)::Iterator formatParam = inputFormattedString_.begin();
            formatParam < inputFormattedString_.end(); formatParam++)
        {
            (*formatParam)->toXmlContent(xmlWriter);
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
    }
};


#endif // FILE_MODIFIER_ACTION_SPLIT_HPP
