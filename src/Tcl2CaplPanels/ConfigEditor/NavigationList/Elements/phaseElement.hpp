#ifndef PHASEELEMENT_HPP
#define PHASEELEMENT_HPP

#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"

namespace Panels::Configuration::Navigation::Phase{

using NavigationElement = List::NavigationElement;

class PhaseElement : public NavigationElement{
public:
    class ListItem : public NavigationElement{
    public:
        ListItem(Qt::ItemFlags flags, PhaseElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : NavigationElement(parent, strings, type)
        {
            setFlags(flags);
        }
        ListItem(PhaseElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : NavigationElement(parent, strings, type)
        {
            setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
        }
    };

    inline PhaseElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
        : NavigationElement(parent, strings, type)
    {
        const Qt::ItemFlags flagsForNonIndexCategories // Predefined categories
                = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

        addChild(new ListItem(flagsForNonIndexCategories, this, {"Reguły"}));
        addChild(new ListItem(flagsForNonIndexCategories, this, {"Koniec reguł"}));
        addChild(new ListItem(flagsForNonIndexCategories, this, {"Brak spełenienia reguł"}));
        setExpanded(true);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    }
    inline PhaseElement(QTreeWidgetItem *parent, const QStringList &strings, int type = Type)
        : NavigationElement(parent, strings, type)
    {
        const Qt::ItemFlags flagsForNonIndexCategories // Predefined categories
                = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

        addChild(new ListItem(flagsForNonIndexCategories, this, {"Reguły"}));
        addChild(new ListItem(flagsForNonIndexCategories, this, {"Koniec procedury"}));
        addChild(new ListItem(flagsForNonIndexCategories, this, {"Domyślny argument"}));
        setExpanded(true);
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

    }

    using Request_ContextMenu_Func = void (PhaseElement::*)(ListItem*);
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
    // Procedure Rules Control


};

class PhasesElement : public NavigationElement{
public:
    class ListItem : public PhaseElement{

    public:
        using ChildType = PhaseElement::ListItem;
        ListItem(Qt::ItemFlags flags, PhasesElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : PhaseElement(parent, strings, type)
        {
        }
        ListItem(PhasesElement *parent = nullptr, const QStringList &strings = QStringList(), int type = Type)
            : PhaseElement(parent, strings, type)
        {
        }
        void menuControl(QContextMenuEvent* cev, ListItem* item);
    };

    inline PhasesElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
        : NavigationElement(parent, strings, type)
    {

        addChild(new ListItem(this, {"Default"}));
        setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    using Request_ContextMenu_Func = void (PhasesElement::*)(ListItem*);
    enum class Request_ContextMenu{
        AddPhase,
        EditPhase,
        RemovePhase,
        ClearPhases,
        Size
    };
    template<Request_ContextMenu>
    void execRequest_ContextMenu(ListItem*);
    // Names Control
    void menuControl(QContextMenuEvent* cev, ListItem* item);
    void edittingFinished();

    //bool eventFilter(QObject* obj, QEvent* ev)override;
    // Procedure Rules Control


};





}

#endif // PHASEELEMENT_HPP
