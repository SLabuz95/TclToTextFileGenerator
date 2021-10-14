#include"definitionsList.hpp"
#include<QMenu>
#include<QAction>
#include<QContextMenuEvent>


void DefinitionsList::newFile(QString mainPath)
{
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

        if(index == count()){
            addElement(new DefinitionsListElement(mainPath));
        }
    }
}


void DefinitionsList::newFolder(QString mainDirPath)
{
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
        if(index == count()){
            addElement(new DefinitionsListElement(mainDirPath));
        }
    }
}

template<>
void DefinitionsList::execRequest_ContextMenu<DefinitionsList::Request_ContextMenu::AddFile>(DefinitionsListElement* item_)
{
    QString mainPath =  QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik z definicjami:"), QString(), QString("Skrypt (*.tcl)"));
    newFile(mainPath);
}

template<>
void DefinitionsList::execRequest_ContextMenu<DefinitionsList::Request_ContextMenu::AddFolder>(DefinitionsListElement* item_)
{
    QString mainDirPath =  QFileDialog::getExistingDirectory(nullptr, QString("Wybierz folder z definicjami:"));
    newFolder(mainDirPath);
}

template<>
void DefinitionsList::execRequest_ContextMenu<DefinitionsList::Request_ContextMenu::EditFile>(DefinitionsListElement* item_)
{
    QString mainPath =  QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik z definicjami:"), QString(), QString("Skrypt (*.tcl)"));
    if(QFileInfo::exists(mainPath) and mainPath != item_->toolTip()){
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
        if(index == count()){
            QFileInfo file(mainPath);
            item_->editFileInfo(file);
        }
    }
}

template<>
void DefinitionsList::execRequest_ContextMenu<DefinitionsList::Request_ContextMenu::EditFolder>(DefinitionsListElement* item_)
{
    QString mainDirPath =  QFileDialog::getExistingDirectory(nullptr, QString("Wybierz folder z definicjami:"));
    if(QFileInfo::exists(mainDirPath) and mainDirPath != item_->toolTip()){
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
        if(index == count()){
            QFileInfo file(mainDirPath);
            item_->editFileInfo(file);
        }
    }
}


template<>
void DefinitionsList::execRequest_ContextMenu<DefinitionsList::Request_ContextMenu::Remove>(DefinitionsListElement* item_){delete item_;}

template<>
void DefinitionsList::execRequest_ContextMenu<DefinitionsList::Request_ContextMenu::Clear>(DefinitionsListElement* item_){list.clear();}


bool DefinitionsList::eventFilter(QObject* obj, QEvent* ev)
{
    if(ev->type() ==  QEvent::ContextMenu)
        {
            using ListItem = DefinitionsListElement;
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

            ListItem* item = itemAt(cev->pos());

            // Specify file and error checking
            using Actions = QList<QAction*>;
            using ActionFuncs = QList<DefinitionsList::Request_ContextMenu_Func>;
            using Request = DefinitionsList::Request_ContextMenu;
            Actions actions;
            ActionFuncs actionFuncs;
            QMenu* menu = nullptr;

            menu = new QMenu;
            if(item){
                actions = {
                  new QAction("Dodaj folder"),
                    new QAction("Dodaj plik"),
                  new QAction("Wskaź inny folder"),
                    new QAction("Wskaż inny plik"),
                  new QAction("Usuń"),
                    new QAction("Usuń wszystkie")
                };
                actionFuncs = {
                    &DefinitionsList::execRequest_ContextMenu<Request::AddFolder>,
                    &DefinitionsList::execRequest_ContextMenu<Request::AddFile>,
                    &DefinitionsList::execRequest_ContextMenu<Request::EditFolder>,
                    &DefinitionsList::execRequest_ContextMenu<Request::EditFile>,
                    &DefinitionsList::execRequest_ContextMenu<Request::Remove>,
                    &DefinitionsList::execRequest_ContextMenu<Request::Clear>,
                };
            }else{
                actions = {
                  new QAction("Dodaj folder"),
                    new QAction("Dodaj plik"),
                    new QAction("Usuń wszystkie")
                };
                actionFuncs = {
                    &DefinitionsList::execRequest_ContextMenu<Request::AddFolder>,
                    &DefinitionsList::execRequest_ContextMenu<Request::AddFile>,
                    &DefinitionsList::execRequest_ContextMenu<Request::Clear>,
                };
            }


            // After configuration
            if(menu){
                menu->addActions(actions);
                int index = actions.indexOf( menu->exec(cev->globalPos()));
                if(index >= 0){
                    Q_ASSERT_X(index < actionFuncs.size(), "DefinitionsList Menu", "Index error for action functions");
                    (this->*(actionFuncs.at(index)))(item);
                }
                delete menu, menu = nullptr;
            }
        }

    if(ev->type() == QEvent::MouseButtonDblClick and obj == list.viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        //curEditItemInfo.item = itemAt(mev->pos());
    }

    return QWidget::eventFilter(obj, ev);
}

