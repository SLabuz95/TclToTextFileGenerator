#include "navigationList.hpp"
#include "Tcl2CaplPanels/ConfigEditor/ConfigViewPanel/configViewPanel.hpp"
#include "Tcl2CaplPanels/ConfigEditor/ConfigTabsPanel/configTabsPanel.hpp"
#include "Elements/procedureElement.hpp"
#include "Elements/phaseElement.hpp"
#include<QMouseEvent>
#include<QMessageBox>
#include<QResizeEvent>
#include"Tcl2CaplPanels/General/multilineEditorDialog.hpp"
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"

using namespace Panels::Configuration::Navigation;
using ConfigEditor = Panels::Configuration::Panel;
using ConfigViewPanel = Panels::Configuration::View::ConfigViewPanel;
using PhasesElement = Phase::PhasesElement;
using PhaseElement = Phase::PhaseElement;
using ProceduresElement = Procedure::ProceduresElement;
using DefaultProcedureElement = Procedure::DefaultProcedureElement;

const QString List::navigationPanelNames[panelType2number(PanelType::Size)]{
            QString("Atrybuty"),
            QString("Procedury - tryb raportowy"),
            QString("Procedury"),
            QString("Procedura domyślna"),
            QString("Fazy")
            //QString("Priorytety"),
};

List::List(View::ConfigViewPanel& parent)
    : QTreeWidget(&parent), configPanel_(parent)
{
    ItemDelegate* itemDelegateObj = new ItemDelegate(*this);
    setItemDelegate(itemDelegateObj);
    setHeaderHidden(true);
    setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setSelectionMode(SelectionMode::NoSelection);
    //setIndentation(0);

    QList<QTreeWidgetItem*> items(panelType2number(PanelType::Size), nullptr);
    for(decltype(items)::Iterator item = items.begin(); item < items.end(); item++){
        switch(number2panelType(item - items.begin())){
        case PanelType::Phases:
        {
            *item = new PhasesElement(this, {navigationPanelNames[item - items.begin()]});
        }
            break;
        case PanelType::Procedures:
        {
            *item = new ProceduresElement(this, {navigationPanelNames[item - items.begin()]});
        }
            break;
        case PanelType::DefaultProcedure:
        {
            *item = new DefaultProcedureElement(this, {navigationPanelNames[item - items.begin()]});
        }
            break;
        default:
            *item = new NavigationElement(this, {navigationPanelNames[item - items.begin()]});
        }

        (*item)->setFirstColumnSpanned(true);

    }

    addTopLevelItems(items);
    viewport()->installEventFilter(this);
    installEventFilter(this);
}


QWidget* List::ItemDelegate::
createEditor(QWidget* parent,
             const QStyleOptionViewItem& option,
             const QModelIndex& index) const
{

    NavigationElement* item = list.itemFromIndex(index);
    PanelType panelType = PanelType();
    if(item->parent() != nullptr){
        // otherwise:
        // if parent is mainNavigationElement -> check index of parent to define panelType
        panelType = static_cast<PanelType>(list.indexOfTopLevelItem(item->parent()));
        switch(panelType){
        case PanelType::NoFound: // Not MainNavigationPanel -> Child of ProcedureElement of ProceduresElement
        {
            // Interpret like DefaultProcedure child
            panelType = PanelType::Procedures;
        }
        Q_FALLTHROUGH();
        case PanelType::DefaultProcedure: // Indexes if index of child < 2 are not editable (0 and 1 are fixed categories)
        {
            QLineEdit* editor = nullptr;
            editor = new QLineEdit(parent);            
            editor->setValidator(new QRegularExpressionValidator(QRegularExpression(RegExpCore::regExpForUint)));
            return editor;
        }
            break;
        case PanelType::Procedures: // All are editable
        case PanelType::Phases:
        default:
            break;
        }
    }
    return QItemDelegate::createEditor(parent, option, index);
}


void List::ItemDelegate::
setEditorData(QWidget* editor,
              const QModelIndex& index) const
{
   QItemDelegate::setEditorData(editor, index);
}

void List::ItemDelegate::
setModelData(QWidget* editor,
             QAbstractItemModel* model,
             const QModelIndex& index) const
{
   QItemDelegate::setModelData(editor, model, index);
}

void List::ItemDelegate::
updateEditorGeometry(QWidget* editor,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) const
{
   QItemDelegate::updateEditorGeometry(editor, option, index);
}


bool List::eventFilter(QObject* obj, QEvent* ev){
    if(ev->type() == QEvent::MouseButtonDblClick and obj == viewport()){
        QMouseEvent* mev = static_cast<QMouseEvent*>(ev);
        QTreeWidgetItem* item = itemAt(mev->pos());
        if(item){
            NavigationElement* nitem = static_cast<NavigationElement*>(item);
            const int indexOfMainNavigationElement = indexOf(nitem);
            if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                // Ask about parent of child item
                // it can only be child of MainNavigation panel:
                // - child of DefaultProcedure element (Category of Rules element),
                // - child of Procedures element (Procedure name element)
                // - child of Phases element (Phase name element)
                // OR
                // - child of Procedure element (Category of Rules element)
                // - child of Phase element (Category of Rules element - all are not edittable)
                const PanelType& panelType = static_cast<PanelType>(indexOf(nitem->parent()));
                switch(panelType){
                case PanelType::Procedures:
                case PanelType::Phases:
                {
                    // No navigation
                    navigateMainPanel(panelType);
                }
                    break;

                case PanelType::NoFound:
                    // - child of Procedure element (Category of Rules element)
                    // - child of Phase element (Category of Rules element - all are not edittable)
                {
                    NavigationElement* ncpitem = static_cast<NavigationElement*>(item->parent()); // Navigation Parent Item (Procedure or Phase)
                    const PanelType& panelType = static_cast<PanelType>(indexOf(ncpitem->parent()));
                    switch(panelType){
                    case PanelType::Procedures:
                    {
                        DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(item->parent());
                        int index = dPE.indexOfChild(item);
                        if(index < 2) {
                            // First 2 categories (index 0 and 1) are FIXED - act like MainNavigationElement clicked (OnEndOfCall oraz OnArgument)
                            configEditor().loadRules(dPE.text(0), QString::number(index - 2));
                        }else{
                            configEditor().loadRules(dPE.text(0), item->text(0));
                        }
                        activateProcedureCategory(nitem);
                        navigateMainPanel(PanelType::Procedures);
                    }
                        break;
                    case PanelType::Phases:
                    {
                        PhaseElement& dPE = *static_cast<PhaseElement*>(item->parent());
                        int index = dPE.indexOfChild(item);

                        // First 3 categories (index 0 1 and 2) are FIXED - act like MainNavigationElement clicked
                        configEditor().loadModifierRules(dPE.text(0), QString::number(index - 2));
                        activatePhase(nitem);
                        navigateMainPanel(PanelType::Phases);
                    }
                        break;
                    default:
                        break;
                    }
                }
                    break;
                case PanelType::DefaultProcedure:
                {
                    DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(item->parent());
                    int index = dPE.indexOfChild(item);
                    if(index < 2) {
                        // First 2 categories (index 0 and 1) are FIXED - act like MainNavigationElement clicked (OnEndOfCall oraz OnArgument)
                        configEditor().loadDefaultRules(QString::number(index - 2));
                    }else{
                        configEditor().loadDefaultRules(item->text(0));
                    }
                    activateDefaultProcedureCategory(nitem);
                    navigateMainPanel(panelType);
                }
                    break;
                default:
                    break;
                }

            }else{// Main navigation panel clicked
                const PanelType& panelType = static_cast<PanelType>(indexOfMainNavigationElement);
                switch(panelType){

                case PanelType::Procedures:
                case PanelType::DefaultProcedure:
                case PanelType::Phases:
                    break;
                default:
                    navigateMainPanel(panelType);
                }
                //navigateMainPanel(number2panelType(indexOfMainNavigationElement));
            }
        }
    }
    //  Hide / unhide
    switch(ev->type()){
    case QEvent::Resize:
    {
        if(obj == this){
            QResizeEvent& resizeEvent = *static_cast<QResizeEvent*>(ev);

            if(resizeEvent.oldSize().width() == 0){
                if( resizeEvent.size().width() != 0){   // Appear Condition
                    configPanel_.navigationListAppeared();
                }
            }else{
                if(resizeEvent.size().width() == 0){ // Disappear Condition
                    configPanel_.navigationListDisappeared();
                }
            }
        }
    }
        break;
    case QEvent::ContextMenu:
    {
        if(obj == viewport()){
            QContextMenuEvent* cev = static_cast<QContextMenuEvent*>(ev);
            QTreeWidgetItem* item = itemAt(cev->pos());
            if(item){
                NavigationElement* nitem = static_cast<NavigationElement*>(item);
                const int indexOfMainNavigationElement = indexOf(nitem);
                if(indexOfMainNavigationElement == -1){ // Not found -> Item is child of main navigation element
                    // Ask about parent of child item
                    // it can only be child of MainNavigation panel:
                    // - child of DefaultProcedure element (Category of Rules element),
                    // - child of Procedures element (Procedure name element)
                    // - child of Phases element (Phase name element)
                    // OR
                    // - child of Procedure element (Category of Rules element)
                    // - child of Phase element (Category of Rules element - all are not edittable)
                    const PanelType& panelType = static_cast<PanelType>(indexOf(nitem->parent()));
                    switch(panelType){
                    case PanelType::Procedures:
                    {
                        ProceduresElement& dPE = *static_cast<ProceduresElement*>(item->parent());
                        dPE.menuControl(cev, static_cast<ProceduresElement::ListItem*>(item));
                    }
                        break;
                    case PanelType::Phases:
                    {                        
                        PhasesElement& dPE = *static_cast<PhasesElement*>(item->parent());
                        dPE.menuControl(cev, static_cast<PhasesElement::ListItem*>(item)); // Simulate no item choosed
                    }
                        break;
                    case PanelType::NoFound:
                    {
                        // - child of Procedure element (Category of Rules element)
                        bool nextCase = false;
                        NavigationElement* ncpitem = static_cast<NavigationElement*>(item->parent()); // Navigation Parent Item (Procedure or Phase)
                        const PanelType& panelType = static_cast<PanelType>(indexOf(ncpitem->parent()));

                        switch(panelType){
                        case PanelType::Procedures:{
                            // Continue to next case
                            nextCase = true;
                        }
                            break;
                        case PanelType::Phases:
                        {
                            PhaseElement& dPE = *static_cast<PhaseElement*>(item->parent());

                            // First 3 categories (index 0 1 and 2) are FIXED - act like MainNavigationElement clicked
                            dPE.menuControl(cev, nullptr); // Simulate no item choosed
                        }
                            break;
                        default:
                            break;
                        }
                        if(not nextCase) // CONDITIONAL FALLTHROUGH
                            break;
                    }
                        Q_FALLTHROUGH();
                    case PanelType::DefaultProcedure:
                    {
                        DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(item->parent());
                        if(dPE.indexOfChild(item) < 2) {
                            // First 2 categories (index 0 and 1) are FIXED - act like MainNavigationElement clicked (OnEndOfCall oraz OnArgument)
                            dPE.menuControl(cev, nullptr); // Simulate no item choosed
                        }else{
                            dPE.menuControl(cev, static_cast<DefaultProcedureElement::ListItem*>(item));
                        }
                    }
                        break;
                    default:
                        break;
                    }

                }else{// Main navigation panel clicked
                    const PanelType& panelType = static_cast<PanelType>(indexOfMainNavigationElement);
                    switch(panelType){
                    case PanelType::Phases:
                    {
                        PhasesElement& dPE = *static_cast<PhasesElement*>(nitem);
                        dPE.menuControl(cev, nullptr); // Simulate no item choosed
                    }
                        break;
                    case PanelType::Procedures:
                    {
                        ProceduresElement& dPE = *static_cast<ProceduresElement*>(nitem);
                        dPE.menuControl(cev, nullptr); // Simulate no item choosed
                    }
                        break;
                    case PanelType::DefaultProcedure:
                    {
                        DefaultProcedureElement& dPE = *static_cast<DefaultProcedureElement*>(nitem);
                        dPE.menuControl(cev, nullptr); // Simulate no item choosed
                    }
                        break;
                    default:
                        break;
                    }
                    //navigateMainPanel(number2panelType(indexOfMainNavigationElement));
                }
            }
        }

    }
        break;
    default:
        break;
    }
    return Super::eventFilter(obj, ev);
}


void List::loadData(ControllerConfigInfo::PhasesView& phasesView, ControllerConfigInfo::ProceduresView& proceduresView){
    PhasesElement& pE = *static_cast<PhasesElement*>(topLevelItem(panelType2number(PanelType::Phases)));
    closePersistentEditor();
    deactivatePhase();
    while(pE.childCount() != 1){
        delete pE.child(1); // Remove First index
    }

    ProceduresElement& dPE = *static_cast<ProceduresElement*>(topLevelItem(panelType2number(PanelType::Procedures)));
    closePersistentEditor();
    deactivateProcedureCategory();
    auto children = dPE.takeChildren();
    for(auto child = children.begin(); child < children.end(); child++)
        delete *child;

    DefaultProcedureElement& defPE = *static_cast<DefaultProcedureElement*>(topLevelItem(panelType2number(PanelType::DefaultProcedure)));
    deactivateDefaultProcedureCategory();
    while(defPE.childCount() != 2){
        delete defPE.child(2); // Remove First index
    }


    using PhaseMap = ControllerConfigInfo::PhasesView;
    using ConfigMap = ControllerConfigInfo::ProceduresView::first_type;
    using DefaultConfigMap = ControllerConfigInfo::ProceduresView::second_type;

    {
        using Config = PhaseMap;
        using ConfigKeyIter = Config::Iterator;

        QString name;
        // Prepare containers
        // On end of Call
        Config& newPhasesMap = phasesView;
        ConfigKeyIter configPhaseStartIter = newPhasesMap.begin();
        ConfigKeyIter configMapIter = configPhaseStartIter;

        while(configMapIter != newPhasesMap.end()){
            configPhaseStartIter = configMapIter;
            name = configMapIter.key().first;
            if(configMapIter.value() >= 0 and name != "Default"){ // Check if Exists or not name == "Default"
                // Write Phase element
                PhasesElement::ListItem* item = new PhasesElement::ListItem();
                pE.addChild(item);
                item->setText(0, name);

                configMapIter = configPhaseStartIter; // On End
                configMapIter++;
                configMapIter++;
                while(configMapIter != newPhasesMap.end() and configMapIter.key().second != ControllerConfigInfo::ModifierRulesCategories::LowestValue){
                    configMapIter++;
                }
            }else{ // Not exist - move to next procedure
                configMapIter++;
                while(configMapIter != newPhasesMap.end() and configMapIter.key().second != ControllerConfigInfo::ModifierRulesCategories::LowestValue){
                    configMapIter++;
                }
                // Do not increment again
            }
        }
    }
    {
        using Config = ConfigMap;
        using ConfigKeyIter = Config::Iterator;

        QString name;
        // Prepare containers
        // On end of Call
        Config& newProceduresMap = proceduresView.first;
        ConfigKeyIter configProcedureStartIter = newProceduresMap.begin();
        ConfigKeyIter configMapIter = configProcedureStartIter;

        while(configMapIter != newProceduresMap.end()){
            configProcedureStartIter = configMapIter;
            name = configMapIter.key().first;
            if(configMapIter.value() >= 0){ // Check if Exists
                // Write Procedure element
                ProceduresElement::ListItem* item = new ProceduresElement::ListItem();
                dPE.addChild(item);
                item->setText(0, name);

                configMapIter = configProcedureStartIter; // On End
                configMapIter++; // Undefined
                configMapIter++; // First Index
                while(configMapIter != newProceduresMap.end() and configMapIter.key().second != ControllerConfigInfo::RulesCategories::OnEndOfCall){
                    if(configMapIter.value() >= 0) // Index exists
                    {
                        // Set Index
                        ProceduresElement::ListItem::ChildType* indexItem = new ProceduresElement::ListItem::ChildType();
                        item->addChild(indexItem);
                        indexItem->setText(0, QString::number(configMapIter.key().second));
                    }
                    configMapIter++;
                }
            }else{ // Not exist - move to next procedure
                configMapIter++; // Undefined
                while(configMapIter != newProceduresMap.end() and configMapIter.key().second != ControllerConfigInfo::RulesCategories::OnEndOfCall){
                    configMapIter++;
                }
                // Do not increment again
            }
        }
    }
    {
        using Config = DefaultConfigMap;
        using ConfigKeyIter = Config::Iterator;

        QString name;
        // Prepare containers
        // On end of Call
        Config& newProceduresMap = proceduresView.second;
        ConfigKeyIter configProcedureStartIter = newProceduresMap.begin();
        ConfigKeyIter configMapIter = configProcedureStartIter;

        configMapIter = configProcedureStartIter; // On End
        configMapIter++; // Undefined
        configMapIter++; // First Index
        while(configMapIter != newProceduresMap.end()){
            if(configMapIter.value() >= 0) // Index exists
            {
                // Set Index
                DefaultProcedureElement::ListItem* indexItem = new DefaultProcedureElement::ListItem();
                defPE.addChild(indexItem);
                indexItem->setText(0, QString::number(configMapIter.key()));
            }
            configMapIter++;
        }
    }

}


bool List::edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event){
    using Trigger = QAbstractItemView::EditTrigger;
    //CurEditItemInfo tempEditItem = curEditItemInfo;
    if(index.isValid()){
        NavigationElement* item = itemFromIndex(index);
        PanelType panelType = PanelType();
        switch(trigger){
        case Trigger::AllEditTriggers:
        {
            // if no parent (== nullptr), mainNavigationElement - no editing
            if(item->parent() == nullptr){
                return false;
            }else{
                // otherwise:
                // if parent is mainNavigationElement -> check index of parent to define panelType
                panelType = static_cast<PanelType>(indexOfTopLevelItem(item->parent()));
                switch(panelType){
                case PanelType::NoFound: // Not MainNavigationPanel -> Child of ProcedureElement of ProceduresElement
                {
                    bool breakFlag = true;
                    NavigationElement* nitem = item;
                    while(nitem->parent() != nullptr){
                        nitem = nitem->parent();
                    }
                    panelType = static_cast<PanelType>(indexOfTopLevelItem(nitem));
                    switch(panelType){
                    case PanelType::Procedures: // Not MainNavigationPanel -> Child of ProcedureElement of ProceduresElement
                    {
                        // Interpret like DefaultProcedure child
                        panelType = PanelType::Procedures;
                        breakFlag = false;
                    }
                        break;
                    default:
                        break; // NO FALLTHROUGH
                    }
                    if(breakFlag)
                        break;
                }
                Q_FALLTHROUGH();

                case PanelType::DefaultProcedure: // Indexes if index of child < 2 are not editable (0 and 1 are fixed categories)
                {
                    if(item->parent()->indexOfChild(item) < 2)
                        return false;
                }
                    break;
                case PanelType::Phases:
                {
                    if(item->parent()->indexOfChild(item) == 0) // Default Phase
                        return false;
                }
                    break;
                case PanelType::Procedures: // All are editable
                    break;
                default:
                    return false; // Other children of other Panel - not editable
                }
            }
        }
            break;
        case Trigger::DoubleClicked:
        {
            // if no parent (== nullptr), mainNavigationElement - no editing
            if(item->parent() == nullptr){
                return false;
            }else{
                // otherwise:
                // if parent is mainNavigationElement -> check index of parent to define panelType
                panelType = static_cast<PanelType>(indexOfTopLevelItem(item->parent()));
                switch(panelType){
                case PanelType::Procedures: // All are editable
                case PanelType::Phases:
                    break;
                case PanelType::NoFound: // Not MainNavigationPanel -> Child of ProcedureElement of ProceduresElement
                {
                    // Navigate to category - not editable
                }
                case PanelType::DefaultProcedure: // Indexes if index of child < 2 are not editable (0 and 1 are fixed categories)
                {
                    // Navigate to category - not editable
                }
                default:
                    return false; // Other children of other Panel - not editable
                }
            }

        }

            break;
        default:
            break;
        }
        curEditItemInfo.panelType = panelType;
        curEditItemInfo.item = item;
        curEditItemInfo.oldStr = curEditItemInfo.item->text(0);
        if(((trigger == Trigger::DoubleClicked or trigger == Trigger::AllEditTriggers)
                and curEditItemInfo.oldStr.contains("\n")) or requestMultiLineEditorAccess == true){ // If multiline
            requestMultiLineEditorAccess = false;
            QString str = curEditItemInfo.oldStr;
            MultiLineEditor editor(str);
            switch(editor.exec()){
            case QDialog::Accepted:
                curEditItemInfo.item->setText(0, editor.text());
                processEditData(curEditItemInfo);
                break;
            case QDialog::Rejected:
                curEditItemInfo = {};
                break;
            }
            return false;
        }

    }
    return Super::edit(index, trigger, event);
}

void List::closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint){
    using Hint = QAbstractItemDelegate::EndEditHint;
    CurEditItemInfo tempInfo = curEditItemInfo;
    Super::closeEditor(editor, hint);
    switch(hint){
    case Hint::EditNextItem:
    case Hint::EditPreviousItem:
    {
        if(curEditItemInfo.item != tempInfo.item){
            processEditData(tempInfo);
        }
    }
        break;
    default:
        processEditData(curEditItemInfo);
    }
}
void List::closePersistentEditor(){
    if(curEditItemInfo.item){
        Super::closePersistentEditor(curEditItemInfo.item);
        qApp->processEvents();
    }
}
void List::editItem(NavigationElement* item){
    curEditItemInfo = {
        item,
        PanelType::Phases,
        item->text(0)
    };
    if(item->parent()->treeWidget()->isDefaultProcedurePanel(item)){
        curEditItemInfo.panelType = PanelType::DefaultProcedure;
    }else{
        if(item->parent()->treeWidget()->isProcedurePanel(item)){
            curEditItemInfo.panelType = PanelType::Procedures;
        }
    }

    Super::editItem(item, 0);
}

void List::processEditData(CurEditItemInfo& curEditItemInfo)
{
    if(curEditItemInfo.item){
        switch(curEditItemInfo.panelType){
        case PanelType::DefaultProcedure: // Only Indexes
        {
            if(curEditItemInfo.item->text(0).isEmpty()){ // Remove or Restore
                if(curEditItemInfo.oldStr.isEmpty()){ // Remove only if empty
                    delete curEditItemInfo.item;
                }else{
                    curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                }
            }else{ // Not empty (New or Change)
                bool ok;
                qsizetype newIndex = curEditItemInfo.item->text(0).toULongLong(&ok);
                if(curEditItemInfo.oldStr.isEmpty()){    // new
                    // Manage New - if manage failed Duplicated for example, remove
                    if(not ok or config().addIndex(newIndex) == false){
                        delete curEditItemInfo.item;
                    }/*else{
                        activateDefaultProcedureCategory(static_cast<NavigationElement*>(curEditItemInfo.item));
                        configPanel.configEditor.loadDefaultRules(curEditItemInfo.item->text(0));
                    }*/
                }else{
                    // Manage Change - if manage failed Duplicated for example, restore
                    // Is new index - valid
                    if(curEditItemInfo.oldStr != curEditItemInfo.item->text(0)){
                        if(not ok or configEditor().editIndex(curEditItemInfo.oldStr.toULongLong(), newIndex) == false){
                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                        }
                    }
                }
            }
        }
            break;
        case PanelType::Procedures:
        {
            using TclProcedure = ControllerConfigInfo::Config::UserProcedure;
            // if Procedure Element (parent == topLevelItem(Panel::Procedures))
            if(curEditItemInfo.item->parent() == topLevelItem(panelType2number(PanelType::Procedures))){
                // Prepare TclProcedureName - namespace control etc
                curEditItemInfo.item->setText(0, TclProcedure::prepareTclProcedureNameFromStr(curEditItemInfo.item->text(0)));
                 // Can be empty or not (New or Change)
                if(curEditItemInfo.oldStr.isEmpty()){    // new
                    if(curEditItemInfo.item->text(0).isEmpty()){
                       if(config().addProcedure(curEditItemInfo.item->text(0)) == false){
                            NavigationElement* parentItem = curEditItemInfo.item->parent();
                            int itemIndex = 0;
                            for( ; itemIndex < parentItem->childCount(); itemIndex++){
                                if(parentItem->child(itemIndex) != curEditItemInfo.item and
                                         parentItem->child(itemIndex)->text(0) == curEditItemInfo.oldStr)
                                { // Duplicated for sure
                                    delete curEditItemInfo.item;
                                    break;
                                }
                            }
                            if(itemIndex == parentItem->childCount()){ // Not found, not duplicated ->
                                // Do nothing -  nothing has changed
                            }
                        }
                    }else{
                        if(config().isProcedureExist("")){ // Check if empty procedure exists
                            NavigationElement* parentItem = curEditItemInfo.item->parent();
                            int itemIndex = 0;
                            for( ; itemIndex < parentItem->childCount(); itemIndex++){
                                if(parentItem->child(itemIndex) != curEditItemInfo.item and
                                         parentItem->child(itemIndex)->text(0) == curEditItemInfo.oldStr)
                                { // New element
                                    if(config().addProcedure(curEditItemInfo.item->text(0)) == false){
                                        delete curEditItemInfo.item;
                                    }
                                    break;
                                }
                            }
                            if(itemIndex == parentItem->childCount()){ // Not found - empty procedure is current element
                                if(configEditor().editProcedure(curEditItemInfo.oldStr, curEditItemInfo.item->text(0))  == false){
                                    curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                                }
                            }
                        }else{
                            // Prepare TclProcedureName - namespace control etc
                            curEditItemInfo.item->setText(0, TclProcedure::prepareTclProcedureNameFromStr(curEditItemInfo.item->text(0)));
                            if(config().addProcedure(curEditItemInfo.item->text(0)) == false){
                                delete curEditItemInfo.item;
                            }
                        }
                    }                    
                }else{

                    if(curEditItemInfo.item->text(0) != curEditItemInfo.oldStr){    // changed
                        // Manage Change - if manage failed Duplicated for example, restore
                        if(configEditor().editProcedure(curEditItemInfo.oldStr, curEditItemInfo.item->text(0))  == false){
                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                        }
                    }
                }
            }
            // Otherwise - Index/category of procedure
            else{
                if(curEditItemInfo.item->text(0).isEmpty()){ // Remove or Restore
                    if(curEditItemInfo.oldStr.isEmpty()){ // Remove only if empty
                        delete curEditItemInfo.item;
                    }else{
                        curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                    }
                }else{ // Not empty (New or Change )
                    bool ok;
                    qsizetype newIndex = curEditItemInfo.item->text(0).toULongLong(&ok);
                    if(curEditItemInfo.oldStr.isEmpty()){    // new
                        // Manage New - if manage failed Duplicated for example, remove
                        if(not ok or config().addIndex(curEditItemInfo.item->parent()->text(0) , newIndex) == false){
                            delete curEditItemInfo.item;
                        }else{
                            activateProcedureCategory(static_cast<NavigationElement*>(curEditItemInfo.item));
                            configEditor().loadRules(curEditItemInfo.item->parent()->text(0) ,curEditItemInfo.item->text(0));
                        }
                    }else{
                        // Manage Change - if manage failed Duplicated for example, restore
                        if(not ok or configEditor().editIndex(curEditItemInfo.item->parent()->text(0) ,curEditItemInfo.oldStr.toULongLong(), newIndex) == false){
                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                        }
                    }
                }
            }
        }
            break;
        case PanelType::Phases:
        {
            using Phase = TcFileModifierConfigBase::ModifierRule;;
            // if Procedure Element (parent == topLevelItem(Panel::Phases))
            if(curEditItemInfo.item->parent() == topLevelItem(panelType2number(PanelType::Phases))){
                // Prepare Name
                curEditItemInfo.item->setText(0, curEditItemInfo.item->text(0));
                 // Can be empty or not (New or Change)
                if(curEditItemInfo.oldStr.isEmpty()){    // new
                    if(curEditItemInfo.item->text(0).isEmpty()){
                        delete curEditItemInfo.item;
                    }else{
                        if(config().addPhase(curEditItemInfo.item->text(0)) == false){
                            delete curEditItemInfo.item;
                        }
                    }
                }else{
                    if(curEditItemInfo.item->text(0) != curEditItemInfo.oldStr){    // changed
                        // Manage Change - if manage failed Duplicated for example, restore
                        if(configEditor().editPhase(curEditItemInfo.oldStr, curEditItemInfo.item->text(0))  == false){
                            curEditItemInfo.item->setText(0, curEditItemInfo.oldStr);
                        }
                    }
                }
            }
        }
            break;
        default:
            break;
        }
    curEditItemInfo = {};
    }
}

List::ConfigEditor& List::configEditor(){
    return configPanel_.configEditor;
}

ControllerConfigInfo& List::config(){
    return configEditor().config();
}

void List::navigateMainPanel(const PanelType tabPanel){
    View::ConfigTabs& configTabsPanel = configPanel_.Panels::Super::ViewPanel::Super::get();
    configTabsPanel.setCurrentIndex(panelType2number(tabPanel));
}

template<>
void List::navigationElementChildFct<PanelType::AttributesList>(NavigationElement* const element){

}

template<>
void List::navigationElementChildFct<PanelType::WriteOnlyProceduresList>(NavigationElement* const element){

}

template<>
void List::navigationElementChildFct<PanelType::Procedures>(NavigationElement* const element){

}


List::NavigationElementChildFctPtr List::navigationElementChildFcts[panelType2number(PanelType::Size)]
{
    &List::navigationElementChildFct<PanelType::AttributesList>,
    &List::navigationElementChildFct<PanelType::WriteOnlyProceduresList>,
    &List::navigationElementChildFct<PanelType::Procedures>,
};
