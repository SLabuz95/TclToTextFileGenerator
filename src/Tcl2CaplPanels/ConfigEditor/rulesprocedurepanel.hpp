#ifndef RULESPROCEDUREPANEL_HPP
#define RULESPROCEDUREPANEL_HPP

#include<QListWidgetItem>
#include<QComboBox>
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

class RulesProcedurePanel : public QWidget{
    using ProcedureRef = Tcl2CaplControllerConfig::Procedure&;
public:
    class RawRulesList : public QWidget{
        using QuickRulesRef = Tcl2CaplControllerConfig::RawRuleRefs&;
        using QuickRuleRef = Tcl2CaplControllerConfig::RawRuleRef;
        using RawRule = Tcl2CaplControllerConfig::RawRule;

    public:
        enum class ToolOptions : uint{
            Begin,
            New = ToolOptions::Begin,
            ClearAll,

            NumbOfOptions
        };

        class RulesList : public QListWidget{
             /*using ColumnNames = const QStringList;
              ColumnNames columns =
              {
                QStringLiteral("Nazwa"),
                QStringLiteral("Ustawienia")
              };*/
            class RuleControlComboBox : public QComboBox{
            public:
                RuleControlComboBox(){
                    addItems(Tcl::Command::Definition::Rule::getRuleControlFlagNames());
                }
            };

        public:
            class ListItem : public QListWidgetItem{

            public:
                ListItem() = delete;
                ListItem(RulesList& list, QuickRuleRef rule = nullptr){
                    list.addItem(this);
                    //itemContent = new ItemContent(*this, rule);
                    //listWidget().setItemWidget(this, itemContent);
                    //setSizeHint(itemContent->sizeHint());
                }
                ListItem(const ListItem& item) : ListItem(item.listWidget()){
                    //itemContent = new ItemContent(item.widget());
                }
                ~ListItem(){}

                QWidget* itemContent = nullptr;

            public:
                inline RulesList &listWidget() const { return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }

                inline QWidget* widget()const{return static_cast<QWidget*>(itemContent);}
            };

            RulesList();
            ~RulesList(){}

        protected:
            ListItem* lastPressedItem = nullptr;

        public:
            inline ListItem* currentItem()const{return static_cast<ListItem*>(QListWidget::currentItem());}
            inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(QListWidget::itemAt(p));}

            inline ListItem* item(int index)const{return static_cast<ListItem*>(QListWidget::item(index));}
            inline ListItem* lastItem()const{return item(count() - 1);}

            void loadRules(QuickRulesRef);

            inline void addNewItem(){
                //ListItem* newItem = nullptr;
                new ListItem(*this);
                //newItem->init();
            }

            inline void addNewItem(QuickRuleRef rule){
                //ListItem* newItem = nullptr;
                new ListItem(*this, rule);
                //newItem->init();
            }

           void mousePressEvent(QMouseEvent* ev)override{
               lastPressedItem = itemAt(ev->pos());
               QListWidget::mousePressEvent(ev);
           }

            void dropEvent(QDropEvent* ev)override{
                ListItem* dropItem = itemAt(ev->position().toPoint());
                if(lastPressedItem){
                    const QPoint dropPos = ev->position().toPoint();
                    if(lastPressedItem == lastItem()){   // Is last Item // next condition - drop point is below or equal bottom of item rect - ignore
                        if(visualItemRect(lastPressedItem).bottom() <= dropPos.y())
                            return; //WARNING: Ignore DropEvent (Bug: item is deleted with item content)
                    }else{  //  Drop region is in upper half of rect of item below - Ignore
                        if( // Drop item is below drag item
                                row(lastPressedItem) + 1 == row(dropItem) and
                            // Drop region is in upper half of rect of item below
                                dropPos.y() >= visualItemRect(lastPressedItem).bottom() and
                                dropPos.y() <= visualItemRect(item(row(lastPressedItem) + 1)).center().y())
                        {
                            return; // Ignore
                        }
                    }
                }

                QListWidget::dropEvent(ev);
            }
        };
        using ListItem = RulesList::ListItem;
        using Request_ContextMenu_Func = void (RawRulesList::*)(ListItem*);
        enum class Request_ContextMenu{
            AddRule,
            CloneRule,
            RemoveRule,
            ClearRules,
            Size
        };
        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);

    public:
        RawRulesList();
        using Super = QWidget;
    protected:

        using Layout = QVBoxLayout;
        Layout layout;
        RulesList list;

        //inline ListItem* currentItem()const{return static_cast<ListItem*>(list.currentItem());}
        //inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(list.itemAt(p));}

        //bool eventFilter(QObject* obj, QEvent* ev)override;
        void contextMenuEvent(QContextMenuEvent *event) override;

        inline ListItem* currentItem()const{return static_cast<ListItem*>(list.currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(list.itemAt(p));}

    public:
        inline void loadRules(QuickRulesRef rules){list.loadRules(rules);}

    };

    using Layout = QVBoxLayout;
    using TitleBar = QLabel;

    RulesProcedurePanel();
    QVBoxLayout layout;
    RawRulesList quickRulesList;


public:
    void loadProcedure(ProcedureRef procedureRef);
};

#endif // RULESPROCEDUREPANEL_HPP
