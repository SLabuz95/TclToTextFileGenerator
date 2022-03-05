#include"analysisPanel.hpp"
#include<QHeaderView>
#include<QMimeData>
#include<External/RegExpCore/regexprcore.hpp>
#include"Tcl2Capl/controllerconfigmanager.hpp"
#include"app.hpp"
#include"proceduresNoRulesList.hpp"
#include"proceduresNotSatisfiedList.hpp"

AnalysisPanel::AnalysisPanel(FileInstanceResultPanel& panel, FunctionDefinitionsRef caplDefinitions)
    : resultPanel(panel),
      caplFunctionDefinitions_(caplDefinitions),
      proceduresNoRulesList(*this),
      proceduresNotSatisfiedList(*this),
    exceptionsPanel(*this)

{

    toolBox.addItem(&proceduresNotSatisfiedList, "Wyjątki od reguł");
    toolBox.addItem(&proceduresNoRulesList, "Wyjątki bez reguł");
    splitter.addWidget(&toolBox);
    splitter.addWidget(&exceptionsPanel);

    layout.setSpacing(0);
    layout.setContentsMargins(0, 0, 0, 0);
    layout.addWidget(&splitter, Qt::AlignTop);
    installEventFilter(this);

    setLayout(&layout);

}


AnalysisPanel::~AnalysisPanel(){

}

bool AnalysisPanel::eventFilter(QObject* obj, QEvent* ev){
    if(obj == this and ev->type() == QEvent::Close){
        delete obj;
        return true;
    }
    return QWidget::eventFilter(obj, ev);
}

void AnalysisPanel::loadNoRules(QString procedure, uint numbOfParameters){
    FunctionDefinitions::Definitions::Iterator caplDefinition;
    if((caplDefinition = caplFunctionDefinitions().definitionsOnNotSatisfiedRulesView().find(procedure)) != caplFunctionDefinitions().definitionsOnNoRulesView().end()){
        FunctionDefinitions::DefinitionInfo::Iterator caplDefinitionInfo;
        if((caplDefinitionInfo = caplDefinition->find(numbOfParameters)) != caplDefinition->end()){
            exceptionsPanel.load(caplDefinitionInfo);
        }
    }
}
void AnalysisPanel::loadNotSatisfiedRules(QString procedure, uint numbOfParameters){
    FunctionDefinitions::Definitions::Iterator caplDefinition;
    if((caplDefinition = caplFunctionDefinitions().definitionsOnNotSatisfiedRulesView().find(procedure)) != caplFunctionDefinitions().definitionsOnNoRulesView().end()){
        FunctionDefinitions::DefinitionInfo::Iterator caplDefinitionInfo;
        if((caplDefinitionInfo = caplDefinition->find(numbOfParameters)) != caplDefinition->end()){
            exceptionsPanel.load(caplDefinitionInfo);
        }
    }
}
void AnalysisPanel::loadNoRules(QString procedure){
    FunctionDefinitions::Definitions::Iterator caplDefinition;
    if((caplDefinition = caplFunctionDefinitions().definitionsOnNoRulesView().find(procedure)) != caplFunctionDefinitions().definitionsOnNoRulesView().end()){

        exceptionsPanel.load(caplDefinition);
    }
}
void AnalysisPanel::loadNotSatisfiedRules(QString procedure){
    FunctionDefinitions::Definitions::Iterator caplDefinition;
    if((caplDefinition = caplFunctionDefinitions().definitionsOnNoRulesView().find(procedure)) != caplFunctionDefinitions().definitionsOnNoRulesView().end()){
        exceptionsPanel.load(caplDefinition);

    }
}

void AnalysisPanel::reloadGui(){
    proceduresNoRulesList.clear();
    proceduresNoRulesList.load(caplFunctionDefinitions());
    proceduresNotSatisfiedList.clear();
    proceduresNotSatisfiedList.load(caplFunctionDefinitions());
}


