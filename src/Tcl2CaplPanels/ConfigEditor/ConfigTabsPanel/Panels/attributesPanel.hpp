#ifndef ATTRIBUTESPANEL_HPP
#define ATTRIBUTESPANEL_HPP

#include<QTreeWidget>
#include<QTreeWidgetItem>
#include<QVBoxLayout>
#include"Tcl2Capl/controllerconfig.hpp"
#include"Tcl2CaplPanels/ConfigEditor/loadconfigsettings.hpp"

namespace Panels::Configuration{

    class ConfigTabsPanel;

    class AttributesPanel : public QTreeWidget{
    public:
        AttributesPanel(ConfigTabsPanel& tabsPanel);
        virtual ~AttributesPanel()override{}
    private:
        ConfigTabsPanel& tabsPanel;

        class ListItem : public QTreeWidgetItem{
        public:
            inline ListItem(QString name = QString(), QString value = QString()) : QTreeWidgetItem({name, value}){
                 setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);

            }
        };
        using Request_ContextMenu_Func = void (AttributesPanel::*)(ListItem*);
        enum class Request_ContextMenu{
            AddAttribute,
            EditAttribute,
            RemoveAttribute,
            ClearAttributes,
            Size
        };
        template<Request_ContextMenu>
        void execRequest_ContextMenu(ListItem*);
        using Config = Tcl2CaplControllerConfig;
        using AttributesPtr = Config::Attributes*;
        using LoadProceduresSettings = LoadConfigSettings;

    protected:
        using Layout = QVBoxLayout;
        using Super = QTreeWidget;
        using Attributes = Config::Attributes;
        using SavedAttributes = Attributes*;
        using NewAttributes = Attributes;
        using RemovedAttributes = Attributes;

        //Layout layout;
        //QListWidget list;

        struct CurEditItemInfo{
            ListItem* item = nullptr;
            QString oldStr;
            int column;
        } curEditItemInfo;


        SavedAttributes savedAttributes = nullptr;
        NewAttributes newAttributes;
        RemovedAttributes removedAttributes;

        inline ListItem* currentItem()const{return static_cast<ListItem*>(QTreeWidget::currentItem());}
        inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(QTreeWidget::itemAt(p));}

        bool eventFilter(QObject* obj, QEvent* ev)override;
        bool tryToManageAttributesName(QString oldName, QString newName);
        bool tryToManageAttributesValue(QString name, QString oldName, QString newName);
    public:                
        inline bool anyChanges()const{
            return not (newAttributes.isEmpty() and removedAttributes.isEmpty());
        }

        inline void restoreSavedData(){
            newAttributes.clear();
            removedAttributes.clear();
        }
        void commitChanges(){}

        void loadAttributes(AttributesPtr, LoadProceduresSettings);

        inline void clearChanges(){
            newAttributes.clear();
            removedAttributes.clear();
        }

        void reloadGui();
    };

}
#endif // ATTRIBUTESPANEL_HPP
