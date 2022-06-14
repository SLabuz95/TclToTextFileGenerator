#include"mainApp.hpp"
#include<QSplitterHandle>
#include<QPainter>
#include<QEnterEvent>
#include"app.hpp"


MainWindow::MainWindow(App& app)
    :  app(app), instanceList(*this)
{
    splitter.addWidget(&instanceList);
    splitter.addWidget(&view);
    setCentralWidget(&splitter);
    show();
}

void MainWindow::insertSubWindow(InstanceList::SubWindowPositionInfo&& positionInfo, QWidget* internalWidget){
    int subWindowListIndex = 0;
    for(int instanceIndex = 0 ; instanceIndex < positionInfo.indexOfInstance; instanceIndex++){
        subWindowListIndex += instanceList.topLevelItem(instanceIndex)->childCount();
    }
    subWindowListIndex += positionInfo.indexOfInterpreter;
    QMdiSubWindow* subWindow = new QMdiSubWindow(&view);
    subWindow->setWidget(internalWidget);
    subWindows.insert(subWindowListIndex, subWindow);
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

