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
#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"../actions.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include"Tcl2CaplPanels/ConfigEditor/RulePanels/FormattedString/formattedString.hpp"

namespace General = Panels::Configuration::View;
namespace Panels::Configuration::View::ActionsList{

    class CompareAndWriteActionView
            : public ActionDataView<ExecutablesFactory::ListOfBases>
    {

        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
        using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<QTreeWidget, QListWidget>;

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
                void loadArgument(QString&);
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
            void loadExpectedArguments(RawRule& rule);
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
/*
        class OutputsList : public QListWidget{
        public:

        protected:
            using Self = OutputsList;
            using Super = QListWidget;
            class OL_Item : public QListWidgetItem{
                using Super = QListWidgetItem;
                using Parent = Self;
                using Self = OL_Item;

                enum class ItemType{
                    PlainTextItem,
                    NameItem,
                    IndexItem,
                    ArgumentsFromItem,
                   // SeparatorItem,
                    FormatItem,
                    Size
                };

                class ItemContent : public QWidget{
                    using Parent = Self;
                    using Self = ItemContent;
                    using Super = QWidget;
                public:
                    using OutputContent_CreateContent_Func = void (Self::*)();
                    inline constexpr static std::underlying_type_t<ItemType> toULying(Parent::ItemType type){
                        return static_cast<std::underlying_type_t<ItemType>>(type);
                    }
                    inline constexpr static ItemType fromULying(std::underlying_type_t<ItemType> type){
                        return static_cast<ItemType>(type);
                    }
                    inline constexpr static std::underlying_type_t<ItemType> numbOfItemTypes(){
                        return static_cast<std::underlying_type_t<ItemType>>(Parent::ItemType::Size);
                    }

                    template<Parent::ItemType itemToCheck, Parent::ItemType itemTypeCreatingLayout = Parent::ItemType::Size, Parent::ItemType ...itemTypesCreatingLayout>
                    inline constexpr static bool isOutputContentLayoutCreatedForSpecifiedType(){
                        return (itemTypeCreatingLayout == ItemType::Size)? false : ((itemToCheck == itemTypeCreatingLayout)
                                || Self::isOutputContentLayoutCreatedForSpecifiedType<itemToCheck, itemTypesCreatingLayout...>());

                    }

                protected:
                    using MainLayout = QFormLayout;
                    using OutputTitleLayout = QHBoxLayout;
                    using OutputTitleComboBox = QComboBox;
                    using OutputTitleRemoveButton = QPushButton;
                    using OutputContentLayout = QLayout*;

                    MainLayout mainLayout;
                    OutputTitleLayout outputTitleLayout;
                    OutputTitleComboBox outputTitleComboBox;
                    OutputTitleRemoveButton outputTitleRemoveButton;
                    OutputContentLayout outputContentLayout = nullptr;

                    Parent::ItemType curType = Parent::ItemType::Size;

                    inline void createOutputContentLayout_priv(Parent::ItemType type){
                        if(type != curType){
                            if(outputContentLayout){
                                mainLayout.removeRow(mainLayout.rowCount() - 1);
                                outputContentLayout = nullptr;
                            }
                            curType = type;
                            (this->*createOutputContentFunctions[Self::toULying(curType)])();
                            qApp->processEvents();
                            parent->setSizeHint(sizeHint());
                        }
                    }

                    template<Parent::ItemType type>
                    void createOutputContentLayout();

                    static const OutputContent_CreateContent_Func createOutputContentFunctions[];

                    template<Parent::ItemType>
                    class OutputContent;

                    bool eventFilter(QObject* obj, QEvent* ev) override;

                public:
                    ItemContent(Parent*);
                    Parent* parent = nullptr;

                };
            public:
                OL_Item(Parent* parent) : Super(parent), itemContent(new ItemContent(this)){
                    init();
                }

            protected:
                ItemContent* itemContent = nullptr;

                inline void init(){
                    if(listWidget()){
                        listWidget()->setItemWidget(this, itemContent);
                        setSizeHint(itemContent->sizeHint());
                    }
                }

                inline ItemContent* widget(){return static_cast<ItemContent*>(itemContent);}
            public:
                //inline ItemType type()const{return type_;}
                inline Parent* listWidget()const{return static_cast<Parent*>(Super::listWidget());}

            };

            using ListItem = OL_Item;
            using Request_ContextMenu_Func = void (Self::*)(ListItem*);

            enum class Request_ContextMenu{
                New,
                Remove,
                RemoveAll,
                Size
            };

            template<Request_ContextMenu>
            void execRequest_ContextMenu(ListItem*);

            //using OutputOption2FormatRuleMap = const QMap<const QString, FormatParametersType>;
            using OutputOption2FormatRuleMap = const QStringList;
            OutputOption2FormatRuleMap outputOption2FormatRuleMap =
            {
                "Tekst",
                "Nazwa procedury",
                "Argument o indeksie",
                "Wszystkie argumenty od wybranego indeksu",
                //"Separator",
                "Format"
            };

        public:
            OutputsList();
        protected:
            ListItem* lastPressedItem = nullptr;

            inline ListItem* currentItem()const{return static_cast<ListItem*>(Super::currentItem());}
            inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(Super::itemAt(p));}
            inline ListItem* item(int index)const{return static_cast<ListItem*>(QListWidget::item(index));}
            inline ListItem* lastItem()const{return item(count() - 1);}

            // bool eventFilter(QObject* oj, QEvent* ev)override;
            //void mouseReleaseEvent(QMouseEvent* ev) override;
            void contextMenuEvent(QContextMenuEvent* ev)override;
            inline void addNewOutput(){
                addItem(new ListItem(this));
            }
            void loadOutputs(RawRule&);

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
*/

       // ListItem& item_;
        QVBoxLayout centralLayout;
        QPushButton closeButton;
        QFormLayout settingsLayout;
        QLineEdit numbOfArgumentCondition;
        //RuleControlComboBox ruleControlComboBox;
        QSplitter quickRuleInput;
        ExpectedArgumentsList expectedArgumentsList;
        General::FormattedString::List formattedStringList;
        //OutputsList outputsList;

    public:
        CompareAndWriteActionView(ActionView&);
        //CompareAndWriteActionView(CompareAndWriteActionView* item);
        ~CompareAndWriteActionView(){}

        static ActionDataView* create(ActionView&, ActionRef);
        //bool eventFilter(QObject* obj, QEvent* ev)override;
        //inline ListItem& item()const{return item_;}
        constexpr ActionType type()const override{return ExecutablesTypes::CompareAndWrite;}

    };

}

#endif // ACTION_PANEL_COMPAREANDWRITE_HPP
