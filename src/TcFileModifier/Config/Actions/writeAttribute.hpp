#ifndef FILE_MODIFIER_ACTION_WRITEATTRIBUTE_HPP
#define FILE_MODIFIER_ACTION_WRITEATTRIBUTE_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"
#include"TcFileModifier/Config/Parameters/Formatted/FCT_products.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::WriteAttribute>::Properties
: public ModifierActionProductDefinition::Definition
{
    using FormattedString = ModifierFormatParametersFactory::ListOfBases;
protected:
    QString name_;
    FormattedString inputFormattedString_;

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::WriteAttribute>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::WriteAttribute>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::WriteAttribute>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::WriteAttribute>
{
public:
    QString name(){return name_;}
    void setName(QString name){name_ = name;}
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
                        Parameters({name_}) <<
                        QString::number(formattedStringParameters.size())
                        << formattedStringParameters
                                           };
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_ACTION_WRITEATTRIBUTE_HPP
