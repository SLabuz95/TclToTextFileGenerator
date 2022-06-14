#ifndef MAINAPP_HPP
#define MAINAPP_HPP

#include<QMainWindow>
#include<QSplitter>
#include<QMdiArea>
#include<QMdiSubWindow>
#include"InstanceList/instanceList.hpp"

class App;
class MainWindow : public QMainWindow{
    class MainWindowSplitter : public QSplitter{
        using Splitter = MainWindowSplitter;
        class Handle : public QSplitterHandle{
            public:
            static constexpr int NonTextModeHandelWidth = 10;
            private:
            static constexpr int TextModeHandelWidth = 20;
            static constexpr int NonTextModeHandelMargin = 1;
            static constexpr int NonTextModeHandelTriangleWidth = NonTextModeHandelWidth - 2*NonTextModeHandelMargin;
            public:
            inline bool isNotActive(){if(splitter()) splitter()->isNotActive();}
            bool textMode = false;
            Handle(Qt::Orientation orientation, Splitter *parent) : QSplitterHandle(orientation, parent){
                if(splitter())
                    splitter()->setHandleWidth(NonTextModeHandelWidth);
            }
            protected:
            inline Splitter* splitter()const{return static_cast<Splitter*>(QSplitterHandle::splitter());}
            void paintEvent(QPaintEvent *event)override;
            void enterEvent(QEnterEvent *event)override;
            void leaveEvent(QEvent *event)override;

        };
    public:
        inline bool isNotActive()const{return sizes().at(0) == 0;}
        static constexpr int NonTextModeHandelWidth = Handle::NonTextModeHandelWidth;
        using QSplitter::moveSplitter;
        using QSplitter::closestLegalPosition;
        QSplitterHandle *createHandle() override{
            return new Handle(orientation(), this);
        }

    };
public:
    MainWindow(App&);
    ~MainWindow()override{}

    void insertSubWindow(InstanceList::SubWindowPositionInfo&&, QWidget*);


protected:
    App& app;
    using View = QMdiArea;
    using Splitter = MainWindowSplitter;
    Splitter splitter;    // Central Widget
    InstanceList instanceList;
    View view;
    QList<QMdiSubWindow*> subWindows;


};

#endif // MAINAPP_HPP
