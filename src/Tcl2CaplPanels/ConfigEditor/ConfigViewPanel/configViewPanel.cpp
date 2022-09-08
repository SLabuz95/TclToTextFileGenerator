#include"configViewPanel.hpp"
#include "Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include<QSplitterHandle>
#include<QPainter>
#include<QEnterEvent>
#include"Tcl2CaplPanels/ConfigEditor/ConfigTabsPanel/Panels/attributesPanel.hpp"
#include"Tcl2CaplPanels/ConfigEditor/writeOnlyProceduresList.hpp"

using namespace Panels::Configuration::View;

ConfigViewPanel::ConfigViewPanel(ConfigEditor& parent)
 : configEditor(parent),
   Panels(*this)
{
    addWidget(&this->Panels::Super::ViewPanel::get());
    addWidget(&this->Panels::Super::ViewPanel::Super::get());
    show();
    moveSplitter(0,1);
}

void ConfigViewPanel::loadConfigData(QStringList& writeOnlyProcedures, ControllerConfigInfo::ProceduresView& proceduresView)
{
    NavigationList& navigationPanel = this->Panels::Super::ViewPanel::get();
    ConfigTabs& configTabsPanel = this->Panels::Super::ViewPanel::Super::get();
    navigationPanel.loadData(proceduresView);
    //configTabsPanel.attributesList().loadAttributes(&configEditor.getConfigInfoPtr()->controllerConfig().attributes(), settings);
    //configTabsPanel.writeOnlyProceduresList().loadProcedures(&configEditor.getConfigInfoPtr()->controllerConfig().writeOnlyProcedures(), settings);
    configTabsPanel.writeOnlyProceduresList().loadProcedures(writeOnlyProcedures);
    //clearProcedureRulesPanel();

}

void ConfigViewPanelSplitter::Splitter::Handle::paintEvent(QPaintEvent *event)
{
    if(not isNotActive())
        return QSplitterHandle::paintEvent(event);

    QPainter p(this);
    if(textMode){
        p.translate(event->rect().bottomLeft());
        p.rotate(-90);
        p.drawText(event->rect().transposed(), QString("Nawigacja"), QTextOption(Qt::AlignCenter));
    }else{
        const QRect& evRect = event->rect();
        p.setBrush(QBrush(Qt::GlobalColor::black));
        p.drawConvexPolygon(
                    QPolygonF(
                        {
                            QPointF(NonTextModeHandelMargin ,(evRect.height() - NonTextModeHandelTriangleWidth)* 0.5),
                            QPointF(NonTextModeHandelMargin ,(evRect.height()  + NonTextModeHandelTriangleWidth) * 0.5),
                            QPointF(NonTextModeHandelWidth - NonTextModeHandelMargin ,evRect.height() * 0.5)
                        }
                        )
                    );
    }
}

void ConfigViewPanelSplitter::Splitter::Handle::enterEvent(QEnterEvent *event){
    textMode = true;
    if(isNotActive()){
        if(splitter())
            splitter()->setHandleWidth(TextModeHandelWidth);
        update();
    }else{
        QSplitterHandle::enterEvent(event);
    }
}

void ConfigViewPanelSplitter::Splitter::Handle::leaveEvent(QEvent *event){
    textMode = false;
    if(isNotActive()){
        if(splitter())
            splitter()->setHandleWidth(NonTextModeHandelWidth);
        update();
    }else{
        QSplitterHandle::leaveEvent(event);
    }
}

