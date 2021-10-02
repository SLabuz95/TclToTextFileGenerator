#ifndef FILESLIST_HPP
#define FILESLIST_HPP

#include<QTreeWidget>
#include"Utils/class.hpp"
#include"filesListRootSelection.hpp"
#include<QFileInfo>

class Tcl2CaplFileParserInstance;
class FilesListElement;
class FilesList : public QTreeWidget{
    using Items = FilesListElement*;

public:
    //using MenuOption = MenuOption;
    FilesList(Tcl2CaplFileParserInstance& parent);
    virtual ~FilesList() override;

protected:
    Tcl2CaplFileParserInstance& parent_;

    FilesListElement* lastClickedItem = nullptr;
    bool lastClickedItemExpanded = false;

    FilesListRootSelection rootSelectionHeader;

public:
     FilesListElement* topLevelItem(int index);
     void addTopLevelItem(FilesListElement* newElement, QFileInfo&);
     FilesListElement* itemAt(const QPoint& p);

     void updateRootPath();
     inline QString rootPath()const{return rootSelectionHeader.rootPathStr();}

     static const QStringList filters(){return QStringList{"*.xml", "*.tc", "*.tcl"};}
     static void findDirsAndFilteredFiles(QString path, QStringList filters, QFileInfoList& outputList);

    WIDGET_PARENT_METHODS_DECL(Tcl2CaplFileParserInstance);

protected:
    bool eventFilter(QObject*, QEvent*) override;

};

#endif // FILESLIST_HPP
