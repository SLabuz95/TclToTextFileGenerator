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
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include<QSizeGrip>

namespace Panels::Configuration::View::Rules{
    class RawRuleView;
}

namespace Panels::Configuration::View::ActionsList{

    using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
    template<class Base>
    using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<Base>;

    template<class Actions>
    class List;
    template<class Actions>
    class ListItem;    
    template<class Actions>
    class ActionView;

    template<class Actions>
    class ActionDataView : public QVBoxLayout{
    public:
        using ActionPtr = typename Actions::Type;
        using ActionBase = std::remove_pointer_t<ActionPtr>;
        using Error = QString;
        using ActionRef = ActionPtr;
        using ActionType = typename ActionBase::ProductsList;
        using ActionView = ActionView<Actions>;
        using DataView = ActionDataView<Actions>;

        using CreateFunction =  ActionDataView* (*)(QWidget*, ActionRef);
        using CreateFunctionTable = CreateFunction[];
     protected:
        ActionDataView(QWidget* parent)
            : QVBoxLayout(parent){}

        /*
        static constexpr CreateFunction initCreateFunctionTable(){
            for(ActionType productType = ActionType::FCT_Begin; productType < ActionType::FCT_End; productType++){
                //static_assert (std::is_base_of_v<ProductBase<productType>, Product<productType>>, "Invalid product base");
                createFunctionTable[toUnderlyng(productType)] = &Factory::create<productType>;
            }

            return createFunctionTable[0];
        }

        constexpr static CreateFunctionTable createFunctionTable = {initCreateFunctionTable()};
*/

        static CreateFunctionTable createFunctionTable;
    public:
        ~ActionDataView()override{
        }
        static ActionDataView* createNoDataView(QWidget*, ActionRef = nullptr){return nullptr;}
        static ActionDataView* createView(QWidget* parent, ActionRef);
        static ActionDataView* createView(QWidget* parent, ActionType);

        //virtual Action toAction() = 0;
        virtual constexpr ActionType type()const = 0;
        virtual void readAction(ActionBase&) = 0;

    };
    template<class Actions>
    class ActionView : public QWidget{
        using ActionPtr = typename Actions::Type;
        using Action = std::remove_pointer_t<ActionPtr>;
        using ActionType = typename Action::ProductsList;
        using ActionDataView = ActionDataView<Actions>;
        using MainLayout = QVBoxLayout;
        using ListItem = ListItem<Actions>;
        using List = List<Actions>;
        using Super = QWidget;
        class ActionTypeComboBox : public QComboBox{
        public:
            ActionTypeComboBox(){

                if constexpr (std::is_same_v<ActionType, Tcl::Command::Definition::Action::Conditional>)
                    addItems(Tcl::Command::Definition::Action::conditionalsNames());
                else
                    addItems(Tcl::Command::Definition::Action::executablesNames());
            }

        };
        public:
            ActionView(List& , ActionPtr);
            ~ActionView()override{

            }
            List& parentWidget()const;

            void readAction(ActionPtr& action);

        protected:
            bool eventFilter(QObject* obj, QEvent* ev)override;

            QFormLayout mainLayout;
            ActionTypeComboBox actionTypeComboBox;
            ActionDataView* dataView_ = nullptr;

            bool createActionDataView(ActionType);

    };

    template<class Actions>
    class ListItem : public QListWidgetItem{
        using ActionPtr = typename Actions::Type;
        using Action = std::remove_pointer_t<ActionPtr>;
    public:
        using List = List<Actions>;
        using View = ActionView<Actions>;
        ListItem() = delete;
        ListItem(List& list, ActionPtr action);
        ListItem(const ListItem& item)
            : ListItem(item.list(), nullptr)
        {
        }
        ~ListItem()override{
        }

        View view_;

    public:
        inline void readAction(ActionPtr& action){view_.readAction(action);}
        List &list() const; //{ return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }
        inline View& view(){return view_;}
    };

    template<class Actions>
    class List : public ContextMenuInterface<QListWidget>{
        using ActionsRef = Actions&;
        using ActionPtr = typename Actions::Type;
    public:
        using ListItem = ListItem<Actions>;
        using RawRuleView = Rules::RawRuleView;
        using Request_ContextMenu_Func = void (List::*)(ListItem*);
        using Super = ContextMenuInterface<QListWidget>;
        using ParentContextMenu = Utils::ContextMenuBuilder::Interface<QListWidget>;
        enum class Request_ContextMenu{
            Add,
            Clone,
            Remove,
            Clear,
            Size
        };
        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);

        List();
        ~List()override{
            for(int i = 0; i < count(); i++)
                itemWidget(item(i))->setParent(nullptr);
            // Dynamic objects - Remove as child - do not set null parent (In RawRules List, static objects are used)
        }

    protected:
        ListItem* lastPressedItem = nullptr;

    public:
        RawRuleView& parentWidget()const;
        inline ListItem* currentItem()const{return static_cast<ListItem*>(Super::currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(Super::itemAt(p));}

        inline ListItem* item(int index)const{return static_cast<ListItem*>(Super::item(index));}
        inline ListItem* lastItem()const{return item(count() - 1);}

        void loadActions(ActionsRef);
        void readActions(ActionsRef);

        inline void addNewItem(){
            //ListItem* newItem = nullptr;
            new ListItem(*this, nullptr);
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
           Super::mousePressEvent(ev);
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
                //lastPressedItem->view().setParent(nullptr);
                //setItemWidget(lastPressedItem, 0, nullptr);
            }


            QListWidget::dropEvent(ev);
        }

        void resizeEvent(QResizeEvent* resizeEv){
            if(resizeEv->size().height() != resizeEv->oldSize().height()){
                QListWidget& listWidget = parentWidget().parentWidget();
                QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
                if(item)
                    item->setSizeHint(parentWidget().sizeHint());
            }
            return QAbstractItemView::resizeEvent(resizeEv);
        }

        /*void dataChanged(const QModelIndex &topLeft,
                        const QModelIndex &bottomRight,
                        const QList<int> &roles = QList<int>())override
        {
            qDebug() << "actions data changed" << topLeft << bottomRight << roles;
            Super::dataChanged(topLeft, bottomRight, roles);
            qApp->processEvents();
            QListWidget& listWidget = parentWidget().parentWidget();
            QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
            item->setSizeHint(listWidget.itemWidget(item)->sizeHint());
        }*/

        QSize minimumSizeHint() const override{
            return  QSize(0, 0);
        }

        QSize sizeHint() const override{
            return (Super::count() >= 0)? Super::viewportSizeHint()+=QSize(0, 6): QSize(0, 0);
        }
        ParentContextMenu& parentContextMenu()const override;
        void extendContextMenu(ContextMenuConfig&)const override;
        void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*)override;
    };

}

#endif // PANEL_ACTIONS_HPP
