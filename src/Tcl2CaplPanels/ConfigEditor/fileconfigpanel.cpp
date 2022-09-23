#include"fileconfigpanel.hpp"
#include<QFileInfo>
#include<QApplication>
#include<QStyle>
#include<QFileDialog>
#include"configEditor.hpp"

using namespace Panels::Configuration;

FileConfigPanel::FileConfigPanel(PanelRef configEditor, QString filePath)
    : configEditor_(configEditor)
{
    if(QFileInfo::exists(filePath)){
        filePathLineEdit.setText(filePath);
    }else{
        filePathLineEdit.setPlaceholderText("Brak");
    }
    filePathLineEdit.setReadOnly(true);
    openButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
    openButton.setToolTip("Otwórz");
    openButton.installEventFilter(this);
    saveButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    saveButton.setToolTip("Zapisz");
    saveButton.installEventFilter(this);
    saveAsButton.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    saveAsButton.setToolTip("Zapisz jako");
    saveAsButton.installEventFilter(this);

    layout.setSpacing(0);
    layout.setContentsMargins(0,0,0,0);
    layout.addWidget(&filePathLineEdit, Qt::AlignLeft);
    layout.addWidget(&openButton, Qt::AlignRight);
    openButton.setSizePolicy(QSizePolicy::Policy::Fixed, openButton.sizePolicy().verticalPolicy());
    layout.addWidget(&saveButton, Qt::AlignRight);
    saveButton.setSizePolicy(QSizePolicy::Policy::Fixed, saveButton.sizePolicy().verticalPolicy());
    layout.addWidget(&saveAsButton, Qt::AlignRight);
    saveAsButton.setSizePolicy(QSizePolicy::Policy::Fixed, saveAsButton.sizePolicy().verticalPolicy());

    mainLayout.setSpacing(0);
    //mainLayout.setContentsMargins(0,0,0,0);
    mainLayout.addRow("Plik konfiguracyjny: ", &layout);
    setLayout(&mainLayout);
}


bool FileConfigPanel::eventFilter(QObject *obj, QEvent *ev){
    switch(ev->type()){
    case QEvent::MouseButtonPress:
    {
        if(obj == &openButton){
            openButtonPressed();
        }else
        if(obj == &saveButton){
            saveButtonPressed();
        }else
        if(obj == &saveAsButton){
            saveAsButtonPressed();
        }
    }
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, ev);
}

void FileConfigPanel::newButtonPressed(){
    // Request new config in configEditor
    // if success, clear text in filePathLineEdit (Placeholder)
    // If failed, do nothing
    if(configEditor_.request_newConfig() == true){
        filePathLineEdit.clear();
    }
}

void FileConfigPanel::openButtonPressed(){
    QString oldPath = filePathStr();
     //get config file by dialog for config files (*.xml)
     QString filePath = QFileDialog::getOpenFileName(nullptr, QString("Wybierz plik konfiguracyjny:"), QString(), "*.xml");

     // if not empty
     if(not filePath.isEmpty()){
         // Request change of config in configEditor
         // If success, change path in filePathLineEdit
         // If failed, do nothing
         filePathLineEdit.setText(filePath);
         if(configEditor_.request_readConfig() == false){
             filePathLineEdit.setText(oldPath);
         }
     }
}

void FileConfigPanel::saveButtonPressed(){

    if(filePathStr().isEmpty()){    // Local Config
        saveAsButtonPressed();
    }else{
       if(configEditor_.request_saveConfig() == true){
           // Do nothing
       }
    }

}

void FileConfigPanel::saveAsButtonPressed(){
    QString oldPath = filePathStr();
    //get save file location
    QString filePath = QFileDialog::getSaveFileName(nullptr, QString("Zapisz plik konfiguracyjny:"), QString(), "*.xml");

    // if not empty,
    if(not filePath.isEmpty()){
        // request save file for the path
        // if success, change path in filePathLineEdit
        filePathLineEdit.setText(filePath);
        if(configEditor_.request_saveAsConfig(filePath) == false){
            filePathLineEdit.setText(oldPath);
        }

    }
}
