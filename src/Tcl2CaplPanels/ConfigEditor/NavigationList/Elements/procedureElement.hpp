#ifndef PROCEDUREELEMENT_HPP
#define PROCEDUREELEMENT_HPP

#include"Tcl2CaplPanels/ConfigEditor/NavigationList/navigationList.hpp"

namespace Panels::Configuration::Navigation::Procedure{


    class ProcedureElement : public QTreeWidgetItem{
    public:
        inline ProcedureElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
            : QTreeWidgetItem(parent, strings, type)
        {
            addChild(new QTreeWidgetItem(this, {"Koniec procedury"}));
            addChild(new QTreeWidgetItem(this, {"Domyślny argument"}));


        }
        // Names Control

        // Procedure Rules Control

    };


    class DefaultProcedureElement : public NavigationElement{
    public:
        using ListItem = QTreeWidgetItem;
        using NewIndexes = QStringList;
        struct CurEditItemInfo{
            ListItem* item = nullptr;
            QString oldStr;
        } curEditItemInfo;
        NewIndexes newIndexes;

        inline DefaultProcedureElement(QTreeWidget *parent, const QStringList &strings, int type = Type)
            : NavigationElement(parent, strings, type)
        {
            addChild(new QTreeWidgetItem(this, {"Koniec procedury"}));
            addChild(new QTreeWidgetItem(this, {"Domyślny argument"}));
            setExpanded(true);

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

        //bool eventFilter(QObject* obj, QEvent* ev)override;
        bool tryToManageIndex(QString oldIndex, QString newIndex);
        // Procedure Rules Control

    };


}

#endif // PROCEDUREELEMENT_HPP
