#include"filesListElement.hpp"
#include"filesList.hpp"
#include<QApplication>
#include<QDir>
#include<QMessageBox>

FilesListElement::FilesListElement()
    : QTreeWidgetItem()
{

}


void FilesListElement::addChild(FilesListElement *elem, QFileInfo& file)
{
    elem->setInfo(file);
    QTreeWidgetItem::addChild(elem);
}


void FilesListElement::setInfo(QFileInfo& file){
    if(file.isDir()){
        setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));
        setText(0, file.fileName());
        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }else{
        setIcon(0, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
        setText(0, file.fileName());
        setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    }

}

void FilesListElement::load(){
    const QStringList FILTERS = QStringList{"*.xml", "*.tc", "*.tcl"};
    QString path;
    getParentPath(path);

    setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    QFileInfo fileInfo(path);
    if(fileInfo.exists() and fileInfo.isDir()){
        // Find All Items
        QFileInfoList files;
        FilesList::findDirsAndFilteredFiles(fileInfo.filePath(), FILTERS, files);

        QFileInfoList::Iterator file = files.begin();

        for(; file < files.end();  file++)
        {
            addChild(new FilesListElement, *file);
        }
    }else{
        QMessageBox::critical(nullptr, "Internal Error", "File not Found: " + fileInfo.filePath());
    }

}



void FilesListElement::unload(){
    takeChildren();
    setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirClosedIcon));
}

