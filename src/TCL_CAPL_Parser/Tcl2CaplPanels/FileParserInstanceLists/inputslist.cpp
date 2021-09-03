#include"inputsList.hpp"

void InputsList::newFileButtonClicked(QString mainPath){
    if(QFileInfo::exists(mainPath)){
        int index = 0;
        bool isActive = false;
        for( ; index < count(); index++){
            // File name which stats with the same characters as other file or dir
            // If path is the same - break, otherwise - ignore
            isActive = item(index)->isActive();
            if(mainPath.contains(item(index)->toolTip()) and
                    ((mainPath == item(index)->toolTip() and isActive) or
                     (QFileInfo(item(index)->toolTip()).path() != QFileInfo(mainPath).path() and QFileInfo(item(index)->toolTip()).isDir()))){
                // The same path - ignore, cause its other file or folder in the same path
                // Diffrent Path - break, cause try to add file to another folder (folder and file cannot be the same)
                    break;
                }
        }

        if(index == trueLastItemIndex()){
            addElement(new InputsListElement(mainPath));
        }
    }
}

void InputsList::newFolderButtonClicked(QString mainDirPath){
    if(QFileInfo::exists(mainDirPath)){
        int index = 0;
        bool isActive = false;
        for( ; index < count(); index++){
            isActive = item(index)->isActive();
            if(mainDirPath.contains(item(index)->toolTip()) and
                    ((mainDirPath == item(index)->toolTip() and isActive) or
                     (QFileInfo(item(index)->toolTip()).path() != QFileInfo(mainDirPath).path() and QFileInfo(item(index)->toolTip()).isDir()))){
                // The same path - ignore, cause its other file or folder in the same path
                // Diffrent Path - break, cause try to add file to another folder (folder and file cannot be the same)
                    break;
                }
            if(item(index)->toolTip().contains(mainDirPath) and QFileInfo(item(index)->toolTip()).path() != QFileInfo(mainDirPath).path()){
                delete list.item(index);
                index--;
            }
        }
        if(index == trueLastItemIndex()){
            addElement(new InputsListElement(mainDirPath));
        }
    }
}


void InputsList::editFileButtonClicked(InputsListElement* item_){
    QString mainPath =  QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik z definicjami:"), QString(), QString("Skrypt (*.tc)"));
    if(QFileInfo::exists(mainPath)){
        int index = 0;
        bool isActive = false;
        for( ; index < count(); index++){
            isActive = item(index)->isActive();
            if(item_ != item(index)){
                if(mainPath.contains(item(index)->toolTip()) and
                        ((mainPath == item(index)->toolTip() and isActive) or
                         (QFileInfo(item(index)->toolTip()).path() != QFileInfo(mainPath).path() and QFileInfo(item(index)->toolTip()).isDir()))){
                    // The same path - ignore, cause its other file or folder in the same path
                    // Diffrent Path - break, cause try to add file to another folder (folder and file cannot be the same)
                        break;
                    }
            }
        }
        if(index == trueLastItemIndex()){
            QFileInfo file(mainPath);
            item_->setFileInfo(file);
        }
    }
}

void InputsList::editFolderButtonClicked(InputsListElement* item_){
    QString mainDirPath =  QFileDialog::getExistingDirectory(nullptr, QString("Wybierz folder z definicjami:"));
    if(QFileInfo::exists(mainDirPath)){
        int index = 0;
        bool isActive = false;
        for( ; index < count(); index++){
            isActive = item(index)->isActive();
            if(item_ != item(index)){
                if(mainDirPath.contains(item(index)->toolTip()) and
                        ((mainDirPath == item(index)->toolTip() and isActive) or
                         (QFileInfo(item(index)->toolTip()).path() != QFileInfo(mainDirPath).path() and QFileInfo(item(index)->toolTip()).isDir()))){
                    // The same path - ignore, cause its other file or folder in the same path
                    // Diffrent Path - break, cause try to add file to another folder (folder and file cannot be the same)
                        break;
                    }
                if(item(index)->toolTip().contains(mainDirPath) and QFileInfo(item(index)->toolTip()).path() != QFileInfo(mainDirPath).path()){
                    delete list.takeItem(index);
                    index--;
                }
            }
        }
        if(index == trueLastItemIndex()){
            QFileInfo file(mainDirPath);
            item_->setFileInfo(file);
        }
    }
}
