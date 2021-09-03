#ifndef FILESLISTELEMENT_HPP
#define FILESLISTELEMENT_HPP

#include<QTreeWidgetItem>
#include<QFileInfo>
#include<QLayout>
#include<QLabel>
#include"filesList.hpp"

class FilesList;
class FilesListElement :  public QTreeWidgetItem{

public:

    FilesListElement();
    ~FilesListElement(){

    }

    void addChild(FilesListElement*, QFileInfo&);
    void setInfo( QFileInfo&);
    void load();
    void unload();

    inline void getParentPath(QString& path){
        if(parent()){
            static_cast<FilesListElement*>(parent())->getParentPath(path);
        }else{
            path = static_cast<FilesList*>(treeWidget())->rootPath();
        }
        path += QString("/") + text(0);
    }
};


#endif // FILESLISTELEMENT_HPP
