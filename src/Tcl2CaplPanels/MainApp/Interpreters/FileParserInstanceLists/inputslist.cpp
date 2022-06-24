#include"inputsList.hpp"
#include<QMenu>
#include<QAction>

void InputsList::newFile(QString mainPath)
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
            addElement(new InputsListElement(mainPath));
        }
    }
}

void InputsList::newFolder(QString mainDirPath)
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
            addElement(new InputsListElement(mainDirPath));
        }
    }
}

template <>
void InputsList::execRequest_ContextMenu<InputsList::Request_ContextMenu::AddFile>(InputsList::InputsListElement* item)
{
    QString mainPath =  QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik:"), QString(), QString("Skrypt (*.tc)"));
    newFile(mainPath);
}

template <>
void InputsList::execRequest_ContextMenu<InputsList::Request_ContextMenu::AddFolder>(InputsList::InputsListElement* item)
{
    QString mainDirPath =  QFileDialog::getExistingDirectory(nullptr, QString("Wybierz folder:"));
    newFolder(mainDirPath);

}

template <>
void InputsList::execRequest_ContextMenu<InputsList::Request_ContextMenu::EditFile>(InputsList::InputsListElement* item_)
{
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
        if(index == count()){
            QFileInfo file(mainPath);
            item_->setFileInfo(file);
        }
    }
}

template <>
void InputsList::execRequest_ContextMenu<InputsList::Request_ContextMenu::EditFolder>(InputsList::InputsListElement* item_)
{
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
        if(index == count()){
            QFileInfo file(mainDirPath);
            item_->setFileInfo(file);
        }
    }
}

template<>
void InputsList::execRequest_ContextMenu<InputsList::Request_ContextMenu::Remove>(InputsListElement* item_)
{delete item_;}

template<>
void InputsList::execRequest_ContextMenu<InputsList::Request_ContextMenu::Clear>(InputsListElement* item_)
{list.clear();}


bool InputsList::eventFilter(QObject* obj, QEvent* ev)
{
    if(ev->type() ==  QEvent::ContextMenu)
        {
            using ListItem = InputsListElement;
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

            ListItem* item = itemAt(cev->pos());

            // Specify file and error checking
            using Actions = QList<QAction*>;
            using ActionFuncs = QList<InputsList::Request_ContextMenu_Func>;
            using Request = InputsList::Request_ContextMenu;
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
                    &InputsList::execRequest_ContextMenu<Request::AddFolder>,
                    &InputsList::execRequest_ContextMenu<Request::AddFile>,
                    &InputsList::execRequest_ContextMenu<Request::EditFolder>,
                    &InputsList::execRequest_ContextMenu<Request::EditFile>,
                    &InputsList::execRequest_ContextMenu<Request::Remove>,
                    &InputsList::execRequest_ContextMenu<Request::Clear>,
                };
            }else{
                actions = {
                  new QAction("Dodaj folder"),
                    new QAction("Dodaj plik"),
                    new QAction("Usuń wszystkie")
                };
                actionFuncs = {
                    &InputsList::execRequest_ContextMenu<Request::AddFolder>,
                    &InputsList::execRequest_ContextMenu<Request::AddFile>,
                    &InputsList::execRequest_ContextMenu<Request::Clear>,
                };
            }

            // After configuration
            if(menu){
                menu->addActions(actions);
                int index = actions.indexOf( menu->exec(cev->globalPos()));
                if(index >= 0){
                    Q_ASSERT_X(index < actionFuncs.size(), "InputsList Menu", "Index error for action functions");
                    (this->*(actionFuncs.at(index)))(item);
                }
                delete menu; menu = nullptr;
            }
        }

    return QWidget::eventFilter(obj, ev);
}
