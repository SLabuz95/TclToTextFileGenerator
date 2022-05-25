#ifndef PANEL_ACTIONS_HPP
#define PANEL_ACTIONS_HPP

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

namespace Panels::Configuration::View::ActionsList{

    template<class Actions, class ActionView>
    class ActionsList;

    template<class Actions, class ActionView>
    class ListItem : public QListWidgetItem{
        using View = ActionView;
        using ViewPtr = ActionView*;
        using Action = typename Actions::Type;
        using ActionsList = ActionsList<Actions, ActionView>;
    public:
        ListItem() = delete;
        ListItem(ActionsList& list);
        ListItem(ActionsList& list, Action action);
        ListItem(const ListItem& item)
            : ListItem(item.actionsList())
        {}
        ~ListItem()override{

        }

        ViewPtr view_ = nullptr;

    public:
        ActionsList &actionsList() const; //{ return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }
        inline ViewPtr view(){return view_;}
    };

    template<class Actions, class ActionView>
    class ActionsList : public QListWidget{
        using ActionsRef = Actions&;
        using ActionPtr = typename Actions::Type;
    public:
        using ListItem = ListItem<Actions, ActionView>;
        using Request_ContextMenu_Func = void (ActionsList::*)(ListItem*);
        enum class Request_ContextMenu{
            Add,
            Clone,
            Remove,
            Clear,
            Size
        };
        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);

        ActionsList();
        ~ActionsList()override{
            /*for(int i = 0; i < count(); i++)
                itemWidget(item(i))->setParent(nullptr);*/
            // Dynamic objects - Remove as child - do not set null parent (In RawRules List, static objects are used)
        }

    protected:
        ListItem* lastPressedItem = nullptr;

    public:
        inline ListItem* currentItem()const{return static_cast<ListItem*>(QListWidget::currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(QListWidget::itemAt(p));}

        inline ListItem* item(int index)const{return static_cast<ListItem*>(QListWidget::item(index));}
        inline ListItem* lastItem()const{return item(count() - 1);}

        void loadActions(ActionsRef);

        inline void addNewItem(){
            //ListItem* newItem = nullptr;
            new ListItem(*this);
            //newItem->init();
        }

        inline void addNewItem(ActionPtr action){
            //ListItem* newItem = nullptr;
            new ListItem(*this, action);
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
    };

}

#endif // PANEL_ACTIONS_HPP
