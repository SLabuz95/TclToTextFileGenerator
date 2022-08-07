#ifndef FILECONFIGPANEL_HPP
#define FILECONFIGPANEL_HPP

#include<QWidget>
#include<QFormLayout>
#include<QLineEdit>
#include<QPushButton>

class QFileInfo;
namespace Panels::Configuration{
    class Panel;
}
namespace Panels::Configuration{
    class FileConfigPanel : public QWidget{

        using PanelRef = Panel&;
        using MainLayout = QFormLayout;
        using Layout = QHBoxLayout;
        using FilePathLineEdit = QLineEdit;
        using OpenButton = QPushButton;
        using SaveButton = QPushButton;
        using SaveAsButton = QPushButton;

        PanelRef configEditor_;
        MainLayout mainLayout;
        Layout layout;
        FilePathLineEdit filePathLineEdit;
        OpenButton openButton;
        SaveButton saveButton;
        SaveAsButton saveAsButton;

    protected:
        bool eventFilter(QObject* obj, QEvent* ev)override;

    public:
        FileConfigPanel(PanelRef, QString = QString());

        inline QString filePathStr(){return filePathLineEdit.text();}
        void changeFilePath(QString newPath){filePathLineEdit.setText(newPath);}

        void newButtonPressed();
        void openButtonPressed();
        void saveButtonPressed();
        void saveAsButtonPressed();
    };
}
#endif // FILECONFIGPANEL_HPP
