#ifndef EXCEPTIONSPANEL_HPP
#define EXCEPTIONSPANEL_HPP

#include<QListWidgetItem>
#include<QComboBox>
#include"tclToCAPL.hpp"
#include<QVBoxLayout>
#include<QFormLayout>
#include<QLineEdit>
#include<QSplitter>
#include<QTreeWidget>
#include<QMouseEvent>
#include<QPushButton>
#include<QToolBar>
#include<QLabel>
#include<QApplication>
#include"Tcl2Capl/controllerconfig.hpp"
#include<QItemDelegate>
#include"Tcl2Capl/caplFunctionDefiniitions.hpp"

class AnalysisPanel;
class ExceptionsPanel : public QWidget{
    using ProcedureRef = Tcl2CaplControllerConfig::Procedure&;
public:

        class ExceptionsList : public QTreeWidget{
             /*using ColumnNames = const QStringList;
              ColumnNames columns =
              {
                QStringLiteral("Nazwa"),
                QStringLiteral("Ustawienia")
              };*/

        public:
            /*
            public:
                ListItem() = delete;
                ListItem(RulesList& list, QuickRuleRef rule = nullptr){
                    list.addItem(this);
                    itemContent = new ItemContent(*this, rule);
                    listWidget().setItemWidget(this, itemContent);
                    setSizeHint(itemContent->sizeHint());
                }
                ListItem(const ListItem& item) : ListItem(item.listWidget()){
                    itemContent = new ItemContent(item.widget());
                }
                ~ListItem(){}

                ItemContent* itemContent = nullptr;

            public:
                inline RulesList &listWidget() const { return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }

                inline ItemContent* widget()const{return static_cast<ItemContent*>(itemContent);}
            };
*/
            ExceptionsList(ExceptionsPanel&);
            ~ExceptionsList(){}

        protected:
            QString toString(CAPLFunctionDefinitions::Parameters params);

        public:
            void load(CAPLFunctionDefinitions::Definitions::Iterator);
            void load(CAPLFunctionDefinitions::DefinitionInfo::Iterator);


        };

    using Layout = QVBoxLayout;
    using TitleBar = QLabel;

    ExceptionsPanel(AnalysisPanel&);
    QVBoxLayout layout;
    ExceptionsList exceptionsList;


public:
    inline void load(CAPLFunctionDefinitions::Definitions::Iterator iter){exceptionsList.load(iter);}
    inline void load(CAPLFunctionDefinitions::DefinitionInfo::Iterator iter){exceptionsList.load(iter);}

};


#endif // EXCEPTIONSPANEL_HPP
