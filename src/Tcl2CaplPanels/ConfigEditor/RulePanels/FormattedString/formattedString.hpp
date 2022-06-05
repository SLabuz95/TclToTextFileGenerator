#ifndef FORMATTEDSTRING_HPP
#define FORMATTEDSTRING_HPP

#include"Tcl2Capl/Config/Actions/Conditional/conditionals.hpp"
#include<QListWidgetItem>
#include<QFormLayout>
#include<QComboBox>
#include<QPushButton>
#include<QMouseEvent>

namespace Panels::Configuration::View::General::FormattedString{

    class ListItem;
    class List;

    class ItemDataView : public QLayout{
        using Parent = ListItem;
        using Self = ItemDataView;
        using Super = QWidget;
        enum class ItemType{
            PlainTextItem,
            NameItem,
            IndexItem,
            ArgumentsFromItem,
           // SeparatorItem,
            FormatItem,
            Size
        };
    public:
        using OutputContent_CreateContent_Func = void (Self::*)();
        inline constexpr static std::underlying_type_t<ItemType> toULying(ItemType type){
            return static_cast<std::underlying_type_t<ItemType>>(type);
        }
        inline constexpr static ItemType fromULying(std::underlying_type_t<ItemType> type){
            return static_cast<ItemType>(type);
        }
        inline constexpr static std::underlying_type_t<ItemType> numbOfItemTypes(){
            return static_cast<std::underlying_type_t<ItemType>>(ItemType::Size);
        }

        template<ItemType itemToCheck, ItemType itemTypeCreatingLayout = ItemType::Size, ItemType ...itemTypesCreatingLayout>
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

        ItemType curType = ItemType::Size;

        inline void createOutputContentLayout_priv(ItemType type){
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

        template<ItemType type>
        void createOutputContentLayout();

        static const OutputContent_CreateContent_Func createOutputContentFunctions[];

        template<ItemType>
        class OutputContent;

        bool eventFilter(QObject* obj, QEvent* ev) override;

    public:
        ItemDataView(Parent*);
        Parent* parent = nullptr;

    };


    class ItemView
    {
        ItemView(ActionView&);
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

    protected:
        // GUI Layout

    public:
       // Action toAction()override{}
        constexpr ActionType type()const override{return ConditionalsTypes::CompareNumbOfArguments;}

    };
    class ListItem : public QListWidgetItem{
        using Super = QListWidgetItem;
        using Parent = List;
        using Self = ListItem;


    public:
        ListItem(Parent* parent) : Super(parent), itemContent(new ItemDataView(this)){
            init();
        }

    protected:
        ItemDataView* itemContent = nullptr;

        inline void init(){
            if(listWidget()){
                listWidget()->setItemWidget(this, itemContent);
                setSizeHint(itemContent->sizeHint());
            }
        }

        inline ItemDataView* widget(){return static_cast<ItemDataView*>(itemContent);}
    public:
        //inline ItemType type()const{return type_;}
        inline Parent* listWidget()const{return static_cast<Parent*>(Super::listWidget());}

    };
    class List : public QListWidget{
    public:

    protected:
        using Self = List;
        using Super = QListWidget;


        using ListItem = ListItem;
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
        List();
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
        //void loadOutputs(RawRule&);

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

}

#endif // FORMATTEDSTRING_HPP
