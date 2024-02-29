#include "todo.h"
#include "ui_todo.h"
#include "device.h"

#include "functions.h"

TodoAppWidget::TodoAppWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TodoAppWidget)
{
    Device* device = Device::getSingleton();
    ui->setupUi(this);
    this->setStyleSheet(readFile("/mnt/onboard/.adds/inkbox/eink.qss"));
    this->setFont(QFont("u001"));
    ui->listWidget->setFont(QFont("u001"));
    ui->itemsListWidget->setFont(QFont("u001"));

    ui->deleteBtn->setEnabled(false);
    ui->setupBtn->setEnabled(false);

    buttonPadding;
    if(device->getScreenSize() <= Device::ScreenSize::MEDIUM) {
        buttonPadding = 5;
    }
    else {
        buttonPadding = 10;
    }
    buttonPaddingString = QString::number(buttonPadding);

    ui->closeBtn->setProperty("type", "borderless");
    ui->newBtn->setProperty("type", "borderless");
    ui->deleteBtn->setProperty("type", "borderless");
    ui->setupBtn->setProperty("type", "borderless");
    ui->newItemBtn->setProperty("type", "borderless");
    ui->deleteItemBtn->setProperty("type", "borderless");
    ui->selectAllItemsBtn->setProperty("type", "borderless");
    ui->selectItemsModeBtn->setProperty("type", "borderless");
    ui->deselectAllItemsBtn->setProperty("type", "borderless");
    ui->saveCurrentListViewBtn->setProperty("type", "borderless");
    ui->editItemBtn->setProperty("type", "borderless");
    ui->deleteBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->setupBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->newItemBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->deleteItemBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->selectAllItemsBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->selectItemsModeBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->deselectAllItemsBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->saveCurrentListViewBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->editItemBtn->setStyleSheet("padding: " + buttonPaddingString + "px;");
    ui->closeBtn->setIcon(QIcon(":/resources/close.png"));
    ui->newBtn->setIcon(QIcon(":/resources/new.png"));
    ui->newItemBtn->setIcon(QIcon(":/resources/plus.png"));
    ui->deleteBtn->setIcon(QIcon(":/resources/x-circle.png"));
    ui->setupBtn->setIcon(QIcon(":/resources/arrow-right.png"));
    ui->deleteItemBtn->setIcon(QIcon(":/resources/x-circle.png"));
    ui->selectAllItemsBtn->setIcon(QIcon(":/resources/checkbox-checked-small.png"));
    ui->selectItemsModeBtn->setIcon(QIcon(":/resources/checkbox-unchecked-small.png"));
    ui->deselectAllItemsBtn->setIcon(QIcon(":/resources/checkbox-x-small.png"));
    ui->editItemBtn->setIcon(QIcon(":/resources/edit.png"));
    ui->saveCurrentListViewBtn->setIcon(QIcon(":/resources/save.png"));
    ui->listWidget->setStyleSheet("font-size: 10pt");
    ui->listWidget->setWordWrap(true);
    ui->itemsListWidget->setStyleSheet("font-size: 10pt");
    ui->itemsListWidget->setWordWrap(true);

    ui->editToolbarWidget->hide();

    refreshList();
}

TodoAppWidget::~TodoAppWidget()
{
    delete ui;
}

void TodoAppWidget::on_closeBtn_clicked()
{
    if(currentView == currentView::home) {
        this->close();
    }
    else {
        saveCurrentList();
        switchViewHome();
    }
}

void TodoAppWidget::on_newBtn_clicked()
{
    global::keyboard::embed = false;
    global::keyboard::keyboardText = "";
    virtualkeyboard * virtualKeyboardWidget = new virtualkeyboard(this);
    virtualKeyboardWidget->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(virtualKeyboardWidget, &virtualkeyboard::enterBtnPressed, this, &TodoAppWidget::createNewList);
    QObject::connect(virtualKeyboardWidget, &virtualkeyboard::closeBtnPressed, this, &TodoAppWidget::setDefaultHomePageStatusText);
    ui->statusLabel->setText("Enter the list's name");
    virtualKeyboardWidget->show();
}

void TodoAppWidget::createNewList(QString listName) {
    log("Initializing new To-Do list with name '" + listName + "'", className);
    QJsonDocument document;
    QJsonArray mainArray;
    QJsonArray listArray;
    QJsonObject object;
    if(QFile::exists(global::localLibrary::todoDatabasePath)) {
        document = readTodoDatabase();
        object = document.object();
        mainArray = object["List"].toArray();
    }
    listArray.append(listName);

    mainArray.push_back(listArray);
    object["List"] = mainArray;

    document.setObject(object);
    writeTodoDatabase(document);

    ui->statusLabel->setText("Select or create a new list");
    refreshList();
}

void TodoAppWidget::addItem(QString itemName) {
    log("Adding item with name '" + itemName + "' to current list", className);
    // Accessing the current list's items array
    QJsonDocument document = readTodoDatabase();
    QJsonObject object = document.object();
    QJsonArray mainArray = object["List"].toArray();
    QJsonArray listArray = mainArray.at(listIndex).toArray();
    QJsonArray itemArray;

    // Item name
    itemArray.insert(0, itemName);
    // Check state (always set to false)
    itemArray.insert(1, false);

    // Adding item array to list array
    listArray.push_back(itemArray);
    mainArray.replace(listIndex, listArray);
    object["List"] = mainArray;

    document.setObject(object);
    writeTodoDatabase(document);

    setupList(listArray.at(0).toString());
}

void TodoAppWidget::refreshList() {
    if(QFile::exists(global::localLibrary::todoDatabasePath)) {
        ui->listWidget->clear();
        QJsonArray array = readTodoDatabase().object()["List"].toArray();
        for(int i = 0; i < array.count(); i++) {
            QString name = array.at(i).toArray().at(0).toString();
            if(!name.isEmpty()) {
                ui->listWidget->addItem(name);
            }
        }
    }
}

void TodoAppWidget::setupList(QString listName) {
    log("Setting up list with name '" + listName + "'", className);
    QJsonArray array = readTodoDatabase().object()["List"].toArray();
    // Iterate through the To-Do lists array to find the list's array index
    for(int i = 0; i < array.count(); i++) {
        QString name = array.at(i).toArray().at(0).toString();
        if(name == listName) {
            listIndex = i;
            break;
        }
    }
    log("List index is " + QString::number(listIndex), className);

    ui->stackedWidget->setCurrentIndex(1);
    currentView = currentView::list;
    // Iterate through the selected list's array to find item arrays
    int count = array.at(listIndex).toArray().count();
    log("List's items count is " + QString::number(count - 1), className);
    // Starting at index 1 because 0 represents the list's name
    ui->itemsListWidget->clear();
    for(int i = 1; i < count; i++) {
        QJsonArray itemArray = array.at(listIndex).toArray().at(i).toArray();
        QListWidgetItem * item = new QListWidgetItem();
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        // Set the item's name
        item->setText(array.at(listIndex).toArray().at(i).toArray().at(0).toString());
        // Is the item checked?
        if(itemArray.at(1).toBool() == true) {
            item->setCheckState(Qt::Checked);
        }
        else {
            item->setCheckState(Qt::Unchecked);
        }
        ui->itemsListWidget->addItem(item);
    }
    // I know it's confusing; think about it twice, or better, have a device at hand to see how this behaves in reality
    ui->closeBtn->setIcon(QIcon(":/resources/arrow-left.png"));
    ui->line_2->hide();
    ui->newBtn->hide();
    ui->statusLabel->setText("Select or create a new item");
}

void TodoAppWidget::on_setupBtn_clicked()
{
    ui->deleteBtn->setEnabled(false);
    ui->setupBtn->setEnabled(false);
    setupList(ui->listWidget->currentItem()->text());
}


void TodoAppWidget::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->deleteBtn->setEnabled(true);
    ui->setupBtn->setEnabled(true);
}

void TodoAppWidget::saveCurrentList() {
    QJsonDocument document = readTodoDatabase();
    QJsonObject object = document.object();
    QJsonArray mainArray = object["List"].toArray();
    QJsonArray listArray = mainArray.at(listIndex).toArray();
    for(int i = 1; i < ui->itemsListWidget->count() + 1; i++) {
        QJsonArray itemArray = listArray.at(i).toArray();
        if(ui->itemsListWidget->item(i - 1)->checkState() == Qt::Checked) {
            itemArray.replace(1, true);
        }
        else {
            itemArray.replace(1, false);
        }
        listArray.replace(i, itemArray);
    }

    // Adding item array to list array
    mainArray.replace(listIndex, listArray);
    object["List"] = mainArray;

    document.setObject(object);
    writeTodoDatabase(document);
}

void TodoAppWidget::on_deleteBtn_clicked()
{
    QJsonDocument document = readTodoDatabase();
    QJsonObject object = document.object();
    QJsonArray mainArray = object["List"].toArray();
    for(int i = 0; i < mainArray.count(); i++) {
        if(mainArray.at(i).toArray().at(0).toString() == ui->listWidget->currentItem()->text()) {
            mainArray.removeAt(i);
        }
    }
    object["List"] = mainArray;
    document.setObject(object);
    writeTodoDatabase(document);
    refreshList();
}

void TodoAppWidget::on_newItemBtn_clicked()
{
    global::keyboard::embed = false;
    global::keyboard::keyboardText = "";
    virtualkeyboard * virtualKeyboardWidget = new virtualkeyboard(this);
    virtualKeyboardWidget->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(virtualKeyboardWidget, &virtualkeyboard::enterBtnPressed, this, &TodoAppWidget::addItem);
    QObject::connect(virtualKeyboardWidget, &virtualkeyboard::closeBtnPressed, this, &TodoAppWidget::setDefaultListPageStatusText);
    ui->statusLabel->setText("Enter the item's name");
    virtualKeyboardWidget->show();
}

void TodoAppWidget::switchViewHome() {
    ui->itemsListWidget->clear();
    switchItemsSelectionMode(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->closeBtn->setIcon(QIcon(":/resources/close.png"));
    ui->line_2->show();
    ui->newBtn->show();
    ui->deleteBtn->setEnabled(true);
    ui->setupBtn->setEnabled(true);
    setDefaultHomePageStatusText();
    currentView = currentView::home;
}

void TodoAppWidget::on_selectItemsModeBtn_clicked()
{
    if(selectItemsMode == false) {
        switchItemsSelectionMode(true);
    }
    else {
        switchItemsSelectionMode(false);
    }
}

void TodoAppWidget::switchItemsSelectionMode(bool selectionMode) {
    if(selectionMode == true) {
        log("Entering selection mode", className);
        saveCurrentList();
        ui->newItemBtn->setEnabled(false);
        ui->selectItemsModeBtn->setStyleSheet("padding: " + buttonPaddingString + "px; background-color: lightGray;");
        ui->editToolbarWidget->show();
        ui->statusLabel->setText("Selection mode");
        selectItemsMode = true;
    }
    else {
        log("Exiting selection mode", className);
        setupList(readTodoDatabase().object()["List"].toArray().at(listIndex).toArray().at(0).toString());
        ui->selectItemsModeBtn->setStyleSheet("padding: " + buttonPaddingString + "px; background-color: white;");
        ui->editToolbarWidget->hide();
        ui->newItemBtn->setEnabled(true);
        ui->statusLabel->setText("Select or create a new item");
        selectItemsMode = false;
    }
    QTimer::singleShot(500, this, SLOT(resize()));
}

void TodoAppWidget::on_deleteItemBtn_clicked()
{
    QList<int> itemsToDelete;
    for(int i = 1; i < ui->itemsListWidget->count() + 1; i++) {
        if(ui->itemsListWidget->item(i - 1)->checkState() == Qt::Checked) {
            itemsToDelete.append(i);
        }
    }

    if(!itemsToDelete.isEmpty()) {
        QString itemsToDeleteString;
        for(int i = 0; i < itemsToDelete.count(); i++) {
            itemsToDeleteString.append(QString::number(itemsToDelete[i]));
            if(i < itemsToDelete.count() - 1) {
                itemsToDeleteString.append(", ");
            }
        }
        log("Deleting list item(s) " + itemsToDeleteString, className);
        QJsonDocument document = readTodoDatabase();
        QJsonObject object = document.object();
        QJsonArray mainArray = object["List"].toArray();
        QJsonArray listArray = mainArray.at(listIndex).toArray();
        int itemsRemoved = 0;
        for(int i = 0; i < itemsToDelete.count(); i++) {
            listArray.removeAt(itemsToDelete.at(i) - itemsRemoved);
            itemsRemoved++;
        }

        // Adding item array to list array
        mainArray.replace(listIndex, listArray);
        object["List"] = mainArray;

        document.setObject(object);
        writeTodoDatabase(document);

        setupList(listArray.at(0).toString());
    }
    else {
        emit showToast("No item(s) selected");
    }
}

void TodoAppWidget::on_selectAllItemsBtn_clicked()
{
    int range = ui->itemsListWidget->count();
    for(int i = 0; i < range; i++) {
        ui->itemsListWidget->item(i)->setCheckState(Qt::Checked);
    }
}

void TodoAppWidget::setDefaultHomePageStatusText() {
    ui->statusLabel->setText("Select or create a new list");
}

void TodoAppWidget::setDefaultListPageStatusText() {
    ui->statusLabel->setText("Select or create a new item");
}

void TodoAppWidget::on_deselectAllItemsBtn_clicked()
{
    int range = ui->itemsListWidget->count();
    for(int i = 0; i < range; i++) {
        ui->itemsListWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

void TodoAppWidget::on_saveCurrentListViewBtn_clicked()
{
    saveCurrentList();
    switchItemsSelectionMode(false);
}

void TodoAppWidget::on_editItemBtn_clicked()
{
    QList<int> selectedItems;
    for(int i = 1; i < ui->itemsListWidget->count() + 1; i++) {
        if(ui->itemsListWidget->item(i - 1)->checkState() == Qt::Checked) {
            selectedItems.append(i);
        }
    }
    int count = selectedItems.count();
    if(count == 1) {
        currentEditItemIndex = selectedItems.at(0);
        global::keyboard::embed = false;
        global::keyboard::keyboardText = ui->itemsListWidget->item(currentEditItemIndex - 1)->text();
        virtualkeyboard * virtualKeyboardWidget = new virtualkeyboard(this);
        virtualKeyboardWidget->setAttribute(Qt::WA_DeleteOnClose);
        QObject::connect(virtualKeyboardWidget, &virtualkeyboard::enterBtnPressed, this, &TodoAppWidget::editItemWrapper);
        QObject::connect(virtualKeyboardWidget, &virtualkeyboard::closeBtnPressed, this, &TodoAppWidget::setDefaultListPageStatusText);
        ui->statusLabel->setText("Enter the edited item's name");
        virtualKeyboardWidget->show();
    }
    else if(count < 1) {
        global::toast::delay = 3000;
        emit showToast("Please select an item");
    }
    else {
        global::toast::delay = 3000;
        emit showToast("Please select only one item");
    }
}

void TodoAppWidget::editItem(int index, QString replacement) {
    QJsonDocument document = readTodoDatabase();
    QJsonObject object = document.object();
    QJsonArray mainArray = object["List"].toArray();
    QJsonArray listArray = mainArray.at(listIndex).toArray();
    QJsonArray itemArray = listArray.at(index).toArray();
    // Replacing item string with provided replacement string
    itemArray.replace(0, replacement);
    listArray.replace(index, itemArray);

    // Adding item array to list array
    mainArray.replace(listIndex, listArray);
    object["List"] = mainArray;

    document.setObject(object);
    writeTodoDatabase(document);

    setupList(listArray.at(0).toString());
}

void TodoAppWidget::editItemWrapper(QString replacement) {
    editItem(currentEditItemIndex, replacement);
    currentEditItemIndex = NULL;
}

void TodoAppWidget::resize() {
    this->setGeometry(QRect(QPoint(0,0), qApp->primaryScreen()->geometry().size()));
    this->repaint();
}

void TodoAppWidget::on_itemsListWidget_itemClicked(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Unchecked) {
        item->setCheckState(Qt::Checked);
    }
    else {
        item->setCheckState(Qt::Unchecked);
    }
}
