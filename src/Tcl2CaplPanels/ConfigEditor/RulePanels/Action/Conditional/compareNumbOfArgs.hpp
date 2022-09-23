#ifndef ACTION_PANEL_COMPARENUMBOFARGS_HPP
#define ACTION_PANEL_COMPARENUMBOFARGS_HPP

#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"

namespace Panels::Configuration::View::ActionsList{

    class CompareNumbOfArgsActionView
            : public ActionDataView<ConditionalsFactory::ListOfBases>
    {
        static constexpr ActionType actionType = ConditionalsTypes::CompareNumbOfArguments;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<QListWidget>;
        using Action = ConditionalsFactory::Product<actionType>;

        CompareNumbOfArgsActionView(QWidget*);
        CompareNumbOfArgsActionView(QWidget*, ActionPtr);
    public:
        using ParentContextMenu = ContextMenuInterface::Interface;
        static ActionDataView* create(QWidget*, ActionRef);
        using ActionView = ActionView;
    protected:
        using Conditionals = ConditionalsFactory::ListOfBases;
        using ConditionalsList = List<Conditionals>;
        using ListItem = ConditionalsList::ListItem;

        // GUI Elements
        // List of Indexes definition
    public:
        class ListOfIndexes : public ContextMenuInterface{
        public:
            using Super = ContextMenuInterface;
            class ItemDelegate : public QItemDelegate{
                ItemDelegate() = delete;
            public:
                explicit ItemDelegate(ListOfIndexes& list, QObject* parent = nullptr) : QItemDelegate(parent), list(list){}

            protected:
                ListOfIndexes& list;

                QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
                void setEditorData(QWidget* editor, const QModelIndex& index) const;
                void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
                void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
            };
            class ListItem : public QListWidgetItem{
            public:
                ListItem() = delete;
                inline ListItem(ListOfIndexes& list,QString str = QString()) : QListWidgetItem(str, &list){
                     setFlags(flags() | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);

                }
            };
            using Request_ContextMenu_Func = void (ListOfIndexes::*)(ListItem*);
            enum class Request_ContextMenu{
                Add,
                Edit,
                Remove,
                Clear,
                Size
            };
            template<Request_ContextMenu>
            void execRequest_ContextMenu(ListItem*);

            enum ChangeAction{
                DuplicatedError,
                New,
                Removed,
                NewAndRemoved,
                Changed,
                Restored,
                ClearedAll,
                NoAction
            };

            using Config = Tcl2CaplControllerConfig;
        public:
            ListOfIndexes(){                
                ItemDelegate* itemDelegateObj = new ItemDelegate(*this);
                setItemDelegate(itemDelegateObj);
                setSortingEnabled(true);
                setEditTriggers(QAbstractItemView::DoubleClicked);
                setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
                setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
                viewport()->installEventFilter(this);
            }
        protected:
            using Layout = QVBoxLayout;

            struct CurEditItemInfo{
                ListItem* item = nullptr;
                QString oldIndex;
            } curEditItemInfo;


            inline ListItem* currentItem()const{return static_cast<ListItem*>(Super::currentItem());}
            inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(Super::itemAt(p));}

            bool eventFilter(QObject* obj, QEvent* ev)override;
            void contextMenuEvent(QContextMenuEvent* ev)override;
            ChangeAction tryToManageIndexes(QString oldIndex, QString newIndex);
        public:

            QWidget& itemView()const; // Splitter -> Widget (Widget with Layout of ActionDataView) -> ActionView (Any - Conditional or Executable)
            QListWidget& itemListView()const; // ActionView -> Viewport -> List

            inline void restoreSavedData(){

            }
            void commitChanges(){}

            void loadIndexes(const Action::NumbOfArgumentsList&);
            void readAll(QStringList& list){
                list.reserve(count());
                list.resize(count());
                int i = 0;
                for(QStringList::Iterator index = list.begin(); index != list.end(); index++, i++)
                    (*index) = item(i)->text();
            }

            inline void clearChanges(){

            }

            void reloadGui();
            ParentContextMenu& parentContextMenu()const override;
            void extendContextMenu(ContextMenuConfig&)const override;
            void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*)override;
         };
    protected:
        // GUI Layout
        ListOfIndexes listOfIndexes;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}
        void readAction(ActionBase&) override;

    };

}

#endif // ACTION_PANEL_COMPARENUMBOFARGS_HPP
