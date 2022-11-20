#include"rawRule.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"
#include<QApplication>

using namespace Panels::Configuration::View::Rules;
using ListItemType = RulesProcedurePanel::ListItem;
using List = RulesProcedurePanel::RulesList;

RawRuleView::RawRuleView(ListItemType& item)
: item_(item){
    // Setup layout
    //setParent(list.viewport());
    centralLayout.setSpacing(0);
    centralLayout.setContentsMargins(QMargins());

    setLayout(&centralLayout);
    closeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    closeButton.installEventFilter(this);
    controlPanel.setContentsMargins(QMargins());
    controlPanel.addWidget(&closeButton, 0, Qt::AlignRight);
    centralLayout.addLayout(&controlPanel);

    settingsLayout.addWidget(&ruleCOntrolLabel, 0,0);
    ruleCOntrolLabel.setText("Flaga reguły");
    settingsLayout.addWidget(&ruleControlComboBox, 0,1);
    settingsLayout.setColumnStretch(1, 1);
    settingsLayout.setContentsMargins(QMargins());
    //settingsLayout.setVerticalSpacing(0);
    centralLayout.addLayout(&settingsLayout);

    actionsPanel.addWidget(&conditionalsList);
    actionsPanel.addWidget(&executablesList);
    centralLayout.addWidget(&actionsPanel);

}


RawRuleView::RawRuleView(List& list, ListItemType& item, RawRuleRef rule)
: RawRuleView(item)
{    
    setParent(list.viewport());
    // Rule available
    if(rule){   // Exists
        using RawRule = Tcl2CaplControllerConfig::RawRule;
        using RawRuleRef = RawRule&;
        RawRuleRef rawRule = *static_cast<RawRule*>(*rule);
        // ControlFlag
        ruleControlComboBox.setCurrentIndex(static_cast<std::underlying_type_t<ControlFlag>>(rawRule.controlFlag()));
        // Conditionals
        conditionalsList.loadActions(rawRule.conditions());
        // Executables
        executablesList.loadActions(rawRule.executables());
    }
}

void RawRuleView::readRule(RuleRef rule){
    using RawRule = Tcl2CaplControllerConfig::RawRule;
    using RawRuleRef = RawRule&;
    RawRuleRef rawRule = *static_cast<RawRule*>(&rule);
    rawRule.setControlFlag(static_cast<ControlFlag>(ruleControlComboBox.currentIndex()));
    // Conditionals
    conditionalsList.readActions(rawRule.conditions());
    // Executables
    executablesList.readActions(rawRule.executables());
}


/*
RawRuleView::RawRuleView(RawRuleView* itemF)
: RawRuleView(itemF->item()){
    centralLayout.setSpacing(0);

    closeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    closeButton.installEventFilter(this);
    centralLayout.addWidget(&closeButton, 1, Qt::AlignRight);

    numbOfArgumentCondition.setValidator(new QRegularExpressionValidator(QRegularExpression("(\\>[\\d]+|[\\d+]+)?")));
    settingsLayout.addRow("Liczba argumentów: ", &numbOfArgumentCondition);
    ruleControlComboBox.setCurrentText(
                Tcl::Command::Definition::Rule::toStr(
                    Tcl::Command::Definition::Rule::Control::BreakRuleCheck));


    settingsLayout.addRow("Flaga kontrolna reguly: ", &ruleControlComboBox);

    centralLayout.addLayout(&settingsLayout, Qt::AlignTop);

    expectedArgumentsList.setHeaderLabels({"Indeksy", "Aktywne", "Spodziewane argumenty"});
    expectedArgumentsList.setSortingEnabled(true);
    quickRuleInput.addWidget(&expectedArgumentsList);
    //outputsList.setHeaderLabels({"Format", "Zawartość"});
    quickRuleInput.addWidget(&outputsList);

    centralLayout.addWidget(&quickRuleInput);

    setLayout(&centralLayout);
}
*/
bool RawRuleView::eventFilter(QObject *obj, QEvent *ev){
    if(obj == &closeButton and ev->type() == QEvent::MouseButtonPress){
        delete &item_;
        return true;
    }
    return QWidget::eventFilter(obj, ev);
}

List& RawRuleView::parentWidget()const{
    return *static_cast<List*>(Super::parentWidget()->parentWidget());// Viewport -> List
}
