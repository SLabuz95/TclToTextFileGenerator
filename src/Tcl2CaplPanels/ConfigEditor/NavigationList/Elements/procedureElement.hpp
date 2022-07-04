#ifndef PROCEDUREELEMENT_HPP
#define PROCEDUREELEMENT_HPP

#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"

namespace Panels::Configuration::Navigation::Procedure{

class DefaultProcedureElement : public NavigationElement{
public:
    class ListItem : public QTreeWidgetItem{
    public:
        ListItem(Qt::ItemFlags flags, DefaultProcedureElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : QTreeWidgetItem(parent, strings, type)
        {
            setFlags(flags);
        }
        ListItem(DefaultProcedureElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : QTreeWidgetItem(parent, strings, type)
        {
            setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
        }
    };
    using NewIndexes = QStringList;
    struct CurEditItemInfo{
        ListItem* item = nullptr;
        QString oldStr;
    } curEditItemInfo;
    NewIndexes newIndexes;

    inline DefaultProcedureElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
        : NavigationElement(parent, strings, type)
    {
        const Qt::ItemFlags flagsForNonIndexCategories // Predefined categories
                = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

        addChild(new ListItem(flagsForNonIndexCategories, this, {"Koniec procedury"}));
        addChild(new ListItem(flagsForNonIndexCategories, this, {"Domyślny argument"}));
        setExpanded(true);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    }
    inline DefaultProcedureElement(QTreeWidgetItem *parent, const QStringList &strings, int type = Type)
        : NavigationElement(parent, strings, type)
    {
        const Qt::ItemFlags flagsForNonIndexCategories // Predefined categories
                = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

        addChild(new ListItem(flagsForNonIndexCategories, this, {"Koniec procedury"}));
        addChild(new ListItem(flagsForNonIndexCategories, this, {"Domyślny argument"}));
        setExpanded(true);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

    }

    using Request_ContextMenu_Func = void (DefaultProcedureElement::*)(ListItem*);
    enum class Request_ContextMenu{
        AddIndex,
        EditIndex,
        RemoveIndex,
        ClearIndexes,
        Size
    };
    template<Request_ContextMenu>
    void execRequest_ContextMenu(ListItem*);
    // Names Control
    void menuControl(QContextMenuEvent* cev, ListItem* item);
    void edittingFinished();

    //bool eventFilter(QObject* obj, QEvent* ev)override;
    bool tryToManageIndex(QString oldIndex, QString newIndex);
    // Procedure Rules Control


};

class ProceduresElement : public NavigationElement{
public:
    class ListItem : public DefaultProcedureElement{
    public:
        ListItem(Qt::ItemFlags flags, ProceduresElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : DefaultProcedureElement(parent, strings, type)
        {
        }
        ListItem(ProceduresElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : DefaultProcedureElement(parent, strings, type)
        {
        }
    };
    using NewIndexes = QStringList;
    struct CurEditItemInfo{
        ListItem* item = nullptr;
        QString oldStr;
    } curEditItemInfo;
    NewIndexes newIndexes;

    inline ProceduresElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
        : NavigationElement(parent, strings, type)
    {
        const Qt::ItemFlags flagsForNonIndexCategories // Predefined categories
                = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

        setExpanded(true);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    using Request_ContextMenu_Func = void (ProceduresElement::*)(ListItem*);
    enum class Request_ContextMenu{
        AddProcedure,
        EditProcedure,
        RemoveProcedure,
        ClearProcedures,
        Size
    };
    template<Request_ContextMenu>
    void execRequest_ContextMenu(ListItem*);
    // Names Control
    void menuControl(QContextMenuEvent* cev, ListItem* item);
    void edittingFinished();

    //bool eventFilter(QObject* obj, QEvent* ev)override;
    bool tryToManageIndex(QString oldIndex, QString newIndex);
    // Procedure Rules Control


};





}

#endif // PROCEDUREELEMENT_HPP
