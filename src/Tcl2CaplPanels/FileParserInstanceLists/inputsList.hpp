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
            QPushButton fileButton;
            QPushButton folderButton;
            QPushButton removeButton;
            bool active = true;
        };
        InputsListElement(QString path)
            : QListWidgetItem(
                  (QFileInfo(path).isDir())? QApplication::style()->standardIcon(QStyle::SP_DirIcon) :
                                  QApplication::style()->standardIcon(QStyle::SP_FileIcon), QString())
        {
            setToolTip(QFileInfo(path).filePath());

        }
        inline QPushButton& fileButtonRef(){return innerWidget()->fileButton;}
        inline QPushButton& folderButtonRef(){return innerWidget()->folderButton;}
        inline QPushButton& removeButtonRef(){return innerWidget()->removeButton;}
        inline InnerWidget* innerWidget(){return static_cast<InnerWidget*>(QListWidgetItem::listWidget()->itemWidget(this));}
        inline InputsList* listWidget(){return static_cast<InputsList*>(listWidget());}


        void initItemWidget(InputsList& list){
            InnerWidget* widget = new InnerWidget;
            widget->info.setText(QFileInfo(toolTip()).fileName());
            widget->folderButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
            widget->fileButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
            widget->removeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
            widget->folderButton.installEventFilter(&list);
            widget->fileButton.installEventFilter(&list);
            widget->removeButton.installEventFilter(&list);
            widget->layout.addWidget(&widget->info, Qt::AlignLeft);
            widget->layout.addWidget(&widget->fileButton, Qt::AlignRight);
            widget->layout.addWidget(&widget->folderButton, Qt::AlignRight);
            widget->layout.addWidget(&widget->removeButton, Qt::AlignRight);
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
            if(QFile::exists(toolTip())){
                setBackground(Qt::transparent);
            }else{
                setBackground(Qt::red);
            }
            return exist and isActive();
        }

    };
    QVBoxLayout layout;
    QLabel title;
    QListWidget list;

    QListWidgetItem lastElement;
    QWidget lastElementWidget;
    QHBoxLayout lastElementLayout;

    QPushButton newFileButton;
    QPushButton newFolderButton;

protected:
    void editFileButtonClicked(InputsListElement*);
    void editFolderButtonClicked(InputsListElement* );
    inline void removeItemButtonClicked(InputsListElement* item_)const{delete item_;}

    bool eventFilter(QObject* obj, QEvent* ev) override
    {
        switch(ev->type()){
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
            if(obj == &newFileButton){
                QString mainPath =  QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik z definicjami:"), QString(), QString("Skrypt (*.tc)"));
                newFileButtonClicked(mainPath);
            }else{
                if(obj == &newFolderButton){
                    QString mainDirPath =  QFileDialog::getExistingDirectory(nullptr, QString("Wybierz folder z definicjami:"));
                    newFolderButtonClicked(mainDirPath);
                }else{
                    InputsListElement* item_ = static_cast<InputsListElement*>(list.itemAt(mev->pos()));
                    if(item_ and item_ != &lastElement){
                        if(obj == &item_->fileButtonRef()){  // FileButton
                            editFileButtonClicked(item_);
                        }else{
                            if(obj == &item_->folderButtonRef()) // FolderButton
                            {
                                editFolderButtonClicked(item_);
                            }else{
                                if(obj == &item_->removeButtonRef())// Remove Button
                                    removeItemButtonClicked(item_);
                            }
                        }
                    }
                }
            }
        }
            break;
        default:
            break;
        }
        return QWidget::eventFilter(obj, ev);
    }

public:
    using InputsListElement = InputsListElement;
    InputsList()
    {
        title.setText("Skrypty Tcl");
        title.setAlignment(Qt::AlignCenter);

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

        layout.addWidget(&title);
        layout.addWidget(&list);
        setLayout(&layout);


    }

    inline void addElement(InputsListElement* element)
    {
        list.insertItem(trueLastItemIndex(), element);
        element->initItemWidget(*this);
    }

    inline void setItemWidget(QListWidgetItem* lw,  QWidget* w){
        list.setItemWidget(lw, w);        
    }
    inline InputsListElement* item(int row)const{
        return static_cast<InputsListElement*>(list.item(row));
    }
    inline int count()const{return trueLastItemIndex();}

    inline int trueLastItemIndex()const{return list.count() - 1;}

    void newFileButtonClicked(QString mainPath);
    void newFolderButtonClicked(QString mainPath);

    inline bool checkActive(){
        bool allActive = true;
        for(int itemIndex = 0; itemIndex < trueLastItemIndex(); itemIndex++)
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
