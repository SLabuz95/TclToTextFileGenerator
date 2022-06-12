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


}

#endif // PROCEDUREELEMENT_HPP
