#ifndef ACTION_PANEL_COMPARE_HPP
#define ACTION_PANEL_COMPARE_HPP

#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"

namespace Panels::Configuration::View::ActionsList::CompareActionView{

    class CompareActionView
            : public ActionDataView<ConditionalsFactory::ListOfBases>
    {
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;

        CompareActionView(ActionView&);
    public:
        static ActionDataView* create(ActionView&, ActionRef);
        using ActionView = ActionView;
    protected:
        using Conditionals = ConditionalsFactory::ListOfBases;
        using ConditionalsList = List<Conditionals>;
        using ListItem = ConditionalsList::ListItem;

        // GUI Elements
        // List of Indexes definition
    public:
        class ListOfIndexes : public QListWidget{
            class ListItem : public QListWidgetItem{
            public:
                inline ListItem(QString str = QString()) : QListWidgetItem(str){
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
                setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                viewport()->installEventFilter(this);
            }
            using Super = QListWidget;
        protected:
            ActionView& parentWidget()const;
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
            inline bool anyChanges()const{
                //return not (newProcedures.isEmpty() and removedProcedures.isEmpty());
            }

            inline void restoreSavedData(){

            }
            void commitChanges(){}

            void loadIndexes();

            inline void clearChanges(){

            }

            void reloadGui();
            void extendContextMenu(ContextMenuConfig&);
            void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*);
        };
    protected:
        // GUI Layout
        ListOfIndexes listOfIndexes;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return ConditionalsTypes::CompareNumbOfArguments;}

    };

}

#endif // ACTION_PANEL_COMPARE_HPP
