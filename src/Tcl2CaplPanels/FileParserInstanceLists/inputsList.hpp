#ifndef INPUTSLIST_HPP
#define INPUTSLIST_HPP

#include<QListWidget>
#include<QListWidgetItem>
#include<QLabel>
#include<QVBoxLayout>
#include<QFileInfo>
#include<QApplication>
#include<QPushButton>
#include<QMouseEvent>
#include<QFileDialog>

class InputsList : public QWidget{
    class InputsListElement : public QListWidgetItem{

    public:
        class InnerWidget : public QWidget{
        public:
            QHBoxLayout layout;
            QLabel info;
            /* [0.9.0]
            QPushButton fileButton;
            QPushButton folderButton;
            QPushButton removeButton;
            [0.9.0] End */
            bool active = true;
        };
        InputsListElement(QString path)
            : QListWidgetItem(
                  (QFileInfo(path).isDir())? QApplication::style()->standardIcon(QStyle::SP_DirIcon) :
                                  QApplication::style()->standardIcon(QStyle::SP_FileIcon), QString())
        {
            setToolTip(QFileInfo(path).filePath());

        }
        /* [0.9.0]
        inline QPushButton& fileButtonRef(){return innerWidget()->fileButton;}
        inline QPushButton& folderButtonRef(){return innerWidget()->folderButton;}
        inline QPushButton& removeButtonRef(){return innerWidget()->removeButton;}
        [0.9.0] End */
        inline InnerWidget* innerWidget(){return static_cast<InnerWidget*>(QListWidgetItem::listWidget()->itemWidget(this));}
        inline InputsList* listWidget(){return static_cast<InputsList*>(listWidget());}


        void initItemWidget(InputsList& list){
            InnerWidget* widget = new InnerWidget;
            widget->info.setText(QFileInfo(toolTip()).fileName());
            /* [0.9.0]
            widget->folderButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
            widget->fileButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
            widget->removeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
            widget->folderButton.installEventFilter(&list);
            widget->fileButton.installEventFilter(&list);
            widget->removeButton.installEventFilter(&list);
            [0.9.0] End*/
            widget->layout.addWidget(&widget->info, Qt::AlignLeft);
            /* [0.9.0]
            widget->layout.addWidget(&widget->fileButton, Qt::AlignRight);
            widget->layout.addWidget(&widget->folderButton, Qt::AlignRight);
            widget->layout.addWidget(&widget->removeButton, Qt::AlignRight);
            [0.9.0] End */
            widget->layout.setSpacing(0);
            widget->layout.setContentsMargins(0,0,0,0);
            widget->setLayout(&widget->layout);
            list.setItemWidget(this, widget);
        }
        inline void setFileInfo(QFileInfo& file){
            innerWidget()->info.setText(file.fileName());
            setIcon((file.isDir())? QApplication::style()->standardIcon(QStyle::SP_DirIcon) :
                                               QApplication::style()->standardIcon(QStyle::SP_FileIcon));
            innerWidget()->info.setToolTip(file.filePath());
        }
        inline bool isActive(){
            bool exist = false;
            if(exist = QFile::exists(toolTip())){
                setBackground(Qt::transparent);
            }else{
                setBackground(Qt::red);
            }
            return exist;
        }
    };

    using Request_ContextMenu_Func = void (InputsList::*)(InputsListElement*);
    enum class Request_ContextMenu{
        AddFolder,
        AddFile,
        EditFolder,
        EditFile,
        Remove,
        Clear,
        Size
    };
    template<Request_ContextMenu>
    void execRequest_ContextMenu(InputsListElement*);

    QVBoxLayout layout;
    QLabel title;
    QListWidget list;
/* [0.9.0]
    QListWidgetItem lastElement;
    QWidget lastElementWidget;
    QHBoxLayout lastElementLayout;

    QPushButton newFileButton;
    QPushButton newFolderButton;
[0.9.0] End */
protected:
    /* [0.9.0]
    void editFileButtonClicked(InputsListElement*);
    void editFolderButtonClicked(InputsListElement* );
    [0.9.0] End */

    bool eventFilter(QObject* obj, QEvent* ev) override;

public:
    using InputsListElement = InputsListElement;
    InputsList()
    {
        title.setText("Skrypty Tcl");
        title.setAlignment(Qt::AlignCenter);

        /*
        lastElementLayout.addWidget(&newFileButton, Qt::AlignRight);
        lastElementLayout.addWidget(&newFolderButton, Qt::AlignRight);
        newFileButton.installEventFilter(this);
        newFileButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
        newFolderButton.installEventFilter(this);
        newFolderButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));

        lastElementWidget.setLayout(&lastElementLayout);
        lastElementLayout.setSpacing(0);
        lastElementLayout.setContentsMargins(0,0,0,0);
         list.addItem(&lastElement);
         list.setItemWidget(&lastElement, &lastElementWidget);
        lastElementWidget.show();
[0.9.0] End */
        layout.addWidget(&title);
        layout.addWidget(&list);
        setLayout(&layout);
        list.viewport()->installEventFilter(this);

    }

    inline void addElement(InputsListElement* element)
    {
        list.insertItem(count(), element);
        element->initItemWidget(*this);
    }

    inline void setItemWidget(QListWidgetItem* lw,  QWidget* w){
        list.setItemWidget(lw, w);        
    }
    inline InputsListElement* item(int row)const{
        return static_cast<InputsListElement*>(list.item(row));
    }
    inline InputsListElement* currentItem()const{return static_cast<InputsListElement*>(list.currentItem());}
    inline InputsListElement* itemAt(const QPoint& p)const{return static_cast<InputsListElement*>(list.itemAt(p));}

    inline int count()const{return list.count();}

    void newFile(QString );
    void newFolder(QString );

    inline bool checkActive(){
        bool allActive = true;
        for(int itemIndex = 0; itemIndex < count(); itemIndex++)
        {
            if(allActive){
                allActive = item(itemIndex)->isActive();
            }else{
                item(itemIndex)->isActive();
            }
        }
        return allActive;
    }
};

#endif // INPUTSLIST_HPP
