#ifndef PROCEDURESNORULESLIST_HPP
#define PROCEDURESNORULESLIST_HPP

#include<QListWidgetItem>
#include<QPushButton>
#include<QToolBar>
#include<QVBoxLayout>
#include<QTreeWidget>
#include"Tcl2Capl/controllerconfig.hpp"

class AnalysisPanel;
class ProceduresNoRulesList : public QWidget{

    using Config = Tcl2CaplControllerConfig;
    using Procedures = Tcl2CaplControllerConfig::Procedures;
    using ProceduresRef = Procedures&;
public:
    ProceduresNoRulesList(AnalysisPanel&);
    using Super = QWidget;
protected:
    using Layout = QVBoxLayout;
    using SavedProceduresPtr = Procedures*;
    using NewProcedures = Procedures;
    using RemovedProcedures = QStringList;

    AnalysisPanel&  analysisPanel;
    Layout layout;
    QTreeWidget list;
    bool eventFilter(QObject* obj, QEvent* ev)override;
public:
    void load(FunctionDefinitionsRef);
    inline void clear(){list.clear();}


};

#endif // PROCEDURESNORULESLIST_HPP
