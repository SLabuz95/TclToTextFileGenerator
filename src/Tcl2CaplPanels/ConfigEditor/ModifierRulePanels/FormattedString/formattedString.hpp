#ifndef GENERAL_VIEW_MODIFIER_FORMATTEDSTRING_HPP
#define GENERAL_VIEW_MODIFIER_FORMATTEDSTRING_HPP

#include<QListWidgetItem>
#include<QFormLayout>
#include<QComboBox>
#include<QPushButton>
#include<QMouseEvent>
#include"TcFileModifier/Config/Parameters/Formatted/FCT_products.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include<QApplication>

using ModifierFormatRules = ModifierFormatParametersFactory::ListOfBases;

namespace Panels::Configuration::View::ModifierFormattedString{

    class ListItem;
    class List;
    class ItemView;

    using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
    using ContextMenuInterface = Utils::ContextMenuBuilder::InterfaceExtended<QListWidget>;
    using ParentContextMenu = ContextMenuInterface::Interface;

    class ItemDataView : public QFormLayout{

    public:
        using FormatRulePtr = typename ModifierFormatRules::Type;
        using FormatRuleBase = std::remove_pointer_t<FormatRulePtr>;
        using FormatRuleRef = FormatRulePtr ;
        using FormatRuleType = typename FormatRuleBase::ProductsList;
        using FormatRuleView = ItemView;
        using Error = QString;

        /*using Parent = ListItem;
        using Self = ItemDataView;
        using Super = QWidget;
        using DataRef = const FormatRules::Type ;
        using ActionType = typename Action::ProductsList;
        using DataViewType =  std::remove_pointer_t<DataRef>::ProductsList;*/
        using CreateFunction =  ItemDataView* (*)(QWidget* , FormatRuleRef);
        using CreateFunctionTable = CreateFunction[];

    public:
        //using ItemView = ItemView;
        // using OutputContent_CreateContent_Func = void (Self::*)();
        /*inline constexpr static std::underlying_type_t<DataViewType> toULying(DataViewType type){
            return static_cast<std::underlying_type_t<DataViewType>>(type);
        }
        inline constexpr static DataViewType fromULying(std::underlying_type_t<DataViewType> type){
            return static_cast<DataViewType>(type);
        }
        inline constexpr static std::underlying_type_t<DataViewType> numbOfItemTypes(){
            return static_cast<std::underlying_type_t<DataViewType>>(DataViewType::Size);
        }

        template<DataViewType itemToCheck, DataViewType itemTypeCreatingLayout = DataViewType::Size, DataViewType ...itemTypesCreatingLayout>
        inline constexpr static bool isOutputContentLayoutCreatedForSpecifiedType(){
            return (itemTypeCreatingLayout == DataViewType::Size)? false : ((itemToCheck == itemTypeCreatingLayout)
                    || Self::isOutputContentLayoutCreatedForSpecifiedType<itemToCheck, itemTypesCreatingLayout...>());

        }*/

        //static const OutputContent_CreateContent_Func createOutputContentFunctions[];
        //template<DataViewType type>
        //void createOutputContentLayout();
        //template<DataViewType>
        //class OutputContent;

        //void createOutputContentLayout_priv(DataViewType type);{
        /*   if(type != curType){
                if(outputContentLayout){
                    mainLayout.removeRow(mainLayout.rowCount() - 1);
                    outputContentLayout = nullptr;
                }
                curType = type;
                (this->*createOutputContentFunctions[Self::toULying(curType)])();
                qApp->processEvents();
                parent->setSizeHint(sizeHint());
            }
        }*/
        static CreateFunctionTable createFunctionTable;
        static ItemDataView* createNoDataView(QWidget* , FormatRuleRef = nullptr){return nullptr;}
    public:
        static ItemDataView* createView(QWidget* parent, FormatRuleType);
        static ItemDataView* createView(QWidget* parent, FormatRuleRef);

    public:
        //ItemView& parentWidget()const;
        ItemDataView(QWidget* parent) : QFormLayout(parent){}
        ~ItemDataView()override{}

        virtual constexpr FormatRuleType type()const = 0;
        virtual void readRule(FormatRuleBase&) = 0;
    };


    class ItemView : public QWidget
    {
        using FormatRulePtr = typename ModifierFormatRules::Type;
        using FormatRule = std::remove_pointer_t<FormatRulePtr>;
        using FormatRuleType = typename FormatRule::ProductsList;
        using Super = QWidget;
    public:
        ItemView(List&, FormatRulePtr);
        ~ItemView()override{

        }
    protected:
        //using Conditionals = ConditionalsFactory::ListOfBases;
        // GUI Elements
        // List of Indexes definition

    protected:
        // GUI Layout
        using MainLayout = QFormLayout;
        using TitleLayout = QHBoxLayout;
        class ComboBox : public QComboBox{
            void wheelEvent(QWheelEvent *) final override{
                // Pass to parent
            }
        public:
            using QComboBox::QComboBox;
        };

        using RemoveButton = QPushButton;
        //FormatRulePtr formatRule = nullptr;

        MainLayout mainLayout;
        //TitleLayout titleLayout;
        ComboBox titleComboBox;
        //RemoveButton titleRemoveButton;
        ItemDataView* dataView_ = nullptr;


        bool eventFilter(QObject* obj, QEvent* ev)override;
        bool createFormatRuleDataView(FormatRuleType);
    public:
       // Action toAction()override{}
        void readRule(FormatRulePtr& rule);
        List& parentWidget()const;

    };
    class ListItem : public QListWidgetItem{
        using FormatRulePtr = typename ModifierFormatRules::Type;
        using FormatRule = std::remove_pointer_t<FormatRulePtr>;
        using Super = QListWidgetItem;
        using Parent = List;
        using Self = ListItem;


    public:
        ListItem() = delete;
        ListItem(List& list, FormatRulePtr rule);
        ListItem(const ListItem& item)
            : ListItem(item.list(), nullptr)
        {
        }
        ~ListItem()override{
        }
    protected:
        ItemView view_;

        void init();/*{
            if(listWidget()){
                listWidget()->setItemWidget(this, itemContent);
                setSizeHint(itemContent->sizeHint());
            }
        }*/

        //ItemDataView* widget(){return static_cast<ItemDataView*>(itemContent);}
    public:
        //inline DataViewType type()const{return type_;}
        inline void readRule(FormatRulePtr& rule){view_.readRule(rule);}
        List &list() const; //{ return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }
        //inline Parent* listWidget()const;//{return static_cast<Parent*>(Super::listWidget());}
        inline ItemView& view(){return view_;}

    };
    class List : public ContextMenuInterface{
        using FormatRulesRef = ModifierFormatRules&;
        using FormatRulePtr = typename ModifierFormatRules::Type;
    protected:
        using Self = List;

        using ListItem = ListItem;
        using Request_ContextMenu_Func = void (Self::*)(ListItem*);
        using Super = ContextMenuInterface;

        enum class Request_ContextMenu{
            Add,
            Clone,
            Remove,
            Clear,
            Size
        };

        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);

        //using OutputOption2FormatRuleMap = const QMap<const QString, FormatParametersType>;
        /*using OutputOption2FormatRuleMap = const QStringList;
        OutputOption2FormatRuleMap outputOption2FormatRuleMap =
        {
            "Tekst",
            "Nazwa procedury",
            "Argument o indeksie",
            "Wszystkie argumenty od wybranego indeksu",
            //"Separator",
            "Format"
        };*/

    public:
        List();
        ~List()override{
            for(int i = 0; i < count(); i++)
                itemWidget(item(i))->setParent(nullptr);
            // Dynamic objects - Remove as child - do not set null parent (In RawRules List, static objects are used)
        }
   protected:
        ListItem* lastPressedItem = nullptr;

        inline ListItem* currentItem()const{return static_cast<ListItem*>(Super::currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(Super::itemAt(p));}
        inline ListItem* item(int index)const{return static_cast<ListItem*>(QListWidget::item(index));}
        inline ListItem* lastItem()const{return item(count() - 1);}

        // bool eventFilter(QObject* oj, QEvent* ev)override;
        //void mouseReleaseEvent(QMouseEvent* ev) override;
        //void addNewOutput();/*{
            //addItem(new ListItem(this));
        //}
    public:
       inline void addNewItem(){
            //ListItem* newItem = nullptr;
            new ListItem(*this, nullptr);


            //newItem->init();
        }
protected:
        inline void addNewItem(FormatRulePtr rule){
            //ListItem* newItem = nullptr;
            new ListItem(*this, rule);
            //newItem->init();
        }

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
             }

             Super::dropEvent(ev);
         }

        bool viewportEvent(QEvent* ev)override{
            switch(ev->type()){
            case QEvent::LayoutRequest:
            {
                if(count() > 0){
                    QListWidget& listWidget = actionListView();
                    QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
                    if(item and actionView().sizeHint().height() < sizeHint().height()){
                        item->setSizeHint(actionView().sizeHint() += QSize(0, sizeHint().height() - height()));
                    }
                }
            }
                break;
            case QEvent::Resize:
            {
                QResizeEvent& rsEv = *static_cast<QResizeEvent*>(ev);
                 if(rsEv.oldSize().height() != 0 and rsEv.size().height() < rsEv.oldSize().height()){
                    QListWidget& listWidget = actionListView();
                    QListWidgetItem* item = listWidget.itemAt(listWidget.viewport()->mapFromGlobal(mapToGlobal(QPoint(0,0))));
                    if(item){
                        item->setSizeHint(actionView().sizeHint() -= QSize(0, rsEv.oldSize().height() - rsEv.size().height()));
                    }
                }
            }
                break;
            default:
                break;
            }

            return QListWidget::viewportEvent(ev);
        }
    public:

        QSize minimumSizeHint() const override{
            return  QSize(0, 0);
        }

        QSize sizeHint() const override{
            const int fW = frameWidth();
            QSize&& s = Super::viewportSizeHint()+QSize(0, fW*2 + 40);
            return (count() > 0)? s: QSize(0, 40);
        }

         void loadRules(FormatRulesRef);
         void readRules(FormatRulesRef);
         void contextMenuEvent(QContextMenuEvent* ev)override;
         ParentContextMenu& parentContextMenu()const override;
         void extendContextMenu(ContextMenuConfig&)const override;
         void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*)override;
         QWidget& actionView()const; // Splitter -> Widget (Widget with Layout of ActionDataView) -> ActionView (Any - Conditional or Executable)
         QListWidget& actionListView()const; // ActionView -> Viewport -> List
    };

    class Panel : public QWidget{
        using FormatRulesRef = ModifierFormatRules&;
        using FormatRulePtr = typename ModifierFormatRules::Type;
    protected:
        QVBoxLayout mainLayout;
        List rulesList;
        QPushButton addRuleButton;
    public:
        Panel(){
            addRuleButton.setText("Dodaj regułę formatującą");

            mainLayout.setSpacing(0);
            mainLayout.setContentsMargins(0,0,0,0);
            mainLayout.addWidget(&rulesList, 0, Qt::AlignTop);
            mainLayout.addWidget(&addRuleButton, 1, Qt::AlignTop);
            addRuleButton.installEventFilter(this);
            setLayout(&mainLayout);
        }
        inline void clear(){rulesList.clear();}

        inline void loadRules(FormatRulesRef rulesView){rulesList.loadRules(rulesView);}
        inline void readRules(FormatRulesRef rules){rulesList.readRules(rules);}

        bool eventFilter(QObject* obj, QEvent* ev)override{
            switch(ev->type()){
            case QEvent::MouseButtonPress:
            {
                if(obj == &addRuleButton){
                    rulesList.addNewItem();
                }
            }
                break;
            default:
                break;
            }
            return QWidget::eventFilter(obj, ev);
        }
    };



}

#endif // GENERAL_VIEW_MODIFIER_FORMATTEDSTRING_HPP
