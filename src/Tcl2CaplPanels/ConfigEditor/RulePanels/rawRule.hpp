#ifndef PANEL_RAWRULE_HPP
#define PANEL_RAWRULE_HPP

#include"Tcl2Capl/controllerconfig.hpp"
#include<QListWidgetItem>
#include<QComboBox>
#include<QVBoxLayout>
#include<QFormLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QSplitter>
#include"Action/actions.hpp"

namespace Panels::Configuration::View::Rules{

    namespace RulesProcedurePanel{
        class ListItem;
        class RulesList;
    }

    class RawRuleView : public QWidget{

        class RuleControlComboBox : public QComboBox{
        public:
            RuleControlComboBox(){
                setFrame(false);
                addItems(Tcl::Command::Definition::Rule::getRuleControlFlagNames());
            }
        };
        using RawRule = Tcl2CaplControllerConfig::RawRule;
        using RawRuleRef = const RawRule&;
        using List = RulesProcedurePanel::RulesList;
        using Super = QWidget;
    public:
        using ListItem = RulesProcedurePanel::ListItem;
        RawRuleView(ListItem&);
        RawRuleView(ListItem&, RawRuleRef);
        RawRuleView(const RawRuleView&);
        ~RawRuleView()override{

        }        

        List& parentWidget()const;
    protected:
        using MainLayout = QVBoxLayout;
        using ControlPanel = QHBoxLayout;
        using SettingsLayout = QGridLayout;
        using ActionsSplitter = QSplitter;
        using Conditionals = ConditionalsFactory::ListOfBases;
        using ConditionalsList = View::ActionsList::List<Conditionals>;
        using Executables = ExecutablesFactory::ListOfBases;
        using ExecutablesList = View::ActionsList::List<Executables>;

        ListItem& item_;
        MainLayout centralLayout;
        ControlPanel controlPanel;
        QPushButton closeButton;
        SettingsLayout settingsLayout;
        QLabel ruleCOntrolLabel;
        RuleControlComboBox ruleControlComboBox;
        ActionsSplitter actionsPanel;
        ConditionalsList conditionalsList;
        ExecutablesList executablesList;
        void resizeEvent(QResizeEvent* ev)override;
        bool eventFilter(QObject* obj, QEvent* ev)override;
    };

}

#endif // PANEL_RAWRULE_HPP
