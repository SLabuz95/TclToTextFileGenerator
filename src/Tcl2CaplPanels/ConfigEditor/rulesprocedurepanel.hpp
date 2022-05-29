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
#include"RulePanels/rawRule.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"

namespace Panels::Configuration::View::Rules::RulesProcedurePanel{
    class RulesList;

    class ListItem : public QListWidgetItem{
        using RawRuleView = Rules::RawRuleView;
        using RulesRef = Tcl2CaplControllerConfig::RawRules&;
        using RuleRef = Tcl2CaplControllerConfig::RawRule&;
        using Rule = Tcl2CaplControllerConfig::RawRule;
    public:
        using List = RulesList;
        ListItem() = delete;
        ListItem(RulesList& list);
        ListItem(RulesList& list, RuleRef rule);
        ListItem(const ListItem& item) : ListItem(item.rulesList()){
            //itemContent = new ItemContent(item.widget());
        }
        ~ListItem()override{

        }

        RawRuleView rawRuleView_;

    public:
        RulesList &rulesList() const; //{ return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }
        inline RawRuleView& rawRuleView(){return rawRuleView_;}
    };
    class RulesList : public QListWidget{
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using RulesRef = Tcl2CaplControllerConfig::RawRules&;
        using RuleRef = Tcl2CaplControllerConfig::RawRule&;
        using Rule = Tcl2CaplControllerConfig::RawRule;
    public:
        using Request_ContextMenu_Func = void (RulesList::*)(ListItem*);
        enum class Request_ContextMenu{
            Add,
            Clone,
            Remove,
            Clear,
            Size
        };
        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);

        RulesList();
        ~RulesList()override{
            for(int i = 0; i < count(); i++)
                itemWidget(item(i))->setParent(nullptr);
        }

    protected:
        ListItem* lastPressedItem = nullptr;

    public:
        inline ListItem* currentItem()const{return static_cast<ListItem*>(QListWidget::currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(QListWidget::itemAt(p));}

        inline ListItem* item(int index)const{return static_cast<ListItem*>(QListWidget::item(index));}
        inline ListItem* lastItem()const{return item(count() - 1);}

        void loadRules(RulesRef);

        inline void addNewItem(){
            //ListItem* newItem = nullptr;
            new ListItem(*this);
            //newItem->init();
        }

        inline void addNewItem(RuleRef rule){
            //ListItem* newItem = nullptr;
            new ListItem(*this, rule);
            //newItem->init();
        }

       void contextMenuEvent(QContextMenuEvent *e) override;

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
        void extendContextMenu(ContextMenuConfig&);
        void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*);
    };

    using Panel = RulesList;


}

#endif // RULESPROCEDUREPANEL_HPP
