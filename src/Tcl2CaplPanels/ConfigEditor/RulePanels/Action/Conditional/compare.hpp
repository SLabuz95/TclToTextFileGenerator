#ifndef ACTION_PANEL_COMPARE_HPP
#define ACTION_PANEL_COMPARE_HPP

#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include<QDialog>
#include<QTextEdit>

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ActionsList::CompareActionView{

    class CompareActionView
            : public ActionDataView<ConditionalsFactory::ListOfBases>
    {
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<QListWidget>;

        CompareActionView(ActionView&);
    public:
        using ParentContextMenu = ContextMenuInterface::Interface;
        static ActionDataView* create(ActionView&, ActionRef);
        using ActionView = ActionView;
    protected:
        using Conditionals = ConditionalsFactory::ListOfBases;
        using ConditionalsList = List<Conditionals>;
        using ListItem = ConditionalsList::ListItem;

        // GUI Elements
        // List of Indexes definition
    public:
        class ListOfIndexes : public ContextMenuInterface{
            class ListItem : public QListWidgetItem{
            public:
                inline ListItem(QString str = QString())
                    : QListWidgetItem(str)
                {
                     setFlags(flags() | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);

                }
                bool multiLineItem = false;
                inline ListOfIndexes& parentList()const{return *static_cast<ListOfIndexes*>(QListWidgetItem::listWidget());}

            };
            class MultiLineEditor : public QDialog{
            public:
                MultiLineEditor(QString text)
                    : QDialog()
                {
                    QTextOption to;
                    to.setFlags(QTextOption::IncludeTrailingSpaces | QTextOption::ShowTabsAndSpaces);
                    setSizeGripEnabled(true);
                    setModal(true);

                    titleLabel.setText("Edytor");

                    ok.setText("Ok");
                    cancel.setText("Cancel");
                    buttons.addWidget(&ok);
                    buttons.addWidget(&cancel);

                    layout.addWidget(&titleLabel);
                    layout.addWidget(&textEditor);
                    layout.addLayout(&buttons);
                    ok.installEventFilter(this);
                    cancel.installEventFilter(this);

                    setLayout(&layout);
                    textEditor.document()->setDefaultTextOption(to);
                    textEditor.setPlainText(text);
                }

            protected:
                QVBoxLayout layout;
                QLabel titleLabel;
                QTextEdit textEditor;
                QHBoxLayout buttons;
                QPushButton ok;
                QPushButton cancel;

                bool eventFilter(QObject* obj, QEvent* ev)override;
            public:
                QString text()const{return textEditor.toPlainText();}
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
            ParentContextMenu& parentContextMenu()const override;
            void extendContextMenu(ContextMenuConfig&)const override;
            void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*)override;
         };
    protected:
        // GUI Layout
        QSplitter splitter;
        ListOfIndexes listOfIndexes;
        General::FormattedString::List formattedStringList;

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return ConditionalsTypes::Compare;}

    };

}

#endif // ACTION_PANEL_COMPARE_HPP
