#ifndef WRITEONLYPROCEDURESLIST_HPP
#define WRITEONLYPROCEDURESLIST_HPP

#include<QListWidgetItem>
#include<QPushButton>
#include<QVBoxLayout>
#include<QToolBar>
#include"Tcl2Capl/controllerconfig.hpp"
#include"loadconfigsettings.hpp"

class QFileInfo;
class WriteOnlyProceduresList : public QWidget{
    class ListItem : public QListWidgetItem{
    public:
        inline ListItem(QString str = QString()) : QListWidgetItem(str){
             setFlags(flags() | Qt::ItemNeverHasChildren | Qt::ItemIsEditable);

        }
    };
    using Request_ContextMenu_Func = void (WriteOnlyProceduresList::*)(ListItem*);
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
    using WriteOnlyProceduresPtr = Config::WriteOnlyProcedures*;
    using LoadProceduresSettings = LoadConfigSettings;
public:

    WriteOnlyProceduresList();
    using Super = QWidget;
protected:

    using Layout = QVBoxLayout;
    using SavedProcedures = Config::WriteOnlyProcedures*;
    using NewProcedures = QStringList;
    using RemovedProcedures = QStringList;

    Layout layout;
    QListWidget list;

    struct CurEditItemInfo{
        ListItem* item = nullptr;
        QString oldName;
    } curEditItemInfo;


    SavedProcedures savedProcedures = nullptr;
    NewProcedures newProcedures;
    RemovedProcedures removedProcedures;

    inline ListItem* currentItem()const{return static_cast<ListItem*>(list.currentItem());}
    inline ListItem* itemAt(const QPoint& p)const{return static_cast<ListItem*>(list.itemAt(p));}

    bool eventFilter(QObject* obj, QEvent* ev)override;
    bool tryToManageProceduresName(QString oldName, QString newName);
public:
    inline bool anyChanges()const{
        return not (newProcedures.isEmpty() and removedProcedures.isEmpty());
    }

    inline void restoreSavedData(){
        newProcedures.clear();
        removedProcedures.clear();
    }
    void commitChanges(){}

    void loadProcedures(WriteOnlyProceduresPtr, LoadProceduresSettings);

    inline void clearChanges(){
        newProcedures.clear();
        removedProcedures.clear();
    }

    void reloadGui();
};

#endif // WRITEONLYPROCEDURESLIST_HPP
