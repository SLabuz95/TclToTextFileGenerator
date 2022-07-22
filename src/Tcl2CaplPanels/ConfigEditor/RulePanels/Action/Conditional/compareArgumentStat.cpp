#include"compareArgumentStat.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using CompareArgumentStatActionView = ActionsList_NS::CompareArgumentStatActionView::CompareArgumentStatActionView;

// CompareArgumentStatActionView Action View Definitions -----------------------------------

CompareArgumentStatActionView::CompareArgumentStatActionView(QWidget* parent)
    : ActionDataView(parent)
{

}

CompareArgumentStatActionView::CompareArgumentStatActionView(QWidget* parent, ActionPtr action)
    : CompareArgumentStatActionView(parent)
{

}

void CompareArgumentStatActionView::readAction(ActionBase& ){
}

CompareArgumentStatActionView::DataView* CompareArgumentStatActionView::create(QWidget* parent, ActionRef action){
    return new CompareArgumentStatActionView(parent, action);
}

