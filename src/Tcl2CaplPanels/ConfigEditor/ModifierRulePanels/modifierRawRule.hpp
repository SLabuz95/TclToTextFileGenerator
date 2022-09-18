#ifndef PANEL_MODIFIERRAWRULE_HPP
#define PANEL_MODIFIERRAWRULE_HPP

#include"Tcl2Capl/controllerconfig.hpp"
#include<QListWidgetItem>
#include<QComboBox>
#include<QVBoxLayout>
#include<QFormLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QSplitter>
#include"Action/actions.hpp"
#include"Tcl2Capl/controllerconfiginfo.hpp"

namespace Panels::Configuration::View::ModifierRules{

    namespace RulesPhasePanel{
        class ListItem;
        class RulesList;
    }

    class RawRuleView : public QWidget{

        class RuleControlComboBox : public QComboBox{
        public:
            RuleControlComboBox(){
                setFrame(false);
                addItems(FCT::ModifierRules::ModifierControlFlagInfo::flagNames());
            }
        };

        using RulesRef = ControllerConfigInfo::ModifierNewRules;
        using Rule = std::remove_pointer_t<RulesRef::Type>;
        using RuleRef = Rule&;

        using RulesViewRef = ControllerConfigInfo::ModifierRulesView;
        using RawRuleRef = RulesViewRef::first_type;
        using List = RulesPhasePanel::RulesList;
        using Super = QWidget;
    public:
        using ListItem = RulesPhasePanel::ListItem;
        RawRuleView(ListItem&);
        RawRuleView(ListItem&, RawRuleRef);
        RawRuleView(const RawRuleView&);
        ~RawRuleView()override{

        }

        void readRule(RuleRef rule);

        List& parentWidget()const;
    protected:
        using MainLayout = QVBoxLayout;
        using ControlPanel = QHBoxLayout;
        using SettingsLayout = QGridLayout;
        using ActionsSplitter = QSplitter;
        using Conditionals = ModifierActionsFactory::ListOfBases;
        using ConditionalsList = View::ModifierActionsList::Panel<Conditionals>;
        using Executables = ModifierActionsFactory::ListOfBases;
        using ExecutablesList = View::ModifierActionsList::Panel<Executables>;

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
        bool eventFilter(QObject* obj, QEvent* ev)override;
    public:
        inline bool isConditionalsList(QWidget *const list)const{
            return list == &conditionalsList;
        }
        inline bool isExecutablesList(QWidget *const list)const{
            return list == &executablesList;
        }

        void activateActionMode();

    };

}

#endif // PANEL_MODIFIERRAWRULE_HPP
