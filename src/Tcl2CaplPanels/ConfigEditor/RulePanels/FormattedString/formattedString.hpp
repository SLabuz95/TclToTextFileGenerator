#ifndef GENERAL_VIEW_FORMATTEDSTRING_HPP
#define GENERAL_VIEW_FORMATTEDSTRING_HPP

#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include<QListWidgetItem>
#include<QFormLayout>
#include<QComboBox>
#include<QPushButton>
#include<QMouseEvent>
#include"Tcl2Capl/Config/Parameters/Formatted/FCT_products.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"

using FormatRules = FormatParametersFactory::ListOfBases;

namespace Panels::Configuration::View::FormattedString{

    class ListItem;
    class List;
    class ItemView;

    class ItemDataView : public QFormLayout{
    protected:
        using Parent = ListItem;
        using Self = ItemDataView;
        using Super = QWidget;
        using DataRef = const FormatRules::Type ;
        using DataViewType =  std::remove_pointer_t<DataRef>::ProductsList;

    public:
        using ItemView = ItemView;
        using CreateFunction =  ItemDataView* (*)(ItemView&, DataRef);
        using CreateFunctionTable = CreateFunction[];
       // using OutputContent_CreateContent_Func = void (Self::*)();
        inline constexpr static std::underlying_type_t<DataViewType> toULying(DataViewType type){
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

        }

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
        static ItemDataView* createNoDataView(ItemView& view, DataRef = nullptr){return nullptr;}
    public:
        static ItemDataView* createView(ItemView& view, DataRef = nullptr);

    public:
        ItemView& parentWidget()const;
        ItemDataView() : QFormLayout(){}

    };


    class ItemView : public QWidget
    {
    public:
        ItemView(ListItem&);
    protected:
        using Conditionals = ConditionalsFactory::ListOfBases;
        using Super = QWidget;
        // GUI Elements
        // List of Indexes definition
    public:

    protected:
        // GUI Layout
        using MainLayout = QFormLayout;
        using TitleLayout = QHBoxLayout;
        using ComboBox = QComboBox;
        using RemoveButton = QPushButton;

        MainLayout mainLayout;
        TitleLayout titleLayout;
        ComboBox titleComboBox;
        RemoveButton titleRemoveButton;
        ItemDataView* dataView_ = nullptr;


    public:
       // Action toAction()override{}
        List& parentWidget()const;

    };
    class ListItem : public QListWidgetItem{
        using Super = QListWidgetItem;
        using Parent = List;
        using Self = ListItem;


    public:
        ListItem() = delete;
        ListItem(List& list);
        ListItem(const ListItem& item)
            : ListItem(item.list())
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
        List &list() const; //{ return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }
        //inline Parent* listWidget()const;//{return static_cast<Parent*>(Super::listWidget());}
        inline ItemView& view(){return view_;}

    };
    class List : public QListWidget{
    public:
    protected:
        using Self = List;
        using Super = QListWidget;

        using ListItem = ListItem;
        using Request_ContextMenu_Func = void (Self::*)(ListItem*);

        enum class Request_ContextMenu{
            Add,
            Clone,
            Remove,
            Clear,
            Size
        };

        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);

        using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
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
        }
    protected:
        ListItem* lastPressedItem = nullptr;

        inline ListItem* currentItem()const{return static_cast<ListItem*>(Super::currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(Super::itemAt(p));}
        inline ListItem* item(int index)const{return static_cast<ListItem*>(QListWidget::item(index));}
        inline ListItem* lastItem()const{return item(count() - 1);}

        // bool eventFilter(QObject* oj, QEvent* ev)override;
        //void mouseReleaseEvent(QMouseEvent* ev) override;
        void contextMenuEvent(QContextMenuEvent* ev)override;
        //void addNewOutput();/*{
            //addItem(new ListItem(this));
        //}
        //void loadOutputs(RawRule&);
        inline void addNewItem(){
            //ListItem* newItem = nullptr;
            new ListItem(*this);
            //newItem->init();
        }
/*
        inline void addNewItem(ActionPtr action){
            //ListItem* newItem = nullptr;
            new ListItem(*this, action);
            //newItem->init();
        }*/
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
         void extendContextMenu(ContextMenuConfig&);
         void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*);

    };

}

#endif // GENERAL_VIEW_FORMATTEDSTRING_HPP
