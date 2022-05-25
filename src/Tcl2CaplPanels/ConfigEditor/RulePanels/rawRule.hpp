#ifndef PANEL_RAWRULE_HPP
#define PANEL_RAWRULE_HPP

#include"Tcl2Capl/controllerconfig.hpp"
#include<QListWidgetItem>
#include<QComboBox>
#include<QVBoxLayout>
#include<QFormLayout>
#include<QLineEdit>
#include<QPushButton>
namespace Panels::Configuration::View::Rules{

    namespace RulesProcedurePanel{
        class ListItem;
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

    public:
        using ListItem = RulesProcedurePanel::ListItem;
        RawRuleView(ListItem&);
        RawRuleView(ListItem&, RawRuleRef);
        RawRuleView(const RawRuleView&);
        ~RawRuleView()override{

        }

    protected:
        ListItem& item_;
        QVBoxLayout centralLayout;
        QPushButton closeButton;
        QFormLayout settingsLayout;
        RuleControlComboBox ruleControlComboBox;

        bool eventFilter(QObject* obj, QEvent* ev)override;
    };

}

#endif // PANEL_RAWRULE_HPP
