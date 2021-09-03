#ifndef FILECONFIGPANEL_HPP
#define FILECONFIGPANEL_HPP

#include<QWidget>
#include<QFormLayout>
#include<QLineEdit>
#include<QPushButton>

class QFileInfo;
class ConfigEditor;
class FileConfigPanel : public QWidget{

    using ConfigEditorRef = ConfigEditor&;
    using MainLayout = QFormLayout;
    using Layout = QHBoxLayout;
    using FilePathLineEdit = QLineEdit;
    using OpenButton = QPushButton;
    using SaveButton = QPushButton;
    using SaveAsButton = QPushButton;

    ConfigEditorRef configEditor_;
    MainLayout mainLayout;
    Layout layout;
    FilePathLineEdit filePathLineEdit;
    OpenButton openButton;
    SaveButton saveButton;
    SaveAsButton saveAsButton;

protected:
    bool eventFilter(QObject* obj, QEvent* ev)override;

public:
    FileConfigPanel(ConfigEditorRef, QString = QString());

    inline QString filePathStr(){return filePathLineEdit.text();}

    void newButtonPressed();
    void openButtonPressed();
    void saveButtonPressed();
    void saveAsButtonPressed();
};

#endif // FILECONFIGPANEL_HPP
