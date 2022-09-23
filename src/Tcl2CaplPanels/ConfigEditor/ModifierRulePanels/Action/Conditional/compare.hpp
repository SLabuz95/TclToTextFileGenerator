#ifndef MODIFIER_ACTION_PANEL_COMPARE_HPP
#define MODIFIER_ACTION_PANEL_COMPARE_HPP

#include"Tcl2CaplPanels/ConfigEditor/ModifierRulePanels/FormattedString/formattedString.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/General/multilineEditorDialog.hpp"

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ModifierActionsList::CompareActionView{

    class CompareActionView
            : public ActionDataView<ModifierActionsFactory::ListOfBases>
    {

        static constexpr ActionType actionType = ModifierActionTypes::Compare;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<QListWidget>;
        using Action = ModifierActionsFactory::Product<actionType>;

        CompareActionView(QWidget* );
        CompareActionView(QWidget* , ActionPtr);
    public:
        using ParentContextMenu = ContextMenuInterface::Interface;
        static ActionDataView* create(QWidget* , ActionRef);
        using ActionView = ActionView;
    protected:
        using ModifierActions = ModifierActionsFactory::ListOfBases;
        using ModifierActionsList = List<ModifierActions>;
        using ListItem = ModifierActionsList::ListItem;

        // GUI Elements
        // List of Indexes definition
    public:
        class ListOfIndexes : public ContextMenuInterface{
            class ListItem : public QListWidgetItem{
            public:
                ListItem() = delete;
                inline ListItem(ListOfIndexes& list, QString str = QString())
                    : QListWidgetItem(str, &list)
                {
                     setFlags(flags() | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);

                }
                bool multiLineItem = false;
                inline ListOfIndexes& parentList()const{return *static_cast<ListOfIndexes*>(QListWidgetItem::listWidget());}

            };

            using Request_ContextMenu_Func = void (ListOfIndexes::*)(ListItem*);
            enum class Request_ContextMenu{
                Add,
                Edit,
                MultiLineEdit,
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
                setWordWrap(true);
                setTextElideMode(Qt::TextElideMode::ElideMiddle);
                setTabKeyNavigation(true);
                setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
                setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
                viewport()->installEventFilter(this);
            }

            QSize sizeHint() const override{
                if(Super::sizeHint().height() > minimumSizeHint().height()){
                    return Super::sizeHint();
                }else{
                    return minimumSizeHint();
                }
            }
            using Super = ContextMenuInterface;
        protected:
            ActionView& parentWidget()const;
            using Layout = QVBoxLayout;

            struct CurEditItemInfo{
                ListItem* item = nullptr;
                QString oldIndex;
            } curEditItemInfo;

            inline ListItem* currentItem()const{return static_cast<ListItem*>(Super::currentItem());}
            inline ListItem* item(int row)const{return static_cast<ListItem*>(Super::item(row));}
            inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(Super::itemAt(p));}
            inline ListItem* itemFromIndex(const QModelIndex& p)const{return static_cast<ListItem*>(Super::itemFromIndex(p));}

            bool eventFilter(QObject* obj, QEvent* ev)override;
            void contextMenuEvent(QContextMenuEvent* ev)override;
            ChangeAction tryToManageIndexes(QString oldIndex, QString newIndex);

            void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) override;
            void rowsInserted(const QModelIndex &parent, int start, int end) override;
            bool edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event)override;
            void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)override;

            void processEditData(CurEditItemInfo&);

        public:

            inline void restoreSavedData(){

            }
            void commitChanges(){}

            void loadIndexes(QStringList);
            void readAll(QStringList& list){
                list.reserve(count());
                list.resize(count());
                int i = 0;
                for(QStringList::Iterator string = list.begin(); string != list.end(); string++, i++)
                    (*string) = item(i)->text();
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
        QSplitter splitter;
        ListOfIndexes listOfIndexes;
        ModifierFormattedString::Panel formattedStringList;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return actionType;}

        void readAction(ActionBase&) override;
    };

}


#endif // MODIFIER_ACTION_PANEL_COMPARE_HPP
