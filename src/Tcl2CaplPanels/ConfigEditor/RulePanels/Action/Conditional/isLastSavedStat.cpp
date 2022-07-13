#include"isLastSavedStat.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using IsLastSavedStatActionView = ActionsList_NS::IsLastSavedStatActionView::IsLastSavedStatActionView;

// IsLastSavedStatActionView Action View Definitions -----------------------------------

IsLastSavedStatActionView::IsLastSavedStatActionView(QWidget* parent)
    : ActionDataView(parent)
{

}

IsLastSavedStatActionView::IsLastSavedStatActionView(QWidget* parent, ActionPtr action)
    : IsLastSavedStatActionView(parent)
{

}

void IsLastSavedStatActionView::readAction(ActionBase& ){
}

IsLastSavedStatActionView::DataView* IsLastSavedStatActionView::create(QWidget* parent, ActionRef action){
    return new IsLastSavedStatActionView(parent, action);
}

