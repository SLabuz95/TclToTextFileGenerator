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
#include"RulePanels/rawRule.hpp"
#include"External/ContextMenuBuilder/contextMenuBuilder.hpp"
#include<QPainter>
#include"Tcl2Capl/controllerconfiginfo.hpp"
#include<QScrollBar>

namespace Panels::Configuration::View::Rules::RulesProcedurePanel{
    class RulesList;

    using ContextMenuConfig = Utils::ContextMenuBuilder::Configuration;
    template<class Base>
    using ContextMenuInterface = Utils::ContextMenuBuilder::Interface<Base>;

    class ListItem : public QListWidgetItem{
        using RulesRef = ControllerConfigInfo::NewRules;
        using Rule = std::remove_pointer_t<RulesRef::Type>;
        using RuleRef = Rule&;
        using RawRuleView = Rules::RawRuleView;
        using RulesViewRef = ControllerConfigInfo::RulesView;
        using RuleViewRef = RulesViewRef::first_type;
    public:
        using List = RulesList;
        ListItem() = delete;
        ListItem(RulesList& list, RuleViewRef rule);
        ListItem(const ListItem& item) : ListItem(item.rulesList(), RuleViewRef(nullptr)){
            //itemContent = new ItemContent(item.widget());
        }
        ~ListItem()override{

        }

        RawRuleView rawRuleView_;

    public:
        inline void readRule(RuleRef rule){rawRuleView_.readRule(rule);}
        RulesList &rulesList() const; //{ return *static_cast<RulesList*>(QListWidgetItem::listWidget()); }
        inline RawRuleView& rawRuleView(){return rawRuleView_;}

    };
    class RulesList : public ContextMenuInterface<QListWidget> {
        using RulesRef = ControllerConfigInfo::NewRules;
        using RulesViewRef = ControllerConfigInfo::RulesView;
        using RuleViewRef = RulesViewRef::first_type;
        using Super = ContextMenuInterface<QListWidget>;
    public:
        using Request_ContextMenu_Func = void (RulesList::*)(ListItem*);
        enum class Request_ContextMenu{
            Add,
            Clone,
            Remove,
            Clear,
            Size
        };
        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);

        RulesList();
        ~RulesList()override{
            for(int i = 0; i < count(); i++)
                itemWidget(item(i))->setParent(nullptr);
        }

    protected:
        ListItem* lastPressedItem = nullptr;
        QString procedureName_;
        ControllerConfigInfo::RulesCategories rulesCategory_ = static_cast<ControllerConfigInfo::RulesCategories>(LONG_LONG_MIN);
    public:
        inline QString& procedureName(){return procedureName_;}
        inline ControllerConfigInfo::RulesCategories& rulesCategory(){return rulesCategory_;}

        inline ListItem* currentItem()const{return static_cast<ListItem*>(QListWidget::currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(QListWidget::itemAt(p));}

        inline ListItem* item(int index)const{return static_cast<ListItem*>(QListWidget::item(index));}
        inline ListItem* lastItem()const{return item(count() - 1);}

        inline bool initialized()const{return rulesCategory_ != static_cast<ControllerConfigInfo::RulesCategories>(LONG_LONG_MIN);}
        inline void setNonInitialized(){rulesCategory_ = static_cast<ControllerConfigInfo::RulesCategories>(LONG_LONG_MIN);}
        inline int countRules()const{return count();}

        void loadRules(RulesViewRef);
        void readRules(RulesRef&);

        inline void addNewItem(){
            //ListItem* newItem = nullptr;
            setUpdatesEnabled(false);
            new ListItem(*this, RuleViewRef( nullptr));
            setUpdatesEnabled(true);
            //newItem->init();
        }

        inline void addNewItem(RuleViewRef rule){
            //ListItem* newItem = nullptr;
            new ListItem(*this, rule);
            //newItem->init();
        }


       void contextMenuEvent(QContextMenuEvent *e) override;

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

        QSize minimumSizeHint() const override{
            return  QSize(0, 0);
        }

        QSize sizeHint() const override{
            const int fW = frameWidth();
            QSize&& s = Super::viewportSizeHint()+QSize(0, fW*2);
            return (Super::count() > 0)? s: QSize(0, 0);
        }


        void extendContextMenu(ContextMenuConfig&)const override;
        void interpretContextMenuResponse(ContextMenuConfig::ActionIndex, QContextMenuEvent*)override;
    };

    class RulesPanel : public QWidget{
        using RulesRef = ControllerConfigInfo::NewRules;
        using RulesViewRef = ControllerConfigInfo::RulesView;
        using RuleViewRef = RulesViewRef::first_type;
    protected:
        QVBoxLayout mainLayout;
        RulesList rulesList;
        QPushButton addRuleButton;
    public:
        RulesPanel(){
            addRuleButton.setText("Dodaj nowa regułę");

            mainLayout.setSpacing(0);
            mainLayout.setContentsMargins(0,0,0,0);
            mainLayout.addWidget(&rulesList, 0, Qt::AlignTop);
            mainLayout.addWidget(&addRuleButton, 1, Qt::AlignTop);
            addRuleButton.installEventFilter(this);
            setLayout(&mainLayout);
        }
        inline QString& procedureName(){return rulesList.procedureName();}
        inline ControllerConfigInfo::RulesCategories& rulesCategory(){return rulesList.rulesCategory();}
        inline void clear(){rulesList.clear();}

        inline bool initialized()const{return rulesList.initialized();}
        inline void setNonInitialized(){rulesList.setNonInitialized();}

        inline void loadRules(RulesViewRef rulesView){rulesList.loadRules(rulesView);}
        inline void readRules(RulesRef& rules){rulesList.readRules(rules);}

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

    using Panel = RulesPanel;


}

#endif // RULESPROCEDUREPANEL_HPP
