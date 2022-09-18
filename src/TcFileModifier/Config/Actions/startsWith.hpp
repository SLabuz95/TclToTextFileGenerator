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
                        Parameters({QString::number(stringsToCompare_.size())})
                            <<  stringsToCompare_
                            //<< QStringList{QString::number(formattedStringParameters.size())}
                            //<< formattedStringParameters
                                           };
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_ACTION_STARTSWITH_HPP
