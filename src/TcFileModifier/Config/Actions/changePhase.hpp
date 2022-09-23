#ifndef FILE_MODIFIER_ACTION_CHANGEPHASE_HPP
#define FILE_MODIFIER_ACTION_CHANGEPHASE_HPP

#include"External/Factory/products.hpp"
#include"TcFileModifier/Config/Actions/definition.hpp"

template<>
template<>
struct ModifierActionProducts::ImplementationData<ModifierActionTypes::ChangePhase>::Properties
: public ModifierActionProductDefinition::Definition
{
protected:

    QString phaseName_;
    bool checkRulesAfterChange_;

};

template<>
template<>
class ModifierActionProducts::ImplementationData<ModifierActionTypes::ChangePhase>::Methods
: public ModifierActionProducts::ImplementationData<ModifierActionTypes::ChangePhase>::Properties
{
};

template<>
template<>
class ModifierActionProducts::InterfaceData<ModifierActionTypes::ChangePhase>::Methods
: public ModifierActionProducts::Implementation<ModifierActionTypes::ChangePhase>
{
public:
    QString phaseName(){return phaseName_;}
    void setPhaseName(QString phaseName){phaseName_ = phaseName;}
    bool checkRule(){return checkRulesAfterChange_;}
    void setCheckRule(bool checkRule){checkRulesAfterChange_ = checkRule;}
    void toAction(ModifierAction& action)override{
        using Parameters = QStringList;
        action = ModifierAction{static_cast<RawModifierActionType>(type()),
                        Parameters({phaseName_, QString::number(checkRulesAfterChange_)})
                                           };
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // FILE_MODIFIER_ACTION_CHANGEPHASE_HPP
