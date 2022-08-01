#ifndef ACTION_PANEL_COMPAREANDWRITE_HPP
#define ACTION_PANEL_COMPAREANDWRITE_HPP

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
#include"Tcl2CaplPanels/ConfigEditor/rulesprocedurepanel.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ActionsList{

    class CompareAndWriteActionView
            : public ActionDataView<ConditionalsFactory::ListOfBases>
    {

        static constexpr ActionType actionType = ConditionalsTypes::CompareAndWrite;
        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<QTreeWidget, QListWidget>;
        using Action = ConditionalsFactory::Product<actionType>;

        using RawRule = Tcl2CaplControllerConfig::RawRule;
    public:
        using ParentContextMenu = ContextMenuInterface::ParentInterface;
    private:
        class ExpectedArgumentsList : public ContextMenuInterface{
        private:
            using Super = QTreeWidget;
            class ItemDelegate : public QItemDelegate{
                ItemDelegate() = delete;
            public:
                explicit ItemDelegate(ExpectedArgumentsList& list, QObject* parent = nullptr) : QItemDelegate(parent), list(list){}

            protected:
                ExpectedArgumentsList& list;

                QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
                void setEditorData(QWidget* editor, const QModelIndex& index) const;
                void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
                void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
            };

            class EAL_Item : public QTreeWidgetItem{
            private:
                //EAL_Item();
            public:
                enum class ItemType{
                    IndexItem,
                    ArgumentItem,
                    EmptyStringItem
                };
                EAL_Item(ItemType type, QString value = QString()) : type_(type){
                    switch(type){
                    case ItemType::EmptyStringItem:
                        setText(2, "<Brak argumentu>");
                        setToolTip(2, "Brak argumentu");
                        break;
                    default:
                        setFlags(flags() | Qt::ItemIsEditable);
                        break;
                    }
                    if(not value.isEmpty())
                    {
                        switch(type){
                        case ItemType::IndexItem:{
                            setText(0, value);
                            setToolTip(0, toolTipText());
                        }
                            break;
                        case ItemType::ArgumentItem:{
                            setText(2, value);
                            setToolTip(2, toolTipText());
                        }
                            break;
                        default:
                            break;
                        }
                    }
                }

                EAL_Item(ItemType type, QString index, QStringList& arguments) : EAL_Item(type, index){
                   loadArguments(arguments);
                }
            protected:
                ItemType type_;
            public:
                inline ItemType type()const{return type_;}
                inline EAL_Item* parent()const{return static_cast<EAL_Item*>(QTreeWidgetItem::parent());}
                inline ExpectedArgumentsList* treeWidget()const{return static_cast<ExpectedArgumentsList*>(QTreeWidgetItem::treeWidget());}
                inline EAL_Item* child(int index) const{return static_cast<EAL_Item*>(QTreeWidgetItem::child(index));}

                void addArgument();
                void addEmptyStringArgument();
                void loadArguments(QStringList&);
                void readArguments(QStringList&);
                void loadArgument(QString&);
                inline QString readArgument(){return (type_ == ItemType::EmptyStringItem)? QString() : text(2);}
                QString toolTipText() const;
            };
            using ListItem = EAL_Item;
            using Self = ExpectedArgumentsList;
            using Request_ContextMenu_Func = void (Self::*)(ListItem*);

            enum class Request_ContextMenu{
                NewIndex,
                NewArgument,
                AddEmptyStringArg,
                EditIndex,
                EditArgument,
                RemoveIndex,
                RemoveArgument,
                RemoveAllIndexes,
                RemoveAllArguments,
                Size
            };

            template<Request_ContextMenu>
            void execRequest_ContextMenu(ListItem*);

            ListItem* curEditItem = nullptr;
            QString correctData;
            bool dataAccepted = false;
            inline void checkData(){
                if(curEditItem){
                    switch(curEditItem->type()){
                    case ListItem::ItemType::IndexItem:
                    {
                        bool convError = false;
                        if(curEditItem->text(0).toUInt(&convError), convError){
                            dataAccepted = false;
                        }else{
                            dataAccepted = true;
                            correctData = curEditItem->text(0);
                        }
                    }
                        break;
                    }
                }
            }

            inline void editItem(ListItem* item){
                curEditItem = item;
                QTreeWidget::editItem(item, (item->type() == ListItem::ItemType::IndexItem)? 0 : 2);
                checkData();
            }
        public:
            ExpectedArgumentsList();
            void addIndex();
            void addEditItem(ListItem*);
            void loadExpectedArguments(Action::ArgumentsMap& argumentsMap);
            void readExpectedArguments(Action::ArgumentsMap& argumentsMap);
            void clearEditItem();
            ParentContextMenu& parentContextMenu()const override;
            void extendContextMenu(ContextMenuConfig&)const override;
            void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*)override;
            QWidget& itemView()const; // Splitter -> Widget (Widget with Layout of ActionDataView) -> ActionView (Any - Conditional or Executable)
            QListWidget& itemListView()const; // ActionView -> Viewport -> List

            QSize sizeHint()const override{
                QSize&& sH = Super::sizeHint();
                QSize&& msH = minimumSizeHint();
                if(sH.height() > msH.height()){
                    return sH;
                }else{
                    return QSize(sH.width(), msH.height());
                }
            }
        protected:

            inline ListItem* currentItem()const{return static_cast<ListItem*>(QTreeWidget::currentItem());}
            inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(QTreeWidget::itemAt(p));}
            inline ListItem* topLevelItem(int index)const{return static_cast<ListItem*>(QTreeWidget::topLevelItem(index));}
            inline ListItem* itemFromIndex(const QModelIndex& index)const{return static_cast<ListItem*>(QTreeWidget::itemFromIndex(index));}
            bool eventFilter(QObject* oj, QEvent* ev)override;
            //void mouseReleaseEvent(QMouseEvent* ev)override;
            void contextMenuEvent(QContextMenuEvent* ev)override;

        };

       // ListItem& item_;
        QVBoxLayout centralLayout;
        QPushButton closeButton;
        QFormLayout settingsLayout;
        QLineEdit numbOfArgumentCondition;
        //RuleControlComboBox ruleControlComboBox;
        QSplitter quickRuleInput;
        ExpectedArgumentsList expectedArgumentsList;
        General::FormattedString::Panel formattedStringList;
        //OutputsList outputsList;

    public:
        CompareAndWriteActionView(QWidget*);
        CompareAndWriteActionView(QWidget*, ActionPtr);
        //CompareAndWriteActionView(CompareAndWriteActionView* item);
        ~CompareAndWriteActionView(){}

        static ActionDataView* create(QWidget*, ActionRef);
        //bool eventFilter(QObject* obj, QEvent* ev)override;
        //inline ListItem& item()const{return item_;}
        constexpr ActionType type()const override{return actionType;}
        void readAction(ActionBase&) override;

    };

}

#endif // ACTION_PANEL_COMPAREANDWRITE_HPP
