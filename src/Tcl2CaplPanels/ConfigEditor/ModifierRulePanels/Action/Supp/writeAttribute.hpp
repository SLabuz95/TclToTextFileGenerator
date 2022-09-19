#ifndef MODIFIER_ACTION_PANEL_WRITEATTRIBUTE_HPP
#define MODIFIER_ACTION_PANEL_WRITEATTRIBUTE_HPP

#include"Tcl2CaplPanels/ConfigEditor/ModifierRulePanels/FormattedString/formattedString.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ModifierActionsList{

    class WriteAttributeActionView
            : public ActionDataView<ModifierActionsFactory::ListOfBases>
    {
        static constexpr ActionType actionType = ModifierActionTypes::WriteAttribute;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using Action = ModifierActionsFactory::Product<actionType>;

        WriteAttributeActionView(QWidget*);
        WriteAttributeActionView(QWidget*, ActionPtr);
        ~WriteAttributeActionView()override{
        }
    public:
        static ActionDataView* create(QWidget*, ActionRef);
        using ActionView = ActionView;
    protected:
        using Supps = ModifierActionsFactory::ListOfBases;
        using SuppsList = List<Supps>;
        using ListItem = SuppsList::ListItem;

        // GUI Elements
        // List of Indexes definition

    public:
    protected:
        // GUI Layout
        QLineEdit attributeNameEditor;
        QSplitter splitter;
        ModifierFormattedString::Panel formattedStringList;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}
        void readAction(ActionBase&) override;

    };


}

#endif // MODIFIER_ACTION_PANEL_WRITEATTRIBUTE_HPP
