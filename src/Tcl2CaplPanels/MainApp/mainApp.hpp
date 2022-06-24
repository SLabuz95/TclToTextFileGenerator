#ifndef MAINAPP_HPP
#define MAINAPP_HPP

#include<QMainWindow>
#include<QSplitter>
#include<QMdiArea>
#include<QMdiSubWindow>
#include"InstanceList/instanceList.hpp"
#include<QEvent>

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
    /*class InterpreterInterface : public QMdiSubWindow{
    public:
        enum class Type{
            FileInterpreter,
            TextInterpreter
        };
        InterpreterInterface(Type type, QMdiArea* area) : QMdiSubWindow(area), type(type){}

        const Type type;
    };*/
    class SubWindow : public QMdiSubWindow{
    public:
        using QMdiSubWindow::QMdiSubWindow;

        void closeEvent(QCloseEvent *) override{
            hide();
        }
    };

    //using SubWindow = QMdiSubWindow;
    using SubWindowsList = QList<SubWindow*>;
    using SubWindowsRange = QPair<SubWindowsList::Iterator, SubWindowsList::Iterator>;

    MainWindow(App&);
    ~MainWindow()override{}

    void insertSubWindow(InstanceList::SubWindowPositionInfo&&, QWidget*);
    void removeSubWindow(InstanceList::SubWindowPositionInfo&&);
    void removeInstance(InstanceListElement*);
    void activateInterpreter(InstanceList::SubWindowPositionInfo&&);

    void interpreterNameChanged(InstanceList::SubWindowPositionInfo&&);
    void instanceNameChanged(InstanceListElement*);

    SubWindowsRange allInterpretersOfInstance(InstanceListElement*);
protected:
    App& app_;
    using View = QMdiArea;
    using Splitter = MainWindowSplitter;
    Splitter splitter;    // Central Widget
    InstanceList instanceList;
    View view;
    SubWindowsList subWindows;

public:
    App& app(){return app_;}
};

#endif // MAINAPP_HPP