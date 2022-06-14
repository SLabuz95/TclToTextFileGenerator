#ifndef INSTANCELIST_HPP
#define INSTANCELIST_HPP

#include<QTreeWidget>
#include"External/Utils/class.hpp"

class MainWindow;
class QMdiSubWindow;

class InstanceListElementConfig : public QTreeWidgetItem{
public:
    inline InstanceListElementConfig(QString name = QString()) : QTreeWidgetItem({name}){
         setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
    }
protected:

};

class InstanceListElement : public QTreeWidgetItem{
public:
    inline InstanceListElement(QString name = QString()) : QTreeWidgetItem({name}){
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        addChild(new InstanceListElementConfig("Konfiguracja"));
    }
};

class InstanceList : public QTreeWidget{

public:
    /*static const QString navigationPanelNames[panelType2number(PanelType::Size)];
    using NavigationElementChildFctPtr = void (List::*)(NavigationElement* const);*/
    struct SubWindowPositionInfo{
        int indexOfInstance;
        int indexOfInterpreter;
    };
private:
    using Super = QTreeWidget;
    using Request_ContextMenu_Func = void (InstanceList::*)(InstanceListElement*);
    enum class Request_ContextMenu{
        AddInstance,
        EditInstance,
        RemoveInstance,
        AddFilesInterpreter,
        AddTextInterpreter,
        EditInterpreterName,
        RemoveInterpreter,
        Size
    };
    template<Request_ContextMenu>
    void execRequest_ContextMenu(InstanceListElement*);

    struct CurEditItemInfo{
        InstanceListElement* item = nullptr;
        QString oldStr;
        QWidget* interpreterWindow = nullptr;
    } curEditItemInfo;



    //static NavigationElementChildFctPtr navigationElementChildFcts[];

    /*void navigateMainPanel(const PanelType);
    template<PanelType>
    void navigationElementChildFct(NavigationElement* const);

    inline void callNavigationElementChildFct(PanelType const type, NavigationElement* const element){
        (this->*navigationElementChildFcts[panelType2number(type)])(element);
    }*/

    inline int indexOf(InstanceListElement* child)const{
        return invisibleRootItem()->indexOfChild(child);
    }
    inline InstanceListElement *itemAt(const QPoint &p) const{
        return static_cast<InstanceListElement*>(QTreeWidget::itemAt(p));
    }

    SubWindowPositionInfo getSubWindowPositionInfo(InstanceListElement* item){
        return SubWindowPositionInfo{
            item->treeWidget()->indexOfTopLevelItem(item->parent()),
            item->parent()->indexOfChild(item)};
    }

public:
    InstanceList(MainWindow& parent);
    virtual ~InstanceList() override{}

    MainWindow& mainApp;


    WIDGET_ADD_EVENT_FILTER

};


#endif // INSTANCELIST_HPP
