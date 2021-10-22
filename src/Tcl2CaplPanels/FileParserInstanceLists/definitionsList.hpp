#ifndef DEFINITIONSLIST_HPP
#define DEFINITIONSLIST_HPP

#include<QListWidget>
#include<QListWidgetItem>
#include<QLabel>
#include<QVBoxLayout>
#include<QFileInfo>
#include<QApplication>
#include<QPushButton>
#include<QMouseEvent>
#include<QFileDialog>
#include<QBitArray>
#include<QFileSystemWatcher>

class DefinitionsList : public QWidget{
private:
    class DefinitionsListElement : public QListWidgetItem{
        class InnerWidget : public QWidget{
            ~InnerWidget(){

            }
        public:
            QHBoxLayout layout;
            QLabel info;
            // [0.9.0]
            //QPushButton fileButton;
            //QPushButton folderButton;
            //QPushButton removeButton;
            // [0.9.0] End
            bool active = true;
        };
    public:
        DefinitionsListElement(QString path)
            : QListWidgetItem(
                  (QFileInfo(path).isDir())? QApplication::style()->standardIcon(QStyle::SP_DirIcon) :
                                  QApplication::style()->standardIcon(QStyle::SP_FileIcon), QString())
        {
            setToolTip(QFileInfo(path).filePath());
        }
        ~DefinitionsListElement(){

        }
        // [0.9.0]
        //inline QPushButton& fileButtonRef(){return innerWidget()->fileButton;}
        //inline QPushButton& folderButtonRef(){return innerWidget()->folderButton;}
        //inline QPushButton& removeButtonRef(){return innerWidget()->removeButton;}
        // [0.9.0] End
        inline InnerWidget* innerWidget(){return static_cast<InnerWidget*>(QListWidgetItem::listWidget()->itemWidget(this));}
        inline DefinitionsList* listWidget(){return static_cast<DefinitionsList*>(listWidget());}

        inline void initItemWidget(DefinitionsList& list){
            InnerWidget* widget = new InnerWidget;
            widget->info.setText(QFileInfo(toolTip()).fileName());
            /* [0.9.0]
            widget->folderButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
            widget->fileButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
            widget->removeButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
            widget->folderButton.installEventFilter(&list);
            widget->fileButton.installEventFilter(&list);
            widget->removeButton.installEventFilter(&list);
            [0.9.0] End */
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
        inline void editFileInfo(QFileInfo& file){
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
    using Request_ContextMenu_Func = void (DefinitionsList::*)(DefinitionsListElement*);
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
    void execRequest_ContextMenu(DefinitionsListElement*);

    /*class ChangeControl : protected QStringList{
        uint wrongDataCounter = 0;
    public:
        void updateAll(QStringList& strList){
            swap(strList);
        }
        inline void onAddItem(QString& str){
            if(contains(str)){
                Q_ASSERT_X(wrongDataCounter > 0, "OnAddItem: wrongDataCounter == 0", "Item already in ChangeControl list");
                wrongDataCounter--; // File already on the list (That means wrongDataCounter should be > 0)
            }else{
                wrongDataCounter++;
            }
        }
        inline void onEditItem(QString& oldStr, QString& newStr){
            if(contains(oldStr)){   // OldStr is ok
                if(contains(newStr)){  // NewStr is ok

                }
            }else{ // OldStr is wrong

            }
        }
        inline void onRemoveItem(QString& str){
             contains(str);
        }
    } changeControl;*/

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
    void editFileButtonClicked(DefinitionsListElement*);
    void editFolderButtonClicked(DefinitionsListElement* );
    [0.9.0] End */

    /*inline void updateChangeControl(){
        QStringList strList;
        for(int itemIter = 0; itemIter < list.count(); itemIter++ ){
            strList.append(item(itemIter)->toolTip());
        }
        changeControl.updateAll(strList);
    }*/

    bool eventFilter(QObject* obj, QEvent* ev) override;
public:
    using DefinitionsListElement = DefinitionsListElement;

    DefinitionsList()
    {
        title.setText("Definicje");
        title.setAlignment(Qt::AlignCenter);
        /* [0.9.0]
        newFileButton.installEventFilter(this);
        newFileButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
        newFolderButton.installEventFilter(this);
        newFolderButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
        lastElementLayout.addWidget(&newFileButton, Qt::AlignRight);
        lastElementLayout.addWidget(&newFolderButton, Qt::AlignRight);

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

    inline void addElement(DefinitionsListElement* element)
    {
        list.insertItem(count(), element);
        element->initItemWidget(*this);
    }
    inline void setItemWidget(QListWidgetItem* lw,  QWidget* w){
        list.setItemWidget(lw, w);
    }
    inline DefinitionsListElement* item(int row)const{
        return static_cast<DefinitionsListElement*>(list.item(row));
    }
    inline int count()const{return list.count();}

    void newFile(QString );
    void newFolder(QString );

    inline DefinitionsListElement* currentItem()const{return static_cast<DefinitionsListElement*>(list.currentItem());}
    inline DefinitionsListElement* itemAt(const QPoint& p)const{return static_cast<DefinitionsListElement*>(list.itemAt(p));}

    inline void checkActive(){
        for(int itemIndex = 0; itemIndex < count(); itemIndex++)
        {
            item(itemIndex)->isActive();
        }
    }

};


#endif // DEFINITIONSLIST_HPP
