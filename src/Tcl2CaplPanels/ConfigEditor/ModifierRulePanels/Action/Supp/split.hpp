#ifndef MODIFIER_ACTION_PANEL_SPLIT_HPP
#define MODIFIER_ACTION_PANEL_SPLIT_HPP

#include"Tcl2CaplPanels/ConfigEditor/ModifierRulePanels/FormattedString/formattedString.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/General/multilineEditorDialog.hpp"

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ModifierActionsList{

    class SplitActionView
            : public ActionDataView<ModifierActionsFactory::ListOfBases>
    {
        static constexpr ActionType actionType = ModifierActionTypes::Split;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using Action = ModifierActionsFactory::Product<actionType>;

        SplitActionView(QWidget*);
        SplitActionView(QWidget*, ActionPtr);
        ~SplitActionView()override{
        }
    public:
        static ActionDataView* create(QWidget*, ActionRef);
        using ActionView = ActionView;
    protected:
        using Supps = ExecutablesFactory::ListOfBases;
        using SuppsList = List<Supps>;
        using ListItem = SuppsList::ListItem;

        // GUI Elements
        // List of Indexes definition

    public:
    protected:
        // GUI Layout
        QSplitter splitter;
        ModifierFormattedString::Panel formattedStringList;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}
        void readAction(ActionBase&) override;

    };


}


#endif // MODIFIER_ACTION_PANEL_SPLIT_HPP
