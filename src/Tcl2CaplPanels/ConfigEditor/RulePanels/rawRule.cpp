#include"rawRule.hpp"
#include"Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"
#include<QApplication>

using namespace Panels::Configuration::View::Rules;
using ListItemType = RulesProcedurePanel::ListItem;
using List = RulesProcedurePanel::RulesList;

RawRuleView::RawRuleView(ListItemType& item)
: item_(item){
    // Setup layout
    centralLayout.setSpacing(0);

    closeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    closeButton.installEventFilter(this);
    centralLayout.addWidget(&closeButton, 1, Qt::AlignRight);

    settingsLayout.addRow("Flaga kontrolna reguly: ", &ruleControlComboBox);
    centralLayout.addLayout(&settingsLayout, Qt::AlignTop);

    actionsPanel.addWidget(&conditionalsList);
    actionsPanel.addWidget(&executablesList);
    centralLayout.addWidget(&actionsPanel);

    setLayout(&centralLayout);

    // Rule available
    //if(rule){   // Exists
        /*QuickRule& quickRule = *static_cast<QuickRule*>(rule);
        // NumbOfArguments
        if(quickRule.getNumbOfArguments() != -1){
            numbOfArgumentCondition.setText(QString::number(quickRule.getNumbOfArguments()));
        }
        // ControlFlag
        ruleControlComboBox.setCurrentIndex(static_cast<std::underlying_type_t<ControlFlag>>(quickRule.controlFlag()));
        // Expected Arguments
        expectedArgumentsList.loadExpectedArguments(quickRule);*/
    //}
}


RawRuleView::RawRuleView(ListItemType& item, RawRuleRef rule)
: item_(item)
{
    // Setup layout

    centralLayout.setSpacing(0);

    closeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    closeButton.installEventFilter(this);
    centralLayout.addWidget(&closeButton, 1, Qt::AlignRight);

    settingsLayout.addRow("Flaga kontrolna reguly: ", &ruleControlComboBox);
    centralLayout.addLayout(&settingsLayout, Qt::AlignTop);


    setLayout(&centralLayout);

    // Rule available
    //if(rule){   // Exists
        /*QuickRule& quickRule = *static_cast<QuickRule*>(rule);
        // NumbOfArguments
        if(quickRule.getNumbOfArguments() != -1){
            numbOfArgumentCondition.setText(QString::number(quickRule.getNumbOfArguments()));
        }
        // ControlFlag
        ruleControlComboBox.setCurrentIndex(static_cast<std::underlying_type_t<ControlFlag>>(quickRule.controlFlag()));
        // Expected Arguments
        expectedArgumentsList.loadExpectedArguments(quickRule);*/
    //}
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
