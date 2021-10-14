#ifndef PROCEDURESLIST_HPP
#define PROCEDURESLIST_HPP

#include<QListWidgetItem>
#include<QPushButton>
#include<QToolBar>
#include<QVBoxLayout>
#include"Tcl2Capl/controllerconfig.hpp"
#include"loadconfigsettings.hpp"

class ProceduresList : public QWidget{
    class ListItem : public QListWidgetItem{
    public:
        inline ListItem(QString str = QString()) : QListWidgetItem(str){
             setFlags(flags() | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);
        }
    };
    using Request_ContextMenu_Func = void (ProceduresList::*)(ListItem*);
    enum class Request_ContextMenu{
        AddProcedure,
        EditProcedure,
        RemoveProcedure,
        ClearProcedures,
        Size
    };
    template<Request_ContextMenu>
    void execRequest_ContextMenu(ListItem*);
    using Config = Tcl2CaplControllerConfig;
    using Procedures = Tcl2CaplControllerConfig::Procedures;
    using ProceduresRef = Procedures&;
public:
    ProceduresList();
    using Super = QWidget;
protected:
    using Layout = QVBoxLayout;
    using SavedProceduresPtr = Procedures*;
    using NewProcedures = Procedures;
    using RemovedProcedures = QStringList;
    using LoadProceduresSettings = LoadConfigSettings;

    Layout layout;
    QListWidget list;

    struct CurEditItemInfo{
        ListItem* item = nullptr;
        QString oldName;
    } curEditItemInfo;

    SavedProceduresPtr savedProcedures = nullptr;
    NewProcedures newProcedures;
    RemovedProcedures removedProcedures;

    inline ListItem* currentItem()const{return static_cast<ListItem*>(list.currentItem());}
    inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(list.itemAt(p));}

    bool eventFilter(QObject* obj, QEvent* ev)override;

    bool tryToManageProceduresName(QString, QString);

public:
    inline bool anyChanges()const{
        return not (newProcedures.isEmpty() and removedProcedures.isEmpty());
    }
    inline void restoreSavedData(){
        newProcedures.clear();
        removedProcedures.clear();
    }
    void commitChanges(){}

    void loadProcedures(SavedProceduresPtr, LoadProceduresSettings);

    inline void clearChanges(){
        newProcedures.clear();
        removedProcedures.clear();
    }

    void reloadGui();
};


#endif // PROCEDURESLIST_HPP
