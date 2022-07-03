#include"tclParse.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using TclParseActionView = ActionsList_NS::TclParseActionView;
using ActionView = TclParseActionView::ActionView;

// TclParseActionView Action View Definitions -----------------------------------

TclParseActionView::TclParseActionView(ActionView& view)
{
    splitter.addWidget(&formattedStringList);
    addWidget(&splitter);
}

TclParseActionView::DataView* TclParseActionView::create(ActionView& view, ActionRef){
    return new TclParseActionView(view);
}


