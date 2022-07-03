#include"isLastSavedStat.hpp"
#include<QMouseEvent>
#include<QApplication>
#include<QMenu>
#include<QMessageBox>

namespace ActionsList_NS = Panels::Configuration::View::ActionsList;
using IsLastSavedStatActionView = ActionsList_NS::IsLastSavedStatActionView::IsLastSavedStatActionView;

// IsLastSavedStatActionView Action View Definitions -----------------------------------

IsLastSavedStatActionView::IsLastSavedStatActionView(ActionView& view)

{

}

IsLastSavedStatActionView::DataView* IsLastSavedStatActionView::create(ActionView& view, ActionRef){
    return new IsLastSavedStatActionView(view);
}

