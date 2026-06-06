#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QTabWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navigation & Auth
    void handleLogin();
    void logout();
    void navigateToScreen(int index);

    // Data Management Slots
    void refreshDashboard();
    void loadUsersData();
    void deleteSelectedUser();
    void loadSuppliersData();
    void deleteSelectedSupplier();
    void loadCustomersData();
    void deleteSelectedCustomer();
    void loadProductsData();
    void deleteSelectedProduct();
    void loadOrdersData();
    void cancelSelectedOrder();
    void loadInventoryData();
    void loadArticlesData();
    void deleteSelectedArticle();

private:
    // --- Core Layout Components ---
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;

    QWidget *sidebarWidget;
    QVBoxLayout *sidebarLayout;
    QStackedWidget *stackedScreens;


    // --- Sidebar Buttons ---
    QPushButton *btnDashboard, *btnUsers, *btnSuppliers, *btnCustomers;
    QPushButton *btnProducts, *btnOrders, *btnInventory, *btnReports;
    QPushButton *btnArticles, *btnLogout;
    QPushButton *btnDeliveries;

    enum ScreenIndex {
        Login = 0, Dashboard, Users, Suppliers, Customers,
        Products, Orders, Inventory, Reports, Articles,Deliveries
    };

    // --- Core Setup Functions ---
    void setupUi();
    void setupSidebar();
    QTableWidget* createStandardTable(QStringList headers); // UI Optimization Helper
    void applyRolePermissions(int role);
    // --- The 11 Screens ---
    void setupLoginScreen();
    void setupDashboardScreen();
    void setupUsersScreen();
    void setupSuppliersScreen();
    void setupCustomersScreen();
    void setupProductsScreen();
    void setupOrdersScreen();
    void setupInventoryScreen();
    void setupReportsScreen();
    void setupArticlesScreen();
    void loadAllReportsData();
    void setupDeliveriesScreen();
    void loadDeliveriesData();
    void approveSelectedDelivery();
    // --- UI Elements ---
    QLineEdit *txtUsername, *txtPassword;
    QLabel *lblRevenue, *lblBones, *lblTotalUsers, *lblTotalOrders,*lblTotalDeliveries;

    QTableWidget *tableUsers;
    QTableWidget *tableSuppliers;
    QTableWidget *tableCustomers;
    QTableWidget *tableProducts;
    QTableWidget *tableOrders;
    QTableWidget *tableInventory;
    QTableWidget *tableArticles;
    QTableWidget *tableDeliveries;
    // --- Dashboard Elements ---
    QLabel *lblDashSuppliers, *lblDashCustomers, *lblDashProducts;
    QLabel *lblDashOrders, *lblDashBones, *lblDashRevenue,*lblDashDeliveries;

    QPushButton *btnQuickAddSupplier, *btnQuickAddProduct;
    QPushButton *btnQuickAddArticle, *btnQuickReports;

    QTableWidget *tableDashLowStock;

    // --- Add this near your other table pointers ---
    QComboBox *cmbInventoryFilter;


    // --- Reports Dashboard Elements ---
    QTabWidget *tabReports;
    QTableWidget *tableRepSales;
    QTableWidget *tableRepInventory;
    QTableWidget *tableRepSuppliers;
    QTableWidget *tableRepCustomers;
    QTableWidget *tableRepDeliveries;
};
