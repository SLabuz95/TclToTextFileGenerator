#include"filesList.hpp"
#include"filesListElement.hpp"
#include"Tcl2CaplPanels/MainApp/Interpreters/tcl2caplfileparserinstance.hpp"

#include<QMouseEvent>
#include<QContextMenuEvent>
#include<QMenu>
#include"filesListRootSelection.hpp"
#include<QHeaderView>
#include<QMessageBox>

FilesList::FilesList(Tcl2CaplFileParserInstance& parent)
    : parent_(parent),
      rootSelectionHeader(*this)
{
    setHeader(&rootSelectionHeader);
    viewport()->installEventFilter(this);
}

FilesList::~FilesList(){

}

Tcl2CaplFileParserInstance* FilesList::parent()
{
    return &parent_;
}

Tcl2CaplFileParserInstance* FilesList::parentWidget()
{
    return &parent_;
}

FilesListElement* FilesList::topLevelItem(int index)
{
    return static_cast<FilesListElement*>(QTreeWidget::topLevelItem(index));
}

void FilesList::addTopLevelItem(FilesListElement *newElement, QFileInfo& file)
{
    newElement->setInfo(file);
    QTreeWidget::addTopLevelItem(newElement);
}

FilesListElement* FilesList::itemAt(const QPoint& p)
{
    return static_cast<FilesListElement*>(QTreeWidget::itemAt(p));
}

bool FilesList::eventFilter(QObject* obj, QEvent* ev)
{
    switch(ev->type()){

    case QEvent::ContextMenu:
    {
        QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);

        FilesListElement* item = itemAt(cev->pos());

        if(item){
            // Specify file and error checking
            QString path;
            item->getParentPath(path);
            QFileInfo file(path);

            using Actions = QList<QAction*>;
            using ActionFuncs = QList<Tcl2CaplFileParserInstance::Request_FileInfoRef_Func>;
            using Request = Tcl2CaplFileParserInstance::Request_FileInfoRef;
            Actions actions;
            ActionFuncs actionFuncs;
            QMenu* menu = nullptr;

            if(file.exists()){
                if(file.isDir()){   // Directory
                    menu = new QMenu;
                    actions = {
                      new QAction("Dodaj do definicji"),
                      new QAction("Dodaj do skryptów Tcl"),
                      new QAction("Ustaw jako wyjście")
                    };
                    actionFuncs = {
                        &Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Request::AddToDefinitions>,
                        &Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Request::AddToInputs>,
                        &Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Request::SetAsOutput>
                    };

                }else{
                    if(file.suffix() == "xml"){   // Configuration
                        menu = new QMenu;
                        actions = {
                          new QAction("Ustaw jako plik konfiguracyjny")
                        };
                        actionFuncs = {
                            &Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Request::SetAsConfiguration>,
                        };

                    }else{
                        if(file.suffix() == "tc"){    // TestCase
                            menu = new QMenu;
                            actions = {
                              new QAction("Dodaj do skryptów Tcl")
                            };
                            actionFuncs = {
                                &Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Request::AddToInputs>,
                            };


                        }else{
                            if(file.suffix() == "tcl"){   // TCL File
                                menu = new QMenu;
                                actions = {
                                  new QAction("Ustaw jako definicje")
                                };
                                actionFuncs = {
                                    &Tcl2CaplFileParserInstance::execRequest_FileInfoRef<Request::AddToDefinitions>,
                                };

                            }
                        }
                    }
                }
                // After configuration
                if(menu){
                    menu->addActions(actions);
                    int index = actions.indexOf( menu->exec(cev->globalPos()));
                    if(index >= 0){
                        Q_ASSERT_X(index < actionFuncs.size(), "FilesList Menu", "Index error for action functions");
                        (parent()->*(actionFuncs.at(index)))(file);
                    }
                    delete menu; menu = nullptr;
                }

            }else{
                QMessageBox::critical(nullptr, "Internal Error", "File not Found: " + file.filePath());
            }
        }
    }
        break;
    case QEvent::MouseButtonPress:{
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        if((lastClickedItem = itemAt(mev->pos())))
            lastClickedItemExpanded = lastClickedItem->isExpanded();

    }
        break;
    case QEvent::Paint:
    {
        if(lastClickedItem){
            if(lastClickedItemExpanded != lastClickedItem->isExpanded()){ // Changed
                if(lastClickedItem->isExpanded()){
                    lastClickedItem->load();
                }else{
                    lastClickedItem->unload();
                }
            }
            lastClickedItem = nullptr;
        }
    }
        break;
    default:
        break;
    }
    return QTreeWidget::eventFilter(obj, ev);
}

void FilesList::updateRootPath(){
    clear();
    // Find All Items
    QFileInfoList files;
    findDirsAndFilteredFiles(rootSelectionHeader.rootPathStr(), filters(), files);

    // Create Items

    QFileInfoList::Iterator file = files.begin();

    for(; file < files.end();  file++)
    {
        addTopLevelItem(new FilesListElement, *file);
    }

}

void FilesList::findDirsAndFilteredFiles(QString path, QStringList filters, QFileInfoList& outputList){
    QDir rootDir(path);
    rootDir.setFilter(QDir::Filter::AllDirs | QDir::NoDotAndDotDot);
    outputList = rootDir.entryInfoList();
    rootDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    rootDir.setNameFilters(filters);
    outputList.append(rootDir.entryInfoList());
}

