#include"addUserInteraction.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using AddUserInteractionActionView = ActionsList_NS::AddUserInteractionActionView;
using ActionView = AddUserInteractionActionView::ActionView;

// AddUserInteractionActionView Action View Definitions -----------------------------------

AddUserInteractionActionView::AddUserInteractionActionView(ActionView& view)
{
    splitter.addWidget(&formattedStringList);
    addRow(&splitter);
}

AddUserInteractionActionView::DataView* AddUserInteractionActionView::create(ActionView& view, ActionRef){
    return new AddUserInteractionActionView(view);
}


