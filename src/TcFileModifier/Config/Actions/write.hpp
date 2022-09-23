#ifndef FILE_MODIFIER_ACTION_WRITE_HPP
#define FILE_MODIFIER_ACTION_WRITE_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/FCT_products.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::Write>::Properties
: public ModifierActionProductDefinition::Definition
{
public:
    using FormattedString = ModifierFormatParametersFactory::ListOfBases;
protected:
     FormattedString inputFormattedString_;

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::Write>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::Write>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::Write>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::Write>
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

    }
};

#endif // FILE_MODIFIER_ACTION_WRITE_HPP
