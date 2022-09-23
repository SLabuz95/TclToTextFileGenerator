#ifndef MODIFIER_ACTION_PANEL_WRITE_HPP
#define MODIFIER_ACTION_PANEL_WRITE_HPP

#include"Tcl2CaplPanels/ConfigEditor/ModifierRulePanels/FormattedString/formattedString.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ModifierActionsList{

    class WriteActionView
            : public ActionDataView<ModifierActionsFactory::ListOfBases>
    {
        static constexpr ActionType actionType = ModifierActionTypes::Write;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using Action = ModifierActionsFactory::Product<actionType>;

        WriteActionView(QWidget*);
        WriteActionView(QWidget*, ActionPtr);
        ~WriteActionView()override{
        }
    public:
        static ActionDataView* create(QWidget*, ActionRef);
        using ActionView = ActionView;
    protected:
        using Executables = ExecutablesFactory::ListOfBases;
        using ExecutablesList = List<Executables>;
        using ListItem = ExecutablesList::ListItem;

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

#endif // MODIFIER_ACTION_PANEL_WRITE_HPP
