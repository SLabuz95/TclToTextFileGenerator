#ifndef FILESLISTROOTSELECTION_HPP
#define FILESLISTROOTSELECTION_HPP

#include<QWidget>
#include<QLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QHeaderView>

class FilesList;
class FilesListRootSelection : public QHeaderView
{
public:
    FilesListRootSelection(FilesList&);

protected:
    FilesList& filesList;
    QWidget centralWidget;
    QHBoxLayout centralLayout;
    QLineEdit rootPath;
    QString rootPathStr_;
    QPushButton selectButton;

    bool eventFilter(QObject* obj, QEvent* ev) override;
    //void showEvent(QShowEvent* ev) override;
    void resizeEvent(QResizeEvent* ev)override;

    void chooseRootPath();
    void setRootPath(QString);

public:
    inline QString rootPathStr()const{return rootPathStr_;}
};

#endif // FILESLISTROOTSELECTION_HPP
