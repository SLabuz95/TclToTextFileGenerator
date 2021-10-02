#include"filesListRootSelection.hpp"
#include"filesList.hpp"
#include<QFileDialog>

#include<QMouseEvent>
#include<QShowEvent>
#include<QApplication>

FilesListRootSelection::FilesListRootSelection(FilesList& filesList)
    : QHeaderView(Qt::Horizontal),
      filesList(filesList)
{

    const QString WORKAREAS_PATH = "C:\\_workdir\\_work_areas";
    rootPath.setReadOnly(true);
    rootPath.setToolTipDuration(10000);
    if(QFileInfo::exists(WORKAREAS_PATH)){
        setRootPath(WORKAREAS_PATH);
    }else{
        setRootPath(QDir::homePath());
    }
    //rootPath.setParent(this);
    selectButton.installEventFilter(this);
    selectButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));

    centralLayout.addWidget(&rootPath);
    centralLayout.addWidget(&selectButton);
    centralLayout.setSpacing(0);
    centralWidget.setContentsMargins(0,0,0,0);

    centralWidget.setParent(this);
    centralWidget.setLayout(&centralLayout);
    centralLayout.setContentsMargins(0,0,0,0);
    centralWidget.show();
    show();


}


bool FilesListRootSelection::eventFilter(QObject *obj, QEvent *ev)
{
    switch(ev->type())
    {
        case QEvent::MouseButtonPress:
        {
            if(obj == &selectButton){
                chooseRootPath();
            }
        }
        break;
    default:
        break;
    }

    return QHeaderView::eventFilter(obj, ev);
}

void FilesListRootSelection::chooseRootPath(){
    QString path = QFileDialog::getExistingDirectory(nullptr, "Wybierz folder główny:");
    if(not path.isEmpty() and QFileInfo::exists(path))
    {
        setRootPath(path);
    }
}

void FilesListRootSelection::setRootPath(QString path){
    QDir dir = QDir(path);
    // Check its drive?
    rootPathStr_ = path;
    if(dir.isRoot())
        rootPath.setText(rootPathStr_);
    else
        rootPath.setText(QDir(rootPathStr_).dirName());
    rootPath.setToolTip(rootPathStr_);
    filesList.updateRootPath();
}

void FilesListRootSelection::resizeEvent(QResizeEvent *ev){
    centralWidget.setGeometry(0,0, width(), height());
    resizeSection(0, width());
    QHeaderView::resizeEvent(ev);
}
