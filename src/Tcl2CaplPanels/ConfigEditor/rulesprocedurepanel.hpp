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
    class QuickRulesList : public QWidget{
        using QuickRulesRef = Tcl2CaplControllerConfig::RawRuleRefs&;
        using QuickRuleRef = Tcl2CaplControllerConfig::RawRuleRef;
        using QuickRule = Tcl2CaplControllerConfig::QuickRule;

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
                class ItemContent : public QWidget{

                    class ExpectedArgumentsList : public QTreeWidget{
                    private:
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
                                    setText(0, "<Brak argumentu>");
                                    setToolTip(0, "Brak argumentu");
                                    break;
                                default:
                                    setFlags(flags() | Qt::ItemIsEditable);
                                    break;
                                }
                                if(not value.isEmpty())
                                {
                                    setText(0, value);
                                    setToolTip(0, toolTipText());
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
                            QTreeWidget::editItem(item);
                            checkData();
                        }
                    public:
                        ExpectedArgumentsList();
                        void addIndex();
                        void addEditItem(ListItem*);
                        void loadExpectedArguments(QuickRule& rule);
                        void clearEditItem();
                    protected:

                        inline ListItem* currentItem()const{return static_cast<ListItem*>(QTreeWidget::currentItem());}
                        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(QTreeWidget::itemAt(p));}
                        inline ListItem* topLevelItem(int index)const{return static_cast<ListItem*>(QTreeWidget::topLevelItem(index));}
                        inline ListItem* itemFromIndex(const QModelIndex& index)const{return static_cast<ListItem*>(QTreeWidget::itemFromIndex(index));}
                        bool eventFilter(QObject* oj, QEvent* ev)override;
                        //void mouseReleaseEvent(QMouseEvent* ev)override;
                        void contextMenuEvent(QContextMenuEvent* ev)override;
                    };

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
                                    //qDebug() << itemContent->parentWidget();
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
                        void loadOutputs(QuickRule&);

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

                    ListItem& item_;
                    QVBoxLayout centralLayout;
                    QPushButton closeButton;
                    QFormLayout settingsLayout;
                    QLineEdit numbOfArgumentCondition;
                    RuleControlComboBox ruleControlComboBox;
                    QSplitter quickRuleInput;
                    ExpectedArgumentsList expectedArgumentsList;
                    OutputsList outputsList;

                public:
                    ItemContent(ListItem& item, QuickRuleRef rule = nullptr);
                    ItemContent(ItemContent* item);
                    ~ItemContent(){}

                    bool eventFilter(QObject* obj, QEvent* ev)override;
                    inline ListItem& item()const{return item_;}

                };

            public:
                ListItem() = delete;
                ListItem(RulesList& list, QuickRuleRef rule = nullptr){
                    list.addItem(this);
                    itemContent = new ItemContent(*this, rule);
                    listWidget().setItemWidget(this, itemContent);
                    setSizeHint(itemContent->sizeHint());
                }
                ListItem(const ListItem& item) : ListItem(item.listWidget()){
                    itemContent = new ItemContent(item.widget());
                }
                ~ListItem(){}

                ItemContent* itemContent = nullptr;

            public:
                inline RulesList &listWidget() const { return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }

                inline ItemContent* widget()const{return static_cast<ItemContent*>(itemContent);}
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
        using Request_ContextMenu_Func = void (QuickRulesList::*)(ListItem*);
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
        QuickRulesList();
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
    QuickRulesList quickRulesList;


public:
    void loadProcedure(ProcedureRef procedureRef);
};

#endif // RULESPROCEDUREPANEL_HPP
