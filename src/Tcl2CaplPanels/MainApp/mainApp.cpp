#include"mainApp.hpp"
#include<QSplitterHandle>
#include<QPainter>
#include<QEnterEvent>
#include"app.hpp"
#include<QApplication>

MainWindow::MainWindow(App& app)
    :  app_(app), instanceList(*this)
{
    splitter.addWidget(&instanceList);
    splitter.addWidget(&view);
    setCentralWidget(&splitter);
    setGeometry(
       QStyle::alignedRect(
           Qt::LeftToRight,
           Qt::AlignCenter,
           qApp->primaryScreen()->availableGeometry().size()*0.7,
           qApp->primaryScreen()->availableGeometry()
       )
    );
    //view.setViewMode(View::TabbedView);
    show();
}

void MainWindow::insertSubWindow(InstanceList::SubWindowPositionInfo&& positionInfo, QWidget* internalWidget){
    int subWindowListIndex = 0;
    for(int instanceIndex = 0 ; instanceIndex < positionInfo.indexOfInstance; instanceIndex++){
        subWindowListIndex += instanceList.topLevelItem(instanceIndex)->childCount();
    }
    subWindowListIndex += positionInfo.indexOfInterpreter;
    internalWidget->setWindowTitle(instanceList.topLevelItem(positionInfo.indexOfInstance)->text(0) + "::"
                                   + instanceList.topLevelItem(positionInfo.indexOfInstance)->child(positionInfo.indexOfInterpreter)->text(0));
    SubWindow* subWindow = new SubWindow;
    subWindow->setWidget(internalWidget);
    subWindows.insert(subWindowListIndex, subWindow);
    view.addSubWindow(subWindow);
    subWindow->show();
}

void MainWindow::removeSubWindow(InstanceList::SubWindowPositionInfo&& positionInfo){
    int subWindowListIndex = 0;
    for(int instanceIndex = 0 ; instanceIndex < positionInfo.indexOfInstance; instanceIndex++){
        subWindowListIndex += instanceList.topLevelItem(instanceIndex)->childCount();
    }
    subWindowListIndex += positionInfo.indexOfInterpreter;
    delete subWindows.at(subWindowListIndex);
    subWindows.remove(subWindowListIndex);
}

void MainWindow::removeInstance(InstanceListElement* instance){
    const int instanceIndex = instanceList.indexOfTopLevelItem(instance);
    if(instanceIndex != -1){
        int firstSubWindowListIndex = 0;
        for(int i = 0 ; i < instanceIndex; i++){
            firstSubWindowListIndex += instanceList.topLevelItem(i)->childCount();
        }

        for(int i = 0; i < instance->childCount(); i++){
            delete subWindows.at(firstSubWindowListIndex + i);
        }
        subWindows.remove(firstSubWindowListIndex, instance->childCount());
    }
}

void MainWindow::activateInterpreter(InstanceList::SubWindowPositionInfo&& positionInfo){
    int subWindowListIndex = 0;
    for(int instanceIndex = 0 ; instanceIndex < positionInfo.indexOfInstance; instanceIndex++){
        subWindowListIndex += instanceList.topLevelItem(instanceIndex)->childCount();
    }
    subWindowListIndex += positionInfo.indexOfInterpreter;
    subWindows.at(subWindowListIndex)->show();
}

MainWindow::SubWindowsRange MainWindow::allInterpretersOfInstance(InstanceListElement* instanceListItem){
    const int indexOfInstanceItem = instanceList.indexOfTopLevelItem(instanceListItem);
    if(indexOfInstanceItem != -1) // Index has been found
    {
        int indexOfSubWindow = 0;
        for(int i = 0; i < indexOfInstanceItem; i++){
            indexOfSubWindow += instanceListItem->child(i)->childCount();
        }
        // Add + 1 to index to pass instance config window
        return MainWindow::SubWindowsRange{
            subWindows.begin() + indexOfSubWindow + 1, // Add + 1 to index to pass instance config window
            subWindows.begin() + indexOfSubWindow + instanceListItem->child(indexOfInstanceItem)->childCount()
            // SubWindow After last interpreter of Instance
        };
    }
    // Error Case
    return MainWindow::SubWindowsRange{
        subWindows.begin(), // Add + 1 to index to pass instance config window
        subWindows.begin()
    };
}

void MainWindow::interpreterNameChanged(InstanceList::SubWindowPositionInfo&& subWindowPos){
    int subWindowListIndex = 0;
    for(int instanceIndex = 0 ; instanceIndex < subWindowPos.indexOfInstance; instanceIndex++){
        subWindowListIndex += instanceList.topLevelItem(instanceIndex)->childCount();
    }
    subWindowListIndex += subWindowPos.indexOfInterpreter;

    subWindows.at(subWindowListIndex)->setWindowTitle(instanceList.topLevelItem(subWindowPos.indexOfInstance)->text(0) + "::"
                                   + instanceList.topLevelItem(subWindowPos.indexOfInstance)->child(subWindowPos.indexOfInterpreter)->text(0));

}

void MainWindow::instanceNameChanged(InstanceListElement* instance){
    const int instanceIndex = instanceList.indexOfTopLevelItem(instance);
    if(instanceIndex != -1){
        int firstSubWindowListIndex = 0;
        for(int i = 0 ; i < instanceIndex; i++){
            firstSubWindowListIndex += instanceList.topLevelItem(i)->childCount();
        }

        for(int i = 0; i < instance->childCount(); i++)
            subWindows.at(firstSubWindowListIndex + i)->setWindowTitle(
                        instanceList.topLevelItem(instanceIndex)->text(0) + "::"
                        + instance->child(i)->text(0));
    }
}


void MainWindow::Splitter::Handle::paintEvent(QPaintEvent *event)
{
    if(not isNotActive())
        return QSplitterHandle::paintEvent(event);

    QPainter p(this);
    if(textMode){
        p.translate(event->rect().bottomLeft());
        p.rotate(-90);
        p.drawText(event->rect().transposed(), QString("Nawigacja"), QTextOption(Qt::AlignCenter));
    }else{
        const QRect& evRect = event->rect();
        p.setBrush(QBrush(Qt::GlobalColor::black));
        p.drawConvexPolygon(
                    QPolygonF(
                        {
                            QPointF(NonTextModeHandelMargin ,(evRect.height() - NonTextModeHandelTriangleWidth)* 0.5),
                            QPointF(NonTextModeHandelMargin ,(evRect.height()  + NonTextModeHandelTriangleWidth) * 0.5),
                            QPointF(NonTextModeHandelWidth - NonTextModeHandelMargin ,evRect.height() * 0.5)
                        }
                        )
                    );
    }
}

void MainWindow::Splitter::Handle::enterEvent(QEnterEvent *event){
    textMode = true;
    if(isNotActive()){
        if(splitter())
            splitter()->setHandleWidth(TextModeHandelWidth);
        update();
    }else{
        QSplitterHandle::enterEvent(event);
    }
}

void MainWindow::Splitter::Handle::leaveEvent(QEvent *event){
    textMode = false;
    if(isNotActive()){
        if(splitter())
            splitter()->setHandleWidth(NonTextModeHandelWidth);
        update();
    }else{
        QSplitterHandle::leaveEvent(event);
    }
}

