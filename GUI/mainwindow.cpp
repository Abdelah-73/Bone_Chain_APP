#include "mainwindow.h"
#include "userdialog.h"
#include "supplierdialog.h"
#include "productdialog.h"
#include "articledialog.h"
#include "articlereaderdialog.h"
#include "customerdialog.h"
#include "orderstatusdialog.h"
#include "deliverydialog.h"
#include <QInputDialog> // NEW: For the Order Status popup
#include <QStringList>

// --- Backend Core Includes ---
#include "../Core/clsUser.h"
#include "../Core/clsSupplier.h"
#include "../Core/clsCustomer.h"
#include "../Core/clsProduct.h"
#include "../Core/clsOrder.h"
#include "../Core/clsArticle.h"
#include "../Core/clsInventory.h"
#include "../Core/clsReport.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi();
    sidebarWidget->hide();
    stackedScreens->setCurrentIndex(ScreenIndex::Login);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar();

    stackedScreens = new QStackedWidget(this);
    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(stackedScreens, 1);

    // Initialize all 10 screens
    setupLoginScreen();
    setupDashboardScreen();
    setupUsersScreen();
    setupSuppliersScreen();
    setupCustomersScreen();
    setupProductsScreen();
    setupOrdersScreen();
    setupInventoryScreen();
    setupReportsScreen();
    setupArticlesScreen();
    setupDeliveriesScreen();
}

// --- UI Helper Method ---
// This saves hundreds of lines of code by standardizing table creation
QTableWidget* MainWindow::createStandardTable(QStringList headers)
{
    QTableWidget* table = new QTableWidget(0, headers.size(), this);
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setStyleSheet("QTableWidget { background-color: #f8f9fa; alternate-background-color: #ffffff; }");
    table->setAlternatingRowColors(true);
    return table;
}

void MainWindow::setupSidebar()
{
    sidebarWidget = new QWidget(this);
    sidebarWidget->setFixedWidth(220);
    sidebarWidget->setStyleSheet("background-color: #2c3e50; color: white;");
    sidebarLayout = new QVBoxLayout(sidebarWidget);

    QLabel *logo = new QLabel("FERTILIZER OS\nManagement System", this);
    logo->setStyleSheet("font-size: 16px; font-weight: bold; padding: 15px; color: #1abc9c;");
    logo->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(logo);

    btnDashboard = new QPushButton(" Dashboard", this);
    btnUsers = new QPushButton(" Users", this);
    btnSuppliers = new QPushButton(" Suppliers", this);
    btnCustomers = new QPushButton(" Customers", this);
    btnProducts = new QPushButton(" Products", this);
    btnOrders = new QPushButton(" Orders", this);
    btnInventory = new QPushButton(" Inventory Alerts", this);
    btnReports = new QPushButton(" Reports", this);
    btnArticles = new QPushButton(" Articles", this);
    btnLogout = new QPushButton(" Logout", this);
    btnDeliveries = new QPushButton(" Deliveries", this);

    QPushButton* buttons[] = {btnDashboard, btnUsers, btnSuppliers, btnCustomers,
                              btnProducts, btnOrders, btnInventory, btnReports, btnArticles,btnDeliveries};

   for (QPushButton* btn : buttons) {
        // We use a specific QSS rule just for the sidebar buttons here
        btn->setStyleSheet(R"(
            QPushButton {
                text-align: left;
                padding: 12px 15px;
                border: none;
                font-size: 15px;
                background-color: transparent;
                color: #bdc3c7;
                border-radius: 6px;
                margin: 2px 10px;
            }
            QPushButton:hover {
                background-color: #34495e;
                color: #1abc9c;
            }
        )");
        btn->setCursor(Qt::PointingHandCursor);
        sidebarLayout->addWidget(btn);
    }

    sidebarLayout->addStretch();

    // Custom style just for the Logout button on the sidebar
    btnLogout->setStyleSheet(R"(
        QPushButton {
            background-color: #c0392b;
            color: white;
            padding: 12px;
            font-weight: bold;
            border-radius: 6px;
            margin: 10px;
        }
        QPushButton:hover { background-color: #a53125; }
    )");
    sidebarLayout->addWidget(btnLogout);

    // Routing
    connect(btnDashboard, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Dashboard); });
    connect(btnUsers, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Users); });
    connect(btnSuppliers, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Suppliers); });
    connect(btnCustomers, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Customers); });
    connect(btnProducts, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Products); });
    connect(btnOrders, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Orders); });
    connect(btnInventory, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Inventory); });
    connect(btnReports, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Reports); });
    connect(btnArticles, &QPushButton::clicked, [this](){ navigateToScreen(ScreenIndex::Articles); });
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::logout);
   connect(btnDeliveries, &QPushButton::clicked, [this]() {
        navigateToScreen(ScreenIndex::Deliveries);
        loadDeliveriesData();
    });
}

void MainWindow::navigateToScreen(int index)
{
    stackedScreens->setCurrentIndex(index);

    // Auto-load data when switching tabs
    switch(index) {
        case ScreenIndex::Dashboard: refreshDashboard(); break;
        case ScreenIndex::Users: loadUsersData(); break;
        case ScreenIndex::Suppliers: loadSuppliersData(); break;
        case ScreenIndex::Customers: loadCustomersData(); break;
        case ScreenIndex::Products: loadProductsData(); break;
        case ScreenIndex::Orders: loadOrdersData(); break;
        case ScreenIndex::Inventory: loadInventoryData(); break;
        case ScreenIndex::Reports: refreshDashboard(); break; // Shared logic
        case ScreenIndex::Articles: loadArticlesData(); break;
    }
}

void MainWindow::applyRolePermissions(int role)
{
    // 1. Reset standard button text (in case a different user logs out and logs back in)
    btnDashboard->setText(" Dashboard");
    btnProducts->setText(" Products");
    btnOrders->setText(" Orders");
    btnSuppliers->setText(" Suppliers");

    // 2. Hide all restricted modules by default
    btnUsers->hide();
    btnSuppliers->hide();
    btnCustomers->hide();
    btnProducts->hide();
    btnOrders->hide();
    btnInventory->hide();
    btnReports->hide();
    btnArticles->hide();

    // 3. Apply the specific blueprint roles
    if (role == clsUser::enRole::Admin)
    {
        // Admin gets everything
        btnUsers->show();
        btnSuppliers->show();
        btnCustomers->show();
        btnProducts->show();
        btnOrders->show();
        btnInventory->show();
        btnReports->show();
        btnArticles->show();
    }
    else if (role == clsUser::enRole::Supplier)
    {
        // Supplier gets Profile, Deliveries, and Points
        btnDashboard->setText(" View Profile & Points");

        btnSuppliers->setText(" Add Delivery");
        btnSuppliers->show();
    }
    else if (role == clsUser::enRole::Customer)
    {
        // Customer gets Products, Orders, and Points
        btnDashboard->setText(" View Points");

        btnProducts->setText(" View Products");
        btnProducts->show();

        btnOrders->setText(" Manage Orders");
        btnOrders->show();
    }
}
// ==========================================
// 1. LOGIN SCREEN
// ==========================================
void MainWindow::setupLoginScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("System Login", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #333; margin-bottom: 20px;");

    txtUsername = new QLineEdit(this);
    txtUsername->setPlaceholderText("Username");
    txtUsername->setFixedSize(300, 40);

    txtPassword = new QLineEdit(this);
    txtPassword->setPlaceholderText("Password");
    txtPassword->setEchoMode(QLineEdit::Password);
    txtPassword->setFixedSize(300, 40);

    QPushButton *btnLogin = new QPushButton("Secure Login", this);
    btnLogin->setFixedSize(300, 45);
    btnLogin->setStyleSheet("background-color: #2980b9; color: white; font-weight: bold; font-size: 16px;");

    l->addWidget(title, 0, Qt::AlignCenter);
    l->addWidget(txtUsername, 0, Qt::AlignCenter);
    l->addWidget(txtPassword, 0, Qt::AlignCenter);
    l->addWidget(btnLogin, 0, Qt::AlignCenter);

    connect(btnLogin, &QPushButton::clicked, this, &MainWindow::handleLogin);
    stackedScreens->insertWidget(ScreenIndex::Login, w);
}

void MainWindow::handleLogin()
{
    string user = txtUsername->text().toStdString();
    string pass = txtPassword->text().toStdString();

    clsUser CurrentUser = clsUser::FindByUsernameAndPassword(user, pass);

    if (!CurrentUser.IsEmpty() && CurrentUser.IsActive()) {
        txtPassword->clear();

        // NEW: Apply the RBAC Permissions before showing the sidebar!
        applyRolePermissions(CurrentUser.Role());

        sidebarWidget->show();
        navigateToScreen(ScreenIndex::Dashboard);
    } else {
        QMessageBox::critical(this, "Access Denied", "Invalid Username/Password or Inactive Account.");
    }
}

void MainWindow::logout()
{
    sidebarWidget->hide();
    txtPassword->clear();
    stackedScreens->setCurrentIndex(ScreenIndex::Login);
}

// ==========================================
// 2. DASHBOARD SCREEN
// ==========================================
void MainWindow::setupDashboardScreen()
{
    QWidget *w = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(w); // Split left and right

    // ==========================================
    // LEFT PANEL: Metrics & Quick Actions
    // ==========================================
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    QLabel *title = new QLabel("Admin Control Center", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");

    // --- Metrics Grid ---
    QGridLayout *metricsGrid = new QGridLayout();
    lblDashSuppliers = new QLabel("Suppliers: 0");
    lblDashCustomers = new QLabel("Customers: 0");
    lblDashProducts  = new QLabel("Products: 0");
    lblDashOrders    = new QLabel("Orders: 0");
    lblDashBones     = new QLabel("Collected Bones: 0 kg");
    lblDashRevenue   = new QLabel("Total Revenue: $0.00");
    lblDashDeliveries = new QLabel("Total Deliveries: 0");

    // Style the metrics to look like cards
    QString cardStyle = "background-color: white; padding: 15px; border-radius: 8px; font-size: 16px; font-weight: bold; border: 1px solid #dfe6e9;";
    lblDashSuppliers->setStyleSheet(cardStyle);
    lblDashCustomers->setStyleSheet(cardStyle);
    lblDashProducts->setStyleSheet(cardStyle);
    lblDashOrders->setStyleSheet(cardStyle);
    lblDashBones->setStyleSheet(cardStyle + "color: #d35400;"); // Orange accent
    lblDashRevenue->setStyleSheet(cardStyle + "color: #27ae60;"); // Green accent
    lblDashDeliveries->setStyleSheet(cardStyle + "color: #2980b9;");

    metricsGrid->addWidget(lblDashSuppliers, 0, 0);
    metricsGrid->addWidget(lblDashCustomers, 0, 1);
    metricsGrid->addWidget(lblDashProducts, 1, 0);
    metricsGrid->addWidget(lblDashOrders, 1, 1);
    metricsGrid->addWidget(lblDashBones, 2, 0);
    metricsGrid->addWidget(lblDashRevenue, 2, 1);
    metricsGrid->addWidget(lblDashDeliveries, 3, 0);

    // --- Quick Actions ---
    QLabel *lblActions = new QLabel("Quick Actions", this);
    lblActions->setStyleSheet("font-size: 18px; font-weight: bold; margin-top: 20px;");

    QHBoxLayout *actionsLayout = new QHBoxLayout();
    btnQuickAddSupplier = new QPushButton("+ Add Supplier");
    btnQuickAddProduct  = new QPushButton("+ Add Product");
    btnQuickAddArticle  = new QPushButton("+ Create Article");
    btnQuickReports     = new QPushButton("View Reports");

    QString actionStyle = "background-color: #34495e; color: white; padding: 10px; border-radius: 6px; font-weight: bold;";
    btnQuickAddSupplier->setStyleSheet(actionStyle);
    btnQuickAddProduct->setStyleSheet(actionStyle);
    btnQuickAddArticle->setStyleSheet(actionStyle);
    btnQuickReports->setStyleSheet(actionStyle);

    connect(btnQuickAddSupplier, &QPushButton::clicked, [this]() {
    SupplierDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
        refreshDashboard(); // Instantly update the metrics if saved!
    }
    });

    connect(btnQuickAddProduct, &QPushButton::clicked, [this]() {
    ProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
        refreshDashboard(); // Instantly update metrics and Low Stock Alerts table!
    }
});

connect(btnQuickAddArticle, &QPushButton::clicked, [this]() {
    ArticleDialog dialog(this);
    dialog.exec();
    // Articles don't directly update the dashboard metrics currently,
    // but we exec() it here so the quick action works!
});

    actionsLayout->addWidget(btnQuickAddSupplier);
    actionsLayout->addWidget(btnQuickAddProduct);
    actionsLayout->addWidget(btnQuickAddArticle);
    actionsLayout->addWidget(btnQuickReports);

    leftLayout->addWidget(title);
    leftLayout->addLayout(metricsGrid);
    leftLayout->addWidget(lblActions);
    leftLayout->addLayout(actionsLayout);
    leftLayout->addStretch();

    // ==========================================
    // RIGHT PANEL: Low Stock Alerts
    // ==========================================
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightPanel->setFixedWidth(400); // Keep it neatly to the side

    QLabel *alertTitle = new QLabel("⚠️ Low Stock Alerts", this);
    alertTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #c0392b; margin-bottom: 10px;");

    tableDashLowStock = createStandardTable({"Product", "Stock", "Min"});
    tableDashLowStock->verticalHeader()->setVisible(false);

    rightLayout->addWidget(alertTitle);
    rightLayout->addWidget(tableDashLowStock);

    // Combine Panels
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel);

    // Connect Quick Action Buttons to their respective screens
    connect(btnQuickReports, &QPushButton::clicked, [this]() { navigateToScreen(ScreenIndex::Reports); });
    // Note: The "Add" buttons will be connected to the Dialog pop-ups once we build them!

    stackedScreens->insertWidget(ScreenIndex::Dashboard, w);
}

void MainWindow::refreshDashboard()
{
    // 1. Update the Metrics Grid using clsReport
    lblDashSuppliers->setText("Suppliers: " + QString::number(clsReport::TotalSuppliers()));
    lblDashCustomers->setText("Customers: " + QString::number(clsReport::TotalCustomers()));
    lblDashProducts->setText("Products: " + QString::number(clsReport::TotalProducts()));
    lblDashOrders->setText("Orders: " + QString::number(clsReport::TotalOrders()));

    lblDashBones->setText("Collected Bones: " + QString::number(clsReport::TotalCollectedBones(), 'f', 1) + " kg");
    lblDashRevenue->setText("Total Revenue: $" + QString::number(clsReport::TotalRevenue(), 'f', 2));

    // 2. Populate the Low Stock Alert Table
    tableDashLowStock->setRowCount(0);
    vector<clsProduct> vLowStock = clsInventory::GetLowStockAlerts();

    for (size_t i = 0; i < vLowStock.size(); i++)
    {
        tableDashLowStock->insertRow(i);
        tableDashLowStock->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vLowStock[i].Name())));

        QTableWidgetItem *stockItem = new QTableWidgetItem(QString::number(vLowStock[i].StockQuantity()));
        stockItem->setForeground(QBrush(Qt::red)); // Visually flag the low number
        stockItem->setFont(QFont("Arial", 10, QFont::Bold));

        tableDashLowStock->setItem(i, 1, stockItem);
        tableDashLowStock->setItem(i, 2, new QTableWidgetItem(QString::number(vLowStock[i].MinimumStock())));
    }
}

// ==========================================
// 3. USERS SCREEN
// ==========================================
void MainWindow::setupUsersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel("Users Management", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");

    tableUsers = createStandardTable({"ID", "Username", "Name", "Role", "Status"});

    // Setup Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("Add New User", this);
    QPushButton *btnEdit = new QPushButton("Edit User", this); // NEW BUTTON
    QPushButton *btnDel = new QPushButton("Delete User", this);

    // Styling the buttons
    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;"); // Orange for Edit
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit); // Add to layout
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableUsers);
    l->addLayout(btnLayout);

    // Connect Delete
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedUser);

    // Connect Add
    connect(btnAdd, &QPushButton::clicked, [this]() {
        UserDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadUsersData();
        }
    });

    // NEW: Connect Edit
    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableUsers->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Select User", "Please select a user to edit from the table first.");
            return;
        }

        // Grab the User ID from the selected row
        QString id = tableUsers->item(row, 0)->text();

        UserDialog dialog(this);
        dialog.loadUserForEdit(id.toStdString()); // Tell the dialog to load this user!

        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadUsersData(); // Instantly refresh the table with the updated data
        }
    });

    stackedScreens->insertWidget(ScreenIndex::Users, w);
}

void MainWindow::loadUsersData()
{
    tableUsers->setRowCount(0);
    vector<clsUser> vList = clsUser::GetUsersList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableUsers->insertRow(i);
        tableUsers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].UserID())));
        tableUsers->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].Username())));
        tableUsers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].FullName())));
        tableUsers->setItem(i, 3, new QTableWidgetItem(QString::number(vList[i].Role())));
        tableUsers->setItem(i, 4, new QTableWidgetItem(vList[i].IsActive() ? "Active" : "Inactive"));
    }
}

void MainWindow::deleteSelectedUser()
{
    int row = tableUsers->currentRow();
    if (row < 0) return;
    QString id = tableUsers->item(row, 0)->text();
    if (QMessageBox::question(this, "Confirm", "Delete User " + id + "?") == QMessageBox::Yes) {
        if (clsUser::Find(id.toStdString()).Delete()) loadUsersData();
    }
}

// ==========================================
// 4. SUPPLIERS SCREEN
// ==========================================
void MainWindow::setupSuppliersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel("Suppliers Management", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableSuppliers = createStandardTable({"ID", "Name", "Phone", "Bone Type", "Points"});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("Add New Supplier", this);
    QPushButton *btnEdit = new QPushButton("Edit Supplier", this);
    QPushButton *btnDel = new QPushButton("Delete Supplier", this);

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableSuppliers);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedSupplier);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        SupplierDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadSuppliersData();
        }
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableSuppliers->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Selection Required", "Please select a supplier to edit.");
            return;
        }
        QString id = tableSuppliers->item(row, 0)->text();
        SupplierDialog dialog(this);
        dialog.loadSupplierForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadSuppliersData();
        }
    });

    stackedScreens->insertWidget(ScreenIndex::Suppliers, w);
}

void MainWindow::loadSuppliersData()
{
    tableSuppliers->setRowCount(0);
    vector<clsSupplier> vList = clsSupplier::GetSuppliersList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableSuppliers->insertRow(i);
        tableSuppliers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].SupplierID())));
        tableSuppliers->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].FirstName())));
        tableSuppliers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].PhoneNumber())));
        tableSuppliers->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vList[i].BoneType())));
        tableSuppliers->setItem(i, 4, new QTableWidgetItem(QString::number(vList[i].Points())));
    }
}

void MainWindow::deleteSelectedSupplier()
{
    int row = tableSuppliers->currentRow();
    if (row < 0) return;
    if (clsSupplier::Find(tableSuppliers->item(row, 0)->text().toStdString()).Delete()) loadSuppliersData();
}

// ==========================================
// 5. CUSTOMERS SCREEN
// ==========================================
void MainWindow::setupCustomersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel("Customers Management", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableCustomers = createStandardTable({"ID", "Name", "Phone", "Address", "Type"});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("Add Customer", this);
    QPushButton *btnEdit = new QPushButton("Edit Customer", this);
    QPushButton *btnDel = new QPushButton("Delete Customer", this);

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableCustomers);
    l->addLayout(btnLayout);

    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedCustomer);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        CustomerDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) loadCustomersData();
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableCustomers->currentRow();
        if (row < 0) return;
        QString id = tableCustomers->item(row, 0)->text();
        CustomerDialog dialog(this);
        dialog.loadCustomerForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) loadCustomersData();
    });

    stackedScreens->insertWidget(ScreenIndex::Customers, w);
}

void MainWindow::loadCustomersData()
{
    tableCustomers->setRowCount(0);
    vector<clsCustomer> vList = clsCustomer::GetCustomersList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableCustomers->insertRow(i);
        tableCustomers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].CustomerID())));
        tableCustomers->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].FirstName())));
        tableCustomers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].PhoneNumber())));
        tableCustomers->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vList[i].Address())));
        tableCustomers->setItem(i, 4, new QTableWidgetItem(QString::number(vList[i].CustomerType())));
    }
}

void MainWindow::deleteSelectedCustomer()
{
    int row = tableCustomers->currentRow();
    if (row < 0) return;
    if (clsCustomer::Find(tableCustomers->item(row, 0)->text().toStdString()).Delete()) loadCustomersData();
}

// ==========================================
// 6. PRODUCTS SCREEN
// ==========================================
void MainWindow::setupProductsScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel("Products Catalog", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableProducts = createStandardTable({"ID", "Name", "Category", "Price", "Stock"});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("Add New Product", this);
    QPushButton *btnEdit = new QPushButton("Edit Product", this);
    QPushButton *btnDel = new QPushButton("Delete Product", this);

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableProducts);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedProduct);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        ProductDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadProductsData();
        }
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableProducts->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Selection Required", "Please select a product to edit.");
            return;
        }
        QString id = tableProducts->item(row, 0)->text();
        ProductDialog dialog(this);
        dialog.loadProductForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadProductsData();
        }
    });

    stackedScreens->insertWidget(ScreenIndex::Products, w);
}

void MainWindow::loadProductsData()
{
    tableProducts->setRowCount(0);
    vector<clsProduct> vList = clsProduct::GetProductsList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableProducts->insertRow(i);
        tableProducts->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].ProductID())));
        tableProducts->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].Name())));
        tableProducts->setItem(i, 2, new QTableWidgetItem(QString::number(vList[i].Category())));
        tableProducts->setItem(i, 3, new QTableWidgetItem("$" + QString::number(vList[i].Price(), 'f', 2)));
        tableProducts->setItem(i, 4, new QTableWidgetItem(QString::number(vList[i].StockQuantity())));
    }
}

void MainWindow::deleteSelectedProduct()
{
    int row = tableProducts->currentRow();
    if (row < 0) return;
    if (clsProduct::Find(tableProducts->item(row, 0)->text().toStdString()).Delete()) loadProductsData();
}

// ==========================================
// 7. ORDERS SCREEN
// ==========================================
void MainWindow::setupOrdersScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel("Order Processing", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableOrders = createStandardTable({"Order ID", "Customer ID", "Product ID", "Total Price", "Status Code"});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnUpdateStatus = new QPushButton("Update Status", this); // NEW BUTTON
    QPushButton *btnCancel = new QPushButton("Cancel Order", this);

    btnUpdateStatus->setStyleSheet("background-color: #2980b9; color: white; padding: 8px; width: 130px;");
    btnCancel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnUpdateStatus);
    btnLayout->addWidget(btnCancel);

    l->addWidget(title);
    l->addWidget(tableOrders);
    l->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, this, &MainWindow::cancelSelectedOrder);

  // --- NEW: Custom Order Status Update Logic ---
connect(btnUpdateStatus, &QPushButton::clicked, [this]() {
    int row = tableOrders->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Select Order", "Please select an order to update.");
        return;
    }

    QString orderID = tableOrders->item(row, 0)->text();
    clsOrder order = clsOrder::Find(orderID.toStdString());

    if (!order.IsEmpty()) {
        // Open our custom colored dialog
        OrderStatusDialog dialog(order.Status(), this);

        if (dialog.exec() == QDialog::Accepted) {
            // Get the status integer from our dropdown
            int newStatus = dialog.getSelectedStatus();

            order.SetStatus((clsOrder::enStatus)newStatus);
            order.Save();

            loadOrdersData();   // Refresh the table (which will apply the colors we set up earlier!)
            refreshDashboard(); // Update metrics
        }
    }
});
    stackedScreens->insertWidget(ScreenIndex::Orders, w);
}

void MainWindow::loadOrdersData()
{
    tableOrders->setRowCount(0);
    vector<clsOrder> vList = clsOrder::GetOrdersList();

    for (size_t i = 0; i < vList.size(); i++) {
        tableOrders->insertRow(i);
        tableOrders->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].OrderID())));
        tableOrders->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].CustomerID())));
        tableOrders->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].ProductID())));
        tableOrders->setItem(i, 3, new QTableWidgetItem("$" + QString::number(vList[i].TotalPrice(), 'f', 2)));

        // --- NEW: Color Coded Status Logic ---
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold)); // Make it bold

        int status = vList[i].Status();

        if (status == clsOrder::enStatus::Pending) {
            statusItem->setText("Pending");
            statusItem->setForeground(QBrush(QColor("#f39c12"))); // Orange
        }
        else if (status == clsOrder::enStatus::Confirmed) {
            statusItem->setText("Confirmed");
            statusItem->setForeground(QBrush(QColor("#3498db"))); // Blue
        }
        else if (status == clsOrder::enStatus::Processing) {
            statusItem->setText("Processing");
            statusItem->setForeground(QBrush(QColor("#9b59b6"))); // Purple
        }
        else if (status == clsOrder::enStatus::Delivered) {
            statusItem->setText("Delivered");
            statusItem->setForeground(QBrush(QColor("#27ae60"))); // Green
        }
        else if (status == clsOrder::enStatus::Cancelled) {
            statusItem->setText("Cancelled");
            statusItem->setForeground(QBrush(QColor("#e74c3c"))); // Red
        }

        // Insert the colored item into column 4
        tableOrders->setItem(i, 4, statusItem);
    }
}

void MainWindow::cancelSelectedOrder()
{
    int row = tableOrders->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Select Order", "Please select an order to cancel.");
        return;
    }

    // 1. Safety Checks
    QString currentStatus = tableOrders->item(row, 4)->text();

    if (currentStatus == "Cancelled") {
        QMessageBox::information(this, "Already Cancelled", "This order has already been cancelled.");
        return;
    }
    if (currentStatus == "Delivered") {
        QMessageBox::warning(this, "Cannot Cancel", "You cannot cancel an order that is already Delivered. Please process a Return instead.");
        return;
    }

    // 2. Ask for Admin Confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Cancellation",
                                  "Are you sure you want to cancel this order? The items will be returned to inventory.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    // Read necessary IDs from the table
    string orderID = tableOrders->item(row, 0)->text().toStdString();
    string productID = tableOrders->item(row, 2)->text().toStdString();

    // Load the order object
    clsOrder order = clsOrder::Find(orderID);
    if (order.IsEmpty()) return;

    // ==========================================
    // Core Logic 1: Restore Inventory
    // ==========================================
    clsProduct product = clsProduct::Find(productID);
    if (!product.IsEmpty()) {
        // Assuming your clsOrder has a Quantity() getter.
        // We add the order's quantity back to the current stock.
        product.SetStockQuantity(product.StockQuantity() + order.Quantity());
        product.Save();
    }

    // ==========================================
    // Core Logic 2: Refund Customer (If applicable)
    // ==========================================
    /* clsCustomer customer = clsCustomer::Find(order.CustomerID());
    if (!customer.IsEmpty()) {
        // Example: If they paid with points, give them back
        // customer.SetPoints(customer.Points() + order.PointsSpent());
        // customer.Save();
    }
    */

    // ==========================================
    // Core Logic 3: Change Status to Cancelled
    // ==========================================
    order.SetStatus(clsOrder::enStatus::Cancelled);
    order.Save();

    // ==========================================
    // Update the UI
    // ==========================================
    QTableWidgetItem *statusItem = tableOrders->item(row, 4);
    statusItem->setText("Cancelled");
    statusItem->setForeground(QBrush(QColor("#e74c3c"))); // Turn the text Red

    QMessageBox::information(this, "Order Cancelled", "The order has been cancelled and the items have been returned to inventory.");

    // Refresh dashboard to show the recovered inventory numbers and updated revenue!
    refreshDashboard();
}

// ==========================================
// 8. INVENTORY ALERTS SCREEN
// ==========================================
void MainWindow::setupInventoryScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    // Header Layout with Title and Filter
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *title = new QLabel("Inventory Control", this);
    title->setStyleSheet("font-weight: bold; font-size: 20px;");

    QLabel *lblFilter = new QLabel("Filter View:", this);
    lblFilter->setStyleSheet("font-weight: bold;");

    cmbInventoryFilter = new QComboBox(this);
    cmbInventoryFilter->addItems({"All Current Stock", "Low Stock Alerts", "Out Of Stock"});
    cmbInventoryFilter->setStyleSheet("padding: 5px; border-radius: 4px; border: 1px solid #bdc3c7; width: 150px;color: #2980b9; font-weight: bold;");

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(lblFilter);
    headerLayout->addWidget(cmbInventoryFilter);

    // Add the new "Status" column to the table
    tableInventory = createStandardTable({"Product ID", "Name", "Quantity", "Minimum", "Status"});

    l->addLayout(headerLayout);
    l->addWidget(tableInventory);

    // Re-load the data whenever the Admin changes the filter dropdown!
    connect(cmbInventoryFilter, &QComboBox::currentIndexChanged, [this]() {
        loadInventoryData();
    });

    stackedScreens->insertWidget(ScreenIndex::Inventory, w);
}

void MainWindow::loadInventoryData()
{
    tableInventory->setRowCount(0);

    // Get all products to evaluate
    vector<clsProduct> vProducts = clsProduct::GetProductsList();
    int currentFilter = cmbInventoryFilter->currentIndex();
    // 0 = All Stock, 1 = Low Stock, 2 = Out Of Stock

    int row = 0; // Keep track of the row index for insertion

    for (size_t i = 0; i < vProducts.size(); i++)
    {
        int stock = vProducts[i].StockQuantity();
        int minStock = vProducts[i].MinimumStock();

        // Determine the Status
        bool isOutOfStock = (stock == 0);
        bool isLowStock = (stock > 0 && stock <= minStock);

        // Apply the Admin's Filter
        if (currentFilter == 1 && !isLowStock && !isOutOfStock) continue; // Skip healthy stock if viewing Low
        if (currentFilter == 2 && !isOutOfStock) continue; // Skip everything with >0 stock if viewing Out

        // If it passes the filter, add it to the table!
        tableInventory->insertRow(row);
        tableInventory->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(vProducts[i].ProductID())));
        tableInventory->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(vProducts[i].Name())));
        tableInventory->setItem(row, 2, new QTableWidgetItem(QString::number(stock)));
        tableInventory->setItem(row, 3, new QTableWidgetItem(QString::number(minStock)));

        // Generate the visual Status Badge
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (isOutOfStock) {
            statusItem->setText("Out of Stock");
            statusItem->setForeground(QBrush(QColor("#c0392b"))); // Dark Red
        }
        else if (isLowStock) {
            statusItem->setText("Low Stock");
            statusItem->setForeground(QBrush(QColor("#e67e22"))); // Orange/Warning
        }
        else {
            statusItem->setText("Healthy");
            statusItem->setForeground(QBrush(QColor("#27ae60"))); // Green
        }

        tableInventory->setItem(row, 4, statusItem);
        row++; // Increment our UI row index
    }
}
// ==========================================
// 9. REPORTS SCREEN
// ==========================================
void MainWindow::setupReportsScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(w);

    // Header Layout with Title and Refresh Button
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("System Reports & Analytics", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    QPushButton *btnRefreshReports = new QPushButton(" Refresh Data", this);
    btnRefreshReports->setStyleSheet("background-color: #34495e; color: white; padding: 8px 15px; border-radius: 4px; font-weight: bold;");

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(btnRefreshReports);

    // Initialize the Tab Widget
    tabReports = new QTabWidget(this);
    tabReports->setStyleSheet(R"(
        QTabWidget::pane { border: 1px solid #bdc3c7; border-radius: 4px; background: white; }
        QTabBar::tab { background: #ecf0f1; padding: 10px 20px; border-top-left-radius: 4px; border-top-right-radius: 4px; font-weight: bold; color: #7f8c8d; margin-right: 2px; }
        QTabBar::tab:selected { background: #3498db; color: white; }
        QTabBar::tab:hover:!selected { background: #d0d3d4; }
    )");

    // --- Tab 1: Sales & Orders Report ---
    QWidget *tabSales = new QWidget();
    QVBoxLayout *lSales = new QVBoxLayout(tabSales);
    tableRepSales = createStandardTable({"Order ID", "Customer", "Product", "Total Revenue", "Status"});
    lSales->addWidget(tableRepSales);
    tabReports->addTab(tabSales, "Sales Reports");

    // --- Tab 2: Inventory Valuation ---
    QWidget *tabInv = new QWidget();
    QVBoxLayout *lInv = new QVBoxLayout(tabInv);
    tableRepInventory = createStandardTable({"Product", "Category", "Stock Level", "Min Alert", "Status"});
    lInv->addWidget(tableRepInventory);
    tabReports->addTab(tabInv, "Inventory Reports");

    // --- Tab 3: Supplier Performance ---
    QWidget *tabSup = new QWidget();
    QVBoxLayout *lSup = new QVBoxLayout(tabSup);
    tableRepSuppliers = createStandardTable({"Supplier Name", "Type", "Avg Weekly (kg)", "Points Issued"});
    lSup->addWidget(tableRepSuppliers);
    tabReports->addTab(tabSup, "Supplier Reports");

    // --- Tab 4: Customer Insights ---
    QWidget *tabCust = new QWidget();
    QVBoxLayout *lCust = new QVBoxLayout(tabCust);
    tableRepCustomers = createStandardTable({"Customer Name", "Type", "Contact", "Points Balance"});
    lCust->addWidget(tableRepCustomers);
    tabReports->addTab(tabCust, "Customer Reports");

    // --- Tab 5: Deliveries Report (NEW) ---
    QWidget *tabDel = new QWidget();
    QVBoxLayout *lDel = new QVBoxLayout(tabDel);
    tableRepDeliveries = createStandardTable({"Delivery ID", "Supplier ID", "Date", "Bone Type", "Quantity (kg)", "Status"});
    lDel->addWidget(tableRepDeliveries);
    tabReports->addTab(tabDel, "Delivery Reports"); // Add it to the tab widget

    // Assemble the main layout
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(tabReports);

    // Connect the refresh button to a generic load function
    connect(btnRefreshReports, &QPushButton::clicked, this, &MainWindow::loadAllReportsData);

    stackedScreens->insertWidget(ScreenIndex::Reports, w);
}

void MainWindow::loadAllReportsData()
{
    // 1. Load Sales Report
    tableRepSales->setRowCount(0);
    vector<clsOrder> vOrders = clsOrder::GetOrdersList();
    for (size_t i = 0; i < vOrders.size(); i++) {
        tableRepSales->insertRow(i);
        tableRepSales->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vOrders[i].OrderID())));
        tableRepSales->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vOrders[i].CustomerID())));
        tableRepSales->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vOrders[i].ProductID())));
        tableRepSales->setItem(i, 3, new QTableWidgetItem("$" + QString::number(vOrders[i].TotalPrice(), 'f', 2)));

        // --- COLOR CODED STATUS ---
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (vOrders[i].Status() == clsOrder::enStatus::Delivered) {
            statusItem->setText("Delivered");
            statusItem->setForeground(QBrush(QColor("#27ae60"))); // Green
        } else if (vOrders[i].Status() == clsOrder::enStatus::Cancelled) {
            statusItem->setText("Cancelled");
            statusItem->setForeground(QBrush(QColor("#e74c3c"))); // Red
        } else {
            statusItem->setText("In Progress");
            statusItem->setForeground(QBrush(QColor("#f39c12"))); // Orange for anything still pending
        }
        tableRepSales->setItem(i, 4, statusItem);
    }

    // 2. Load Inventory Report
    tableRepInventory->setRowCount(0);
    vector<clsProduct> vProducts = clsProduct::GetProductsList();
    for (size_t i = 0; i < vProducts.size(); i++) {
        tableRepInventory->insertRow(i);
        tableRepInventory->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vProducts[i].Name())));
        tableRepInventory->setItem(i, 1, new QTableWidgetItem(QString::number(vProducts[i].Category())));
        tableRepInventory->setItem(i, 2, new QTableWidgetItem(QString::number(vProducts[i].StockQuantity())));
        tableRepInventory->setItem(i, 3, new QTableWidgetItem(QString::number(vProducts[i].MinimumStock())));

        // --- COLOR CODED STATUS ---
        QTableWidgetItem *stockStatusItem = new QTableWidgetItem();
        stockStatusItem->setFont(QFont("Arial", 10, QFont::Bold));

        int stock = vProducts[i].StockQuantity();
        int minStock = vProducts[i].MinimumStock();

        if (stock == 0) {
            stockStatusItem->setText("Out of Stock");
            stockStatusItem->setForeground(QBrush(QColor("#c0392b"))); // Red
        } else if (stock <= minStock) {
            stockStatusItem->setText("Needs Restock");
            stockStatusItem->setForeground(QBrush(QColor("#e67e22"))); // Orange
        } else {
            stockStatusItem->setText("Healthy");
            stockStatusItem->setForeground(QBrush(QColor("#27ae60"))); // Green
        }
        tableRepInventory->setItem(i, 4, stockStatusItem);
    }

    // 3. Load Supplier Report
    tableRepSuppliers->setRowCount(0);
    vector<clsSupplier> vSuppliers = clsSupplier::GetSuppliersList();
    for (size_t i = 0; i < vSuppliers.size(); i++) {
        tableRepSuppliers->insertRow(i);
        tableRepSuppliers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vSuppliers[i].FirstName())));
        tableRepSuppliers->setItem(i, 1, new QTableWidgetItem(QString::number(vSuppliers[i].SupplierType())));
        tableRepSuppliers->setItem(i, 2, new QTableWidgetItem(QString::number(vSuppliers[i].WeeklyQuantity())));
        tableRepSuppliers->setItem(i, 3, new QTableWidgetItem(QString::number(vSuppliers[i].Points())));
    }

    // 4. Load Customer Report
    tableRepCustomers->setRowCount(0);
    vector<clsCustomer> vCustomers = clsCustomer::GetCustomersList();
    for (size_t i = 0; i < vCustomers.size(); i++) {
        tableRepCustomers->insertRow(i);
        tableRepCustomers->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vCustomers[i].FirstName())));
        tableRepCustomers->setItem(i, 1, new QTableWidgetItem(QString::number(vCustomers[i].CustomerType())));
        tableRepCustomers->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vCustomers[i].PhoneNumber())));
        tableRepCustomers->setItem(i, 3, new QTableWidgetItem("0"));
    }

    // 5. Load Deliveries Report
    tableRepDeliveries->setRowCount(0);
    vector<clsDelivery> vReportDeliveries = clsDelivery::GetDeliveriesList();
    for (size_t i = 0; i < vReportDeliveries.size(); i++) {
        tableRepDeliveries->insertRow(i);
        tableRepDeliveries->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vReportDeliveries[i].DeliveryID())));
        tableRepDeliveries->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vReportDeliveries[i].SupplierID())));
        tableRepDeliveries->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vReportDeliveries[i].Date())));
        tableRepDeliveries->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vReportDeliveries[i].ProductID())));
        tableRepDeliveries->setItem(i, 4, new QTableWidgetItem(QString::number(vReportDeliveries[i].Quantity())));

        // --- COLOR CODED STATUS ---
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (vReportDeliveries[i].Status() == clsDelivery::enStatus::Approved) {
            statusItem->setText("Approved");
            statusItem->setForeground(QBrush(QColor("#27ae60"))); // Green
        } else if (vReportDeliveries[i].Status() == clsDelivery::enStatus::Rejected) {
            statusItem->setText("Rejected");
            statusItem->setForeground(QBrush(QColor("#e74c3c"))); // Red
        } else {
            statusItem->setText("Pending");
            statusItem->setForeground(QBrush(QColor("#f39c12"))); // Orange
        }

        tableRepDeliveries->setItem(i, 5, statusItem);
    }
}
// ==========================================
// 10. ARTICLES SCREEN
// ==========================================
void MainWindow::setupArticlesScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel("Articles & Publications", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    tableArticles = createStandardTable({"ID", "Title", "Category", "Publish Date"});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("Create Article", this);
    QPushButton *btnEdit = new QPushButton("Edit Article", this);
    QPushButton *btnDel = new QPushButton("Delete Article", this);

    btnAdd->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 130px;");
    btnEdit->setStyleSheet("background-color: #f39c12; color: white; padding: 8px; width: 130px;");
    btnDel->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 130px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnEdit);
    btnLayout->addWidget(btnDel);

    l->addWidget(title);
    l->addWidget(tableArticles);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::deleteSelectedArticle);

    connect(btnAdd, &QPushButton::clicked, [this]() {
        ArticleDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadArticlesData();
        }
    });

    connect(btnEdit, &QPushButton::clicked, [this]() {
        int row = tableArticles->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Selection Required", "Please select an article to edit.");
            return;
        }
        QString id = tableArticles->item(row, 0)->text();
        ArticleDialog dialog(this);
        dialog.loadArticleForEdit(id.toStdString());
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadArticlesData();
        }
    });

    // --- Connections ---

// NEW: Double-Click to Read!
connect(tableArticles, &QTableWidget::cellDoubleClicked, [this](int row, int column) {
    QString id = tableArticles->item(row, 0)->text();
    ArticleReaderDialog reader(id.toStdString(), this);
    reader.exec(); // Pop open the reader window
});
    stackedScreens->insertWidget(ScreenIndex::Articles, w);
}

void MainWindow::loadArticlesData()
{
    tableArticles->setRowCount(0);
    vector<clsArticle> vList = clsArticle::GetArticlesList();
    for (size_t i = 0; i < vList.size(); i++) {
        tableArticles->insertRow(i);
        tableArticles->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vList[i].ArticleID())));
        tableArticles->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vList[i].Title())));
        tableArticles->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vList[i].Category())));
        tableArticles->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vList[i].PublishDate())));
    }
}

void MainWindow::deleteSelectedArticle()
{
    int row = tableArticles->currentRow();
    if (row < 0) return;
    if (clsArticle::Find(tableArticles->item(row, 0)->text().toStdString()).Delete()) loadArticlesData();
}

// ==========================================
// 11. DELIVERIES SCREEN
// ==========================================

void MainWindow::setupDeliveriesScreen()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);

    QLabel *title = new QLabel("Deliveries Management", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");

    // Create the table with the exact columns needed
    tableDeliveries = createStandardTable({"Delivery ID", "Supplier ID", "Date", "Product ID", "Quantity (kg)", "Status"});

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnAdd = new QPushButton("Log New Delivery", this);
    QPushButton *btnApprove = new QPushButton("Approve Delivery", this);
    QPushButton *btnReject = new QPushButton("Reject", this);

    btnAdd->setStyleSheet("background-color: #34495e; color: white; padding: 8px; width: 140px;");
    btnApprove->setStyleSheet("background-color: #27ae60; color: white; padding: 8px; width: 140px; font-weight: bold;");
    btnReject->setStyleSheet("background-color: #e74c3c; color: white; padding: 8px; width: 140px;");

    btnLayout->addStretch();
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnApprove);
    btnLayout->addWidget(btnReject);

    l->addWidget(title);
    l->addWidget(tableDeliveries);
    l->addLayout(btnLayout);

    // --- Connections ---
    connect(btnAdd, &QPushButton::clicked, [this]() {
        DeliveryDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted && dialog.wasSaved()) {
            loadDeliveriesData();
            QMessageBox::information(this, "Success", "Delivery submitted! It is now Pending approval.");
        }
    });

    connect(btnApprove, &QPushButton::clicked, this, &MainWindow::approveSelectedDelivery);

    stackedScreens->insertWidget(ScreenIndex::Deliveries, w);
}

void MainWindow::loadDeliveriesData()
{
    tableDeliveries->setRowCount(0);

    vector<clsDelivery> vDeliveries = clsDelivery::GetDeliveriesList();

    for (size_t i = 0; i < vDeliveries.size(); i++) {
        tableDeliveries->insertRow(i);
        tableDeliveries->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(vDeliveries[i].DeliveryID())));
        tableDeliveries->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(vDeliveries[i].SupplierID())));
        tableDeliveries->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(vDeliveries[i].Date())));
        tableDeliveries->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(vDeliveries[i].ProductID())));
        tableDeliveries->setItem(i, 4, new QTableWidgetItem(QString::number(vDeliveries[i].Quantity())));

        // Color Code the Status
        QTableWidgetItem *statusItem = new QTableWidgetItem();
        statusItem->setFont(QFont("Arial", 10, QFont::Bold));

        if (vDeliveries[i].Status() == clsDelivery::enStatus::Approved) {
            statusItem->setText("Approved");
            statusItem->setForeground(QBrush(QColor("#27ae60"))); // Green
        } else if (vDeliveries[i].Status() == clsDelivery::enStatus::Rejected) {
            statusItem->setText("Rejected");
            statusItem->setForeground(QBrush(QColor("#e74c3c"))); // Red
        } else {
            statusItem->setText("Pending");
            statusItem->setForeground(QBrush(QColor("#f39c12"))); // Orange
        }

        tableDeliveries->setItem(i, 5, statusItem);
    }
}

void MainWindow::approveSelectedDelivery()
{
    int row = tableDeliveries->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Select Delivery", "Please select a pending delivery to approve.");
        return;
    }

    // 1. Check if it is already approved
    QString currentStatus = tableDeliveries->item(row, 5)->text();
    if (currentStatus == "Approved") {
        QMessageBox::information(this, "Already Approved", "This delivery has already been processed into inventory.");
        return;
    }

    // 2. Read the data from the table
    string deliveryID = tableDeliveries->item(row, 0)->text().toStdString();
    string supplierID = tableDeliveries->item(row, 1)->text().toStdString();
    string productID  = tableDeliveries->item(row, 3)->text().toStdString();
    double quantity   = tableDeliveries->item(row, 4)->text().toDouble();

    // ==========================================
    // Core Logic 1: Inventory += Quantity
    // ==========================================
    clsProduct product = clsProduct::Find(productID);
    if (!product.IsEmpty()) {
        product.SetStockQuantity(product.StockQuantity() + quantity);
        product.Save();
    }

    // ==========================================
    // Core Logic 2: Supplier Points += Points
    // ==========================================
    clsSupplier supplier = clsSupplier::Find(supplierID);
    if (!supplier.IsEmpty()) {
        // Assume 5 points per KG delivered
        int calculatedPoints = quantity * 5;
        supplier.SetPoints(supplier.Points() + calculatedPoints);
        supplier.Save();
    }

    // ==========================================
    // Core Logic 3: Update Delivery Status
    // ==========================================
    clsDelivery delivery = clsDelivery::Find(deliveryID);
    if (!delivery.IsEmpty()) {
        delivery.SetStatus(clsDelivery::enStatus::Approved);
        delivery.Save();
    }

    // 4. Update the UI directly
    QTableWidgetItem *statusItem = tableDeliveries->item(row, 5);
    statusItem->setText("Approved");
    statusItem->setForeground(QBrush(QColor("#27ae60"))); // Green

    QMessageBox::information(this, "Delivery Approved",
        QString("Success! Added %1 kg to inventory and awarded points to the supplier.").arg(quantity));

    // Refresh the dashboard to show the new inventory total
    refreshDashboard();
}
