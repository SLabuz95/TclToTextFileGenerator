#include"proceduresNotSatisfiedList.hpp"
#include<QApplication>
#include<QMouseEvent>
#include<QMenu>
#include<QMessageBox>
#include"Tcl2Capl/caplFunctionDefiniitions.hpp"
#include<QTreeWidgetItem>
#include"analysisPanel.hpp"


ProceduresNotSatisfiedList::ProceduresNotSatisfiedList(AnalysisPanel& panel)
    : analysisPanel(panel)
{
    // ToolBar Setup
    layout.addWidget(&list);

    list.viewport()->installEventFilter(this);
    list.setSortingEnabled(true);
    list.setHeaderLabel(QString());
    setLayout(&layout);
    show();
}

bool ProceduresNotSatisfiedList::eventFilter(QObject* obj, QEvent* ev){


    if(ev->type() == QEvent::MouseButtonDblClick and obj == list.viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        QTreeWidgetItem* item = list.itemAt(mev->pos());
        if(item){
            QTreeWidgetItem* parent = item->parent();
            if(parent){
                analysisPanel.loadNotSatisfiedRules(parent->text(0), item->text(0).toUInt());
            }else{
                analysisPanel.loadNotSatisfiedRules(item->text(0));
            }
        }
    }
    return Super::eventFilter(obj, ev);

}


void ProceduresNotSatisfiedList::load(FunctionDefinitionsRef definitionsRef){
    using Definitions = FunctionDefinitions::Definitions;
    FunctionDefinitions::Definitions &definitions = definitionsRef.definitionsOnNotSatisfiedRulesView();
    for(Definitions::Iterator definition = definitions.begin();
        definition != definitions.end();
        definition++)
    {
        QTreeWidgetItem* definitionItem = new QTreeWidgetItem(&list);
        definitionItem->setText(0, definition.key());
        using ParametersNumb = Definitions::mapped_type::Iterator;
        for(ParametersNumb parametersNumb = definition.value().begin();
            parametersNumb != definition.value().end();
            parametersNumb++)
        {

            definitionItem->addChild(new QTreeWidgetItem(definitionItem, {QString::number(parametersNumb.key())}));
        }
    }
}
