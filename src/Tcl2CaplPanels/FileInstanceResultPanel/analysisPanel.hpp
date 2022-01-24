#ifndef ANALYSISPANEL_HPP
#define ANALYSISPANEL_HPP

#include<QBoxLayout>
#include<QTabWidget>
#include<QListWidget>
#include<QTreeWidget>
#include<QToolBar>
#include<QPushButton>
#include<QApplication>
#include<QMouseEvent>
#include<QSplitter>
#include<QLabel>
#include<QToolBox>
#include<QTableWidget>
#include<QTableWidgetItem>
#include<QComboBox>
#include<QLineEdit>
#include<QHeaderView>
#include"tclToCAPL.hpp"
#include<QFormLayout>
#include"exceptionsPanel.hpp"
#include"proceduresNoRulesList.hpp"
#include"proceduresNotSatisfiedList.hpp"

class App;
class ActionsView;
class FileInstanceResultPanel;
class AnalysisPanel : public QWidget{
public:
    using Config = Tcl2CaplControllerConfig;
    using ConfigRef = Config&;
    using ConfigPtr = Config*;

    AnalysisPanel(FileInstanceResultPanel&, CAPLFunctionDefinitionsRef caplDefinitions);
    virtual ~AnalysisPanel() override;

protected:
    using Layout = QVBoxLayout;
    using Splitter = QSplitter;

    using RulesView = QTreeWidget;
    using NoSelectedProcedurePanel =  QLabel;

    FileInstanceResultPanel& resultPanel;

    Layout layout;
    // NO GUI ELEMENT
    CAPLFunctionDefinitionsRef caplFunctionDefinitions_;
    // ---------------------
    Splitter splitter;
    QToolBox toolBox;
    ProceduresNoRulesList proceduresNoRulesList;
    ProceduresNotSatisfiedList proceduresNotSatisfiedList;
    ExceptionsPanel exceptionsPanel;
    NoSelectedProcedurePanel noSelectedProcedurePanel;



    bool eventFilter(QObject*, QEvent*) override;

public:
    void loadNoRules(QString, uint);
    void loadNotSatisfiedRules(QString, uint);
    void loadNoRules(QString);
    void loadNotSatisfiedRules(QString);

    void reloadGui();
    inline CAPLFunctionDefinitionsRef caplFunctionDefinitions(){return caplFunctionDefinitions_;}

};


#endif // ANALYSISPANEL_HPP
