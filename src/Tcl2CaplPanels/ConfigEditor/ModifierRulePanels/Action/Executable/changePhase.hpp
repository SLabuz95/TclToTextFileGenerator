#ifndef MODIFIER_ACTION_PANEL_CHANGEPHASE_HPP
#define MODIFIER_ACTION_PANEL_CHANGEPHASE_HPP

#include"Tcl2CaplPanels/ConfigEditor/ModifierRulePanels/FormattedString/formattedString.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/General/multilineEditorDialog.hpp"

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ModifierActionsList{

    class ChangePhaseActionView
            : public ActionDataView<ModifierActionsFactory::ListOfBases>
    {

        static constexpr ActionType actionType = ModifierActionTypes::ChangePhase;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<QListWidget>;
        using Action = ModifierActionsFactory::Product<actionType>;

        ChangePhaseActionView(QWidget* );
        ChangePhaseActionView(QWidget* , ActionPtr);
    public:
        using ParentContextMenu = ContextMenuInterface::Interface;
        static ActionDataView* create(QWidget* , ActionRef);
        using ActionView = ActionView;
    protected:
        using Executables = ModifierActionsFactory::ListOfBases;
        using ExecutablesList = List<Executables>;
        using ListItem = ExecutablesList::ListItem;

        // GUI Elements
        // List of Indexes definition
    public:

    protected:
        // GUI Layout
        QLineEdit phaseNameEditor;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}

        void readAction(ActionBase&) override;
    };

}


#endif // MODIFIER_ACTION_PANEL_CHANGEPHASE_HPP
