#ifndef FILE_MODIFIER_ACTION_TCLINTERPRET_HPP
#define FILE_MODIFIER_ACTION_TCLINTERPRET_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/FCT_products.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::Interpret>::Properties
: public ModifierActionProductDefinition::Definition
{
public:
    using FormattedString = ModifierFormatParametersFactory::ListOfBases;
protected:
     FormattedString inputFormattedString_;

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::Interpret>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::Interpret>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::Interpret>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::Interpret>
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
                            Parameters({QString::number(formattedStringParameters.size())})
                            << formattedStringParameters
                                           };
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_ACTION_TCLINTERPRET_HPP
